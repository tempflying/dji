#ifndef OURSTABLEACTION_H
#define OURSTABLEACTION_H

#include "OurAction.h"

class OurStableAction : public OurAction{
public:
    int waitFrameCount;
    OurStableAction(int waitFrameCount=30){
        count = 0;
        this->waitFrameCount = waitFrameCount;
    }

    int count;
    bool stepOver(Flight* flight){
        count ++;
        if(count >= waitFrameCount) return true;
        uint8_t flag = Flight::HORIZONTAL_VELOCITY | Flight::VERTICAL_VELOCITY | Flight::YAW_PALSTANCE
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_DISABLE;
        flight->control(flag, 0, 0, 0, 0);
        return false;
    }
};

#endif // OURSTABLEACTION_H
