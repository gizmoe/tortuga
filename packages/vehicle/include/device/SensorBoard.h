/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SensorBoard.h
 */

#ifndef RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2007
#define RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IDepthSensor.h"

#include "core/include/Event.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

#include "sensorapi-r5/include/sensorapi.h"

namespace ram {
namespace vehicle {
namespace device {

/** This class encompasses all comunication with the physical sensor board
 *
 *  This allows the the vehicle code to remain as backend agnostic as possible
 *  by providing services to the other IDevice objects the vehicle current has.
 *  The vehicle is not supposed to talk with device directly.
 */
class SensorBoard : public Device, // for getName, boost::noncopyable
                    public IDepthSensor
{
public:
    /**
     * @defgroup Events SensorBoard Events
     */
    /* @{ */
    static const core::Event::EventType DEPTH_UPDATE;
    /* @{ */
    
    /** Creates a device with the given file descriptor */
    SensorBoard(int deviceFD,
                core::EventHubPtr eventHub = core::EventHubPtr());
    
    /** Create a SensorBoard from the config file with the given vehicle */
    SensorBoard(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr(),
                IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SensorBoard();

    // IDevice interface methods
    virtual std::string getName() { return Device::getName(); }

    // IUpdatable interface methods
    
    /** Does a single iteration of the communication with the sensor board
     *
     *  Each iteration sends a set of thruster commands, get a depth and 1/10
     *  of all the other telemetry the board provides. So after ten update
     *  calls a complete set of telemetry will be returned by the board.
     */
    virtual void update(double timestep);

    virtual void background(int interval) {
        Updatable::background(interval);
    };
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };

    // IDepth interface methods
    virtual double getDepth();

    // The class methods

    /** Sets the commanded thruster value
     *
     *  @param address
     *      The number of the thruster (1-6)
     *  @param count
     *      The value to command the thruster (-1024 to 1023)
     */
    virtual void setThrusterValue(int address, int count);

    /** Returns true if the thruster will repond to thruster commands
     *
     *  @param address
     *      The number of the thruster (1-6)
     */
    virtual bool isThrusterEnabled(int address);
    
private
    struct VehicleState
    {
        double depth;
        struct boardInfo telemetry;
    };
    
    /** The device file to open the file descriptor from */
    std::string m_deviceFile;
    
    /** The file descriptor which is connected to the SB's USB port */
    int m_deviceFD;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SENSORBOARD_06_04_2007
