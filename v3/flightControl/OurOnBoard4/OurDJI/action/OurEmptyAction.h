#ifndef OUREMPTYACTION_H
#define OUREMPTYACTION_H

#include "../OurDJI.h"

class OurEmptyAction : public OurAction{
protected:
    int waitFrameCount;
    int count;
public:
    OurEmptyAction(int waitFrameCount = 10){
        this->waitFrameCount = waitFrameCount;
        count = 0;
    }

    virtual bool stepOver(Flight* flight){
        count ++;
        if(count <= waitFrameCount) return false;
        return true;
    }

};

#endif // OUREMPTYACTION_H
