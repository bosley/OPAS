#include <EnDecode.h>

/*

    Converts rc commands from to quad copter specific commands

*/

#ifndef AV_QUAD_COMMAND_CONVERTER_HEADER_GUARD
#define AV_QUAD_COMMAND_CONVERTER_HEADER_GUARD

namespace VEHICLE { namespace AV { namespace QUADCOPTER {

    struct cmd
    {
        float roll;       // Requested change roll
        float pitch;      // Requested change pitch
        float yaw;        // Requested change yaw
        float zAxis;      // Vertical
        bool button0;     // Custom button command 0 
        bool button1;     // Custom button command 1
    };

    // --------------------------------------

    inline cmd commToCmd(COMM::wData data)
    {
        cmd command = {0.0, 0.0, 0.0, false, false};



        //
        // Left Joystick
        //
        if(data.joystick == 0)
        {
            // Pitch
            if(data.control == 0)
            {
                if(data.value > 127)
                {
                    // + Pitch (Move Forward)
                    command.pitch = 35.0;         // Angle drone +35 degrees
                }
                else
                {
                    // - Pitch (Move Reverse)
                    command.pitch = -35.0;         // Angle drone -35 degrees
                }
            }
            // Roll
            else if (data.control == 1)
            {
                if(data.value > 127)
                {
                    // - Roll (Move left)
                    command.roll = -35.0;           // Angle drone -35 degrees
                }
                else
                {
                    // + Roll (Move right)
                    command.roll = 35.0;           // Angle drone +35 degrees
                }
            }
            else
            {
                command.button0 = true;
            }
        }

        //
        // Right Joystick
        //
        if(data.joystick == 1)
        {
            // Yaw
            if(data.control == 0)
            {
                if(data.value > 127)
                {
                    command.yaw = -10.0;        // Spin right
                }
                else
                {
                    command.yaw = 10.0;         // Spin left
                }
            }
            // Z 
            else if (data.control == 1)
            {
                if(data.value > 127)
                {
                    command.zAxis = 1.0;        // Increase z axis by 1 unit
                }
                else
                {
                    command.zAxis = -1.0;        // Decrease z axis by 1 unit
                }
            }
            else
            {
                command.button1 = true;
            }
        }


        return command;
    }

}}} 


#endif