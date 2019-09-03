#include <Arduino.h>
#include <VehicleIf.h>
#include <IMU/DFRobotIMU.h>
#include <IMU/IMUCallbackIf.h>
#include <Servo.h>

#define SERVO_ONE   2   // Front right
#define SERVO_TWO   3   // Back  right
#define SERVO_THREE 4   // Front left
#define SERVO_FOUR  5   // Back  left

namespace VEHICLE
{
    class QuadCopter : public VehicleIf, public SENSORS::IMU::CallbackIf
    {
    public:
        QuadCopter();
        ~QuadCopter();

        // From vehicleIf
        virtual void tick() override;
        virtual void handleRcData(COMM::wData data) override;

        // From Imu CallbackIf
        virtual void yaw(double z, double angVelocity) override;
        virtual void pitch(double x, double angVelocity) override;
        virtual void roll(double y, double angVelocity) override;
        virtual void temperature(double c) override;
        virtual void pressure(double pa) override;
        virtual void altitude(double m) override;

    private:
        
        enum class State
        {
            INIT,
            STANDBY,
            FLIGHT,
            ERROR
        };

        State currentState;

        Servo FL, FR, BL, BR; // Front Left, Front Right, Back Left, Back Right
        
        // The IMU device
        SENSORS::IMU::DFRobotIMU *imu;
    
        void readyMotors();
    
    };
    
} // VEHICLE
