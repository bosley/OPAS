
'''

    Josh A. Bosley 
    The device router serves as a means to interact with devices in a 
    signal/slot-like relation. The device monitor thread of XNS will
    routinely run each device's methods stored in their respective
    routine dictionaries. If an action is meant to trigger a resoinse,
    the method will return a command string back to the device monitor to
    carry out the action.
    When requested, xns will run the device's methods stored in their slot 
    dictionaries as-to allow remote control off a device (From another node.)

    This file is rewritten when running the command : 

            ./configure source devices

'''

from .light_switch import routines as light_switch_routines
from .light_switch import slots as light_switch_slots
from .motion_sensor import routines as motion_sensor_routines
from .motion_sensor import slots as motion_sensor_slots
from .temp_sensor import routines as temp_sensor_routines
from .temp_sensor import slots as temp_sensor_slots


devRouter = {
	"light_switch": (light_switch_routines, light_switch_slots),
	"motion_sensor": (motion_sensor_routines, motion_sensor_slots),
	"temp_sensor": (temp_sensor_routines, temp_sensor_slots),
}


