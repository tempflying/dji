#ifndef OURMOVEBYACTION_H
#define OURMOVEBYACTION_H

#include "OurAction.h"

/*
    移动相应距离的action
*/

// 每次获取位置信息,再用位置信息更新还需要移动的位移,使用POSITION模式进行飞行

class OurMoveByUsePositionAction : public OurAction{
public:
    float64_t err;
    FlightData flightData;
    PositionData beginPos_r;
    PositionData tag_offset_m;          // 目标位移

/*    OurMoveByUsePositionAction(float64_t error=0.3){
        err = error;
    }
*/
    OurMoveByUsePositionAction(float64_t x, float64_t y, float32_t z, float64_t error = 0.3){
        offset_m.latitude = x;
        offset_m.longitude = y;
        offset_m.altitude = z;
        err = error;
        tag_offset_m  = offset_m;

        isFirstStep = true;
    }

    OurMoveByUsePositionAction(PositionData offset_m, float64_t error = 0.3){
        this->offset_m = offset_m;
        err = error;
        tag_offset_m  = offset_m;
        isFirstStep = true;
    }

    void init(Flight* flight){
        beginPos_r = flight->getPosition();
    }

    bool isFirstStep;
    PositionData offset_m;              // 还剩余的位移
    // 返回是否结束
    bool stepOver(Flight* flight){
        if(isFirstStep) {
            init(flight);
            isFirstStep = false;
        }
        printf("offset_m: %f %lf %lf ++++++++++++++++++++++++++++++++++++++++++++++\n",
               offset_m.altitude,offset_m.latitude,offset_m.longitude);
        if(calLengthInMeter(offset_m) < err)
        {
            flightData.flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE; //Flight::YAW_BODY == SMOOTH_ENABLE
            flightData.x = 0;
            flightData.y = 0;
            flightData.z = 0;
            flightData.yaw = 0;
            flight->setFlight(&flightData);
            return true;
        }else{
            flightData.flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_POSITION | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
            flightData.x = offset_m.latitude;
            flightData.y = offset_m.longitude;
            flightData.z = beginPos_r.altitude + tag_offset_m.altitude; // vertical 是相对于地面的
            flightData.yaw = 0;
            flight->setFlight(&flightData);

            offset_m = calOffsetInMeter(beginPos_r, flight->getPosition()); // 已经移动了的位移
            offset_m.altitude = tag_offset_m.altitude - offset_m.altitude;
            offset_m.latitude = tag_offset_m.latitude - offset_m.latitude;
            offset_m.longitude = tag_offset_m.longitude - offset_m.longitude;
            return false;
        }
    }
};

#endif // OURMOVEBYACTION_H
