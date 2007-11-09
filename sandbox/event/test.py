# STD Imports
import unittest

# Project Imports
import event
import props
import state

class Reciever(object):
    def __init__(self):
        self.etype = None
        self.sender = None
        self.args = None
        self.called = False
    
    def __call__(self, etype, sender, args):
        self.etype = etype
        self.sender = sender
        self.args = args
        self.called = False

class TestEventManager(unittest.TestCase):
    
    def setUp(self):
        self.emgr = event.EventManager()

    def testEventArgs(self):
        args = event.EventArgs(Hello = 10, Bob = "John")
        self.assertEqual(10, args.Hello)
        self.assertEqual("John", args.Bob)

        s = set([event.EventArgs(a = 1), event.EventArgs(a = 1)])
        self.assertEquals(1, len(s))

        s = set([event.EventArgs(a = 1), event.EventArgs(b = 2)])
        self.assertEquals(set([event.EventArgs(a = 1), event.EventArgs(b = 2)]),
                          s)

    def testSend(self):
        # Handler for the function
        reciever = Reciever()

        # Register function to recieve the event
        self.emgr.subscribe("TestEvent", reciever)

        # Send message to that function
        self.emgr.publish("TestEvent", self, testArg = 5, other = "A");

        # Make sure the event got through
        def check():
            self.assertEquals(reciever.sender, self)
            self.assertEquals(reciever.eventType, "TestEvent")
            self.assertEquals(5, reciever.args.testArg)
            self.assertEquals("A", reciever.args.other)
        check()

        # Run a second event to make sure the events aren't cross fed
        self.emgr.publish("AnotherEvent", self, bob = "John")

    def testNone(self):
        self.emgr.publish("TestEvent", None)

    def testMultiple(self):
        self.senders = set()
        self.etypes = set()
        self.args = set()
        self.aCalled = False
        self.bCalled = False
        
        def factory(signal):
            def recieve(eventType_, sender_, args_):
                self.etypes.add(eventType_)
                self.senders.add(sender_)
                self.args.add(args_)
                # Ensure that each makes a unique action, so we can be sure
                # we have two distinct functions
                setattr(self, signal, True)
            return recieve
            
        self.emgr.subscribe("TestEvent", factory("aCalled"))
        self.emgr.subscribe("TestEvent", factory("bCalled"))
        self.emgr.publish("TestEvent", self, test = 1984)
        self.emgr.publish("AnotherEvent", self, other = "10")

        self.assertEquals(self.senders, set([self]))
        self.assertEquals(self.etypes, set(["TestEvent"]))
        self.assertEquals(self.args, set([event.EventArgs(test = 1984)]))
        self.assert_(self.aCalled)
        self.assert_(self.bCalled)

class Reciever(object):
    def __init__(self):
        self.eventType = None
        self.sender = None
        self.args = None
        self.called = False
    
    def __call__(self, etype, sender, args):
        self.eventType = etype
        self.sender = sender
        self.args = args
        self.called = False

class TestQueuedEventManager(unittest.TestCase):
    def test(self):
        emgr = event.EventManager()
        recv = Reciever()

        # Create a queued event manager to listen queue up recieved events
        qemgr = event.QueuedEventManager(emgr)
        qemgr.subscribe("TestEvent", recv)

        # Publish a message and make sure its not recieved
        emgr.publish("TestEvent", self, a = 10)
        self.assertEquals(False, recv.called)

        # Release all messages
        qemgr.processMessages()

        self.assertEquals("TestEvent", recv.eventType)
        self.assertEquals(10, recv.args.a)
        self.assertEquals(self, recv.sender)
        
class PropClass(props.PropertyClass):
    def __init__(self, myProp, myReadOnly):
        props.PropertyClass.__init__(self)

        self.myProp = myProp
        self.myReadOnly = myReadOnly

        self.addProperty('MyProp', self.getMyProp, self.setMyProp)
        self.addProperty('ReadOnly', self.getMyReadOnly)

    def getMyProp(self):
        return self.myProp
    
    def setMyProp(self, myProp):
        self.myProp = myProp
        
    def getMyReadOnly(self):
        return self.myReadOnly


class TestProperty(unittest.TestCase):
    def testBasic(self):
        obj = PropClass(10, "John")

        self.assertEquals(10, obj.getProperty('MyProp').value())
        self.assertEquals('John', obj.getProperty('ReadOnly').value())

        myProp = obj.getProperty('MyProp')
        self.assertEquals(10, myProp.value())
        myProp.set(287)
        self.assertEqual(287, myProp.value())
        self.assertEqual(287, obj.getProperty('MyProp').value())
        self.assertEqual(287, obj.myProp)

# Test States (Consider Magic base class to take care of the init method)
class TrackedState(state.State):
    def __init__(self, machine):
        state.State.__init__(self, machine)

    def enter(self):
        self.entered = True

    def exit(self):
        self.exited = True

class Start(TrackedState):
    def __init__(self, machine):
        TrackedState.__init__(self, machine)
        self.sender = None
        self.func = None
        
    @staticmethod
    def transitions():
        return { "Start" : End,
                 "Change" : Simple,
                 "LoopBack" : LoopBack }

    def Start(self, sender, args):
        self.sender = sender
        self.args = args

class Simple(state.State):
    pass

class LoopBack(TrackedState):
    def __init__(self, machine):
        TrackedState.__init__(self, machine)
        self.count = 0
        'MYSTATS',dir(self)

    @staticmethod
    def transitions():
        return { "Update" : LoopBack }

    def Update(self, sender, args):
        self.count += 1

class End(TrackedState):
    pass

class TestStateMachine(unittest.TestCase):
    def setUp(self):
        self.machine = state.Machine(Start)

    def testbasic(self):
        # Check to make sure we get the default
        cstate = self.machine.currentState()
        self.assertEquals(Start, type(cstate))

    def testStart(self):
        self.machine.start()
        cstate = self.machine.currentState()
        self.assert_(cstate.entered)
        
    def testInjectEvent(self):
        startState = self.machine.currentState()

        self.machine.injectEvent("Start", self, 1)
        cstate = self.machine.currentState()

        # Check to me sure we left the start state
        self.assert_(startState.exited)

        # Check to make sure we reached the proper state
        self.assertEquals(End, type(cstate))
        self.assert_(cstate)

        # Make sure the transition function was called
        self.assertEquals(startState.sender, self)
        self.assertEquals(startState.args, 1)

    def testSimple(self):
        self.machine.injectEvent("Change", self, 1)
        cstate = self.machine.currentState()
        self.assertEquals(Simple, type(cstate))

    def testLoop(self):
        self.machine.injectEvent("LoopBack", self, 0)
        cstate = self.machine.currentState()
        self.assertEquals(LoopBack, type(cstate))

        cstate = self.machine.currentState()
        self.assert_(cstate.entered)

        # Make  A Loopback
#        self.machine.injectEvent("Update", self, 0)
#        cstate = self.machine.currentState()
#        self.assertEquals(LoopBack, type(cstate))
#        print 'TEST',dir(cstate)
#        self.assert_(cstate.exited)


if __name__ == '__main__':
    unittest.main()

