#ifndef OURDJIFSMPIPELINE_H
#define OURDJIFSMPIPELINE_H
#include "OurDJIPipelineInterface.h"

/*
*   相对OurDJIPipelineInterface 的改变
*     update函数按照状态拆开了成若干个状态函数，这些状态函数可以被直接继承重写,而不用修改update函数
*       同时update被修改为私有的不可再重写的
*     添加了 保证自动 起飞和降落 成功的判断条件 参考whenStartTakeOff 和 whenStartLanding
*/

class OurDJIFSMPipeline : public OurDJIPipelineInterface
{
public:
    OurDJIFSMPipeline(){
        initAutoFlightSafeCondition();
        updateCallTimes = 0;
        minTakeOffTime = 6000; // 必须起飞完成一段时间后才能降落，设置小了可能无法降落
        minLanddingTime = 4000;
    }

protected:

    long updateCallTimes;

    virtual void whenActived(){
        setControl(OBTAIN_CONTROL);
        ourCurrentState = OUR_WAITTING;
    }

    virtual void whenObtainControlRunning(){
        ourCurrentState = OUR_WAITTING;
    }

    virtual void whenObtainControlSuccess(){
        takeoff();
        ourCurrentState = OUR_TAKEOFF;
    }

    // 这些变量只能用在自动飞行控制上,在 initAutoFlightSafeCondition中被初始化
    // takeoff 和landdig会调用这个初始化函数
    float32_t lastAutoFlightAltitude;               // 自动飞行进行时,上一帧时所在的海拔,单位是米
    int autoFlightTimer;                            // 已经自动飞行了的时间,单位都是毫秒
    int autoFlightSecondTimer;                      // 自动飞行读秒器,单位都是毫秒
    int minTakeOffTime;                             // 起飞保证持续的最小时间,单位都是毫秒
    int minLanddingTime;                            // 降落保证最少需要的时间,单位都是毫秒
    // 飞机成功起飞降落的条件是 自动飞行时间超过minTakeOffTime/minLanddingTime,并且两秒内发生的位移几乎为零
    virtual void whenStartTakeOff(){
        if(autoFlightTimer > minTakeOffTime && autoFlightSecondTimer > 1000){
            if(floatEqual(flight->getPosition().altitude,lastAutoFlightAltitude)){
                ourCurrentState = OUR_MAINFLIGHT;
            }
            autoFlightSecondTimer = 0;
            lastAutoFlightAltitude = flight->getPosition().altitude;
        }
        autoFlightSecondTimer += interval;
        autoFlightTimer += interval;
    }
    virtual void whenStartLanding(){
        if(autoFlightTimer > minLanddingTime && autoFlightSecondTimer > 2000){
            if(floatEqual(flight->getPosition().altitude, lastAutoFlightAltitude)){
                setControl(RELEASE_CONTROL);
                ourCurrentState = OUR_WAITTING;
            }
            autoFlightSecondTimer = 0;
            lastAutoFlightAltitude = flight->getPosition().altitude;
        }
        autoFlightSecondTimer += interval;
        autoFlightTimer += interval;
    }

    virtual void whenReleaseControlRunning(){
        ourCurrentState = OUR_WAITTING;
    }

    virtual void whenReleaseControlSuccess(){
        closePort();
        ourCurrentState = OUR_WAITTING;
        endProgram();
    }

    // 新线程中运行命令的方法
    virtual void whenInMainFlight(){
        // 异步线程执行mainFlight
//        if(!createThread(OurDJIPipelineInterface::mainFlightThread, this)){
//            landing();
//            ourCurrentState = OUR_LANDDING;
//        }
//        ourCurrentState = OUR_WAITTING;

        // 有限状态机执行 按帧执行 mainFlight
        landing();
        ourCurrentState = OUR_LANDDING;
    }

private:
    void update(){
        if(updateCallTimes % 10 == 0)
            printf("ourcurrentState: %d, updateCallTimes: %d, autoFlightTimer: %d\n", int(ourCurrentState), int(updateCallTimes), autoFlightTimer);

        switch(ourCurrentState){            // 当前所处的状态

        case OUR_WAITTING:
            break;

        case OUR_BEGIN:                     // 暂时无用 1
            ourCurrentState = OUR_WAITTING;
            break;

        case OUR_ACTIVATED:                 // 刚刚激活成功 2
            whenActived();
            break;

        case OUR_OBTAINCONTROL_RUNNING:     // 控制权第一步成功 3
            whenObtainControlRunning();
            break;

        case OUR_OBTAINCONTROL_SUCCESS:     // 获得控制权成功 4
            whenObtainControlSuccess();
            break;

        case OUR_TAKEOFF:                   // 起飞中 5
            whenStartTakeOff();
            break;

        case OUR_MAINFLIGHT:                // 核心飞控程序 6
            whenInMainFlight();
            break;

        case OUR_LANDDING:                  // 降落中 7
            whenStartLanding();
            break;

        case OUR_RELEASECONTROL_RUNNING:    // 释放控制权第一步成功
            whenReleaseControlRunning();
            break;

        case OUR_RELEASECONTROL_SUCCESS:    // 释放控制权成功
            whenReleaseControlSuccess();

            break;
        }
        updateCallTimes ++;
    }

    void endProgram(){
        printf("Done 8------------------------------\n");
        //QEventLoop::exit(0);
        exit(0);
    }

    bool floatEqual(float a, float b, float err=0.0001){
        return abs(a - b) < err;
    }

    void initAutoFlightSafeCondition(){
        OurDJIPipelineInterface::initAutoFlightSafeCondition();
        lastAutoFlightAltitude = flight->getPosition().altitude;
        autoFlightSecondTimer = 0;
        autoFlightTimer = 0;
    }
};

#endif // OURDJIFSMPIPELINE_H
