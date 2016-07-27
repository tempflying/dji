#ifndef OUREMPTYACTION_H
#define OUREMPTYACTION_H

#include "OurAction.h"

// 空动作,什么都不做的Action,一般用于等待

class OurEmptyAction : public OurAction{
protected:
    int waitFrameCount;
    int count;
public:
    OurEmptyAction():waitFrameCount(10){
        count = 0;
    }

    virtual bool stepOver(Flight* flight){
        count ++;
        if(count <= waitFrameCount) return false;
        return true;
    }

};

#endif // OUREMPTYACTION_H
