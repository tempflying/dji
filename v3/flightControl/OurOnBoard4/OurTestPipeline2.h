#ifndef OURTESTPIPELINE2_H
#define OURTESTPIPELINE2_H

#include "OurDJI/pipeline/OurDJIFSMPipeline.h"


class OurTestPipeline2 : public OurDJIFSMPipeline{
public:
    OurTestPipeline2(){
        count = 0;
        flag = Flight::HORIZ_POS_VERT_VEL_YAW_ANG
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
    }
protected:
    int count;
    uint8_t flag;

    virtual void whenInMainFlight(){
        count++;
        if(count <= 90){
            flight->control(flag, 5, 5, 1, 0);

        }else if(count <= 100){
            flight->control(flag, 0, 0, 0, 0);
        }else{
            landing();
            ourCurrentState = OUR_LANDDING;
        }
    }
};

#endif // OURTESTPIPELINE2_H
