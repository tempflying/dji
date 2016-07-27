#ifndef OURMOVETOAVTION_H
#define OURMOVETOAVTION_H

#include "OurAction.h"

/*
 *  移动到某个点的Action
 *
*/

class OurMoveToUsePositionAction : public OurAction{
public:
    PositionData tagPos_r;
    float64_t err;
    FlightData flightData;

    OurMoveToUsePositionAction(float64_t error=0.3){
        err = error;
    }

    OurMoveToUsePositionAction(float64_t latitude, float64_t longitude, float32_t altitude, float64_t error = 0.3){
        tagPos_r.latitude = latitude;
        tagPos_r.longitude = longitude;
        tagPos_r.altitude = altitude;
        err = error;
    }

    OurMoveToUsePositionAction(PositionData tagPos_r, float64_t error = 0.3){
        this->tagPos_r = tagPos_r;
        err = error;
    }

    PositionData currentPos_r;
    PositionData offset_m;
    // 返回是否结束
    bool stepOver(Flight *flight){
        currentPos_r = flight->getPosition();
        offset_m = calOffsetInMeter(currentPos_r, tagPos_r);

        if(calLengthInMeter(offset_m) < err)
        {
            flightData.flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
            flightData.x = 0;
            flightData.y = 0;
            flightData.z = 0;
            flightData.yaw = 0;
            flight->setFlight(&flightData);
            return true;
        }

        flightData.flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_POSITION | Flight::YAW_ANGLE
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
        flightData.x = offset_m.latitude;
        flightData.y = offset_m.longitude;
        flightData.z = tagPos_r.altitude;
        flightData.yaw = 0;
        flight->setFlight(&flightData);
        return false;
    }

};

#endif // OURMOVETOAVTION_H
