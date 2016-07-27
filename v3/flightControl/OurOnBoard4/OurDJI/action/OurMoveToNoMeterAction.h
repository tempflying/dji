#ifndef OURMOVETONOMETERACTION_H
#define OURMOVETONOMETERACTION_H

#include "OurAction.h"

class OurMoveToNoMeterAction : public OurAction{

    float64_t x;
    float64_t y;
    float32_t zv;
    uint8_t flag;
    int count;
public:
    OurMoveToNoMeterAction(float64_t xx, float64_t yy, float32_t zzv){
        x = xx;
        y = yy;
        zv = zzv;
        count = 0;
        flag = Flight::HORIZ_POS_VERT_VEL_YAW_ANG
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
    }

    bool stepOver(Flight* flight){
        count++;
        if(count <= 90){
            flight->control(flag, x, y, zv, 0);
            return false;
        }else if(count <= 100){
            flight->control(flag, 0, 0, 0, 0);
            return false;
        }
        return true;
    }
};

#endif // OURMOVETONOMETERACTION_H
