#ifndef OURTESTPIPELINE_H
#define OURTESTPIPELINE_H

#include "../OurDJI.h"

class OurActionTest : public OurAction{
public:
    OurActionTest(){
        count = 0;
        flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
    }

    uint8_t flag;
    int count;
    bool stepOver(Flight* flight){
        switch (count / 100) {
        case 0:
            flight->control(flag, 10, 0, 0, 0);
            break;

        case 1:
            flight->control(flag, 0, 10, 0, 0);
            break;

        case 2:
            flight->control(flag, -10, 0, 0, 0);
            break;

        case 3:
            flight->control(flag, 0, -10, 0, 0);
            break;

        default:
            return true;
        }
        count ++;
        return false;
    }
};

class OurActionTest2 : public OurAction{
public:
    OurActionTest2(){
        count = 0;
    }

    int count;
    bool stepOver(Flight* flight){
        if(count < 100){
            uint8_t flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
            flight->control(flag, 5, 0, 0, 0);
        }
        count++;
        if(count <= 100) return false;
        else return true;
    }
};

class OurTestPipeline : public OurDJIFSMPipeline{
public:

#define MOVECOUNT 10
    OurAction* action[MOVECOUNT];
    int actionCount;
    OurTestPipeline(){
        int i = 0;
        action[i++] = new OurMoveByUsePositionAction(0, 0, 10);
        //action[i++] = new OurMoveByAction(5, 0 , 0);
        //action[i++] = new OurMoveByAction(0, 5, 0);
        //action[i++] = new OurMoveByAction(-5, 0, 0);
        //action[i++] = new OurMoveByAction(0, -5, 0);

        action[i++] = new OurActionTest();

        //action[i++] = new OurActionTest2();
        actionCount = i;
        currentTaskId = 0;
    }

protected:

    int currentTaskId;
    virtual void whenInMainFlight(){
//        if(!createThread(OurDJIPipelineInterface::mainFlightThread, this)){
//            landing();
//            times = 0;
//            ourCurrentState = OUR_LANDDING;
//        }
//        ourCurrentState = OUR_WAITTING;


//         帧模式
        if(currentTaskId < actionCount){
            if(action[currentTaskId]->stepOver(flight)){
                currentTaskId ++;
                //getchar();
            }
        }
        else{
            landing();
            ourCurrentState = OUR_LANDDING;
        }
    }

    // 为了使用多线程而存在，如果使用帧模式代替了多线程，这个就用不到了
//    virtual void mainFlight(){

//    }

//    void mobileTest(){
//        if(updateCallTimes * interval % 3000 == 0){
//            char* data = (char*) std::string("helloonboard").c_str();
//            sendDataTOMobile(data, 12);
//            char* receivedata = new char[101];
//            getReceiveDataFromMobile();
//            if(receivedata)
//                printf("--------------------------------------------- receive data %s\n", receivedata);
//            getchar();
//        }
//    }

};

#undef MOVECOUNT

#endif // OURTESTPIPELINE_H
