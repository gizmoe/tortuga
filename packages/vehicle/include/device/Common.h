/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Common.h
 */

#ifndef RAM_VEHICLE_DEVICE_COMMON_06_11_2007
#define RAM_VEHICLE_DEVICE_COMMON_06_11_2007

#include <boost/shared_ptr.hpp>

namespace ram {
namespace vehicle {
namespace device {
    
// Forward Declartions
class IDevice;
typedef boost::shared_ptr<IDevice> IDevicePtr;

class Device;
typedef boost::shared_ptr<Device> DevicePtr;

class Thruster;
typedef boost::shared_ptr<Thruster> ThrusterPtr;

class ThrusterCommand;
typedef boost::shared_ptr<ThrusterCommand> ThrusterCommandPtr;


namespace details {
/** To expose things to GCC-XML */
inline int instantiate()
{
    int a = 0;
    a += sizeof(boost::shared_ptr<IDevice>);
    a += sizeof(boost::shared_ptr<IDevice>);
    a += sizeof(boost::shared_ptr<Thruster>);
    a += sizeof(boost::shared_ptr<ThrusterCommand>);
    return a;
}
}
    
} // namespace device
} // namespace vehicle
} // namespace ram



#endif // RAM_VEHICLE_DEVICE_COMMON_06_11_2007
