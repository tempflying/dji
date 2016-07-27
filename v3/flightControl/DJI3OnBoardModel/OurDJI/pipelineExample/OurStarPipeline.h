#ifndef OURSTARPIPELINE_H
#define OURSTARPIPELINE_H

#include "../OurDJI.h"
// data_type
#define fp32 float
#define fp64 double

//  注意所有带有isFirst在stepOver返回true时，都要把他们置为true
//  注意一个action对象使用完之后，最好将他们释放了，不要二次利用，因为可能有些参数不会重新初始化，后面会改成使用init初始化参数，重复利用前先调用init

// 绕raw轴旋转的Action
class OurYawRotateToAction: public OurAction{
    int waitFrameCount;
public:
#define MAXRATE 100
    float angleDegree;
    int interval; // 毫秒
    // angleDegree / rotateRate * frameRate ; rotateRate=【-100, 100】;frameRate
    // 认为他的平均角速度快于 MAXRATE/6
    OurYawRotateToAction(float32_t angleDegree, int interval){
        this->angleDegree = angleDegree;
        count = 0;
        this->interval = interval;
        waitFrameCount = sqrt(2 * angleDegree / (MAXRATE / 10)) * (1000 / interval);
        waitFrameCount = (waitFrameCount > 0) ? waitFrameCount : -waitFrameCount;
    }

    int count;

    bool stepOver(Flight *flight){
        count ++;
        if(count >= waitFrameCount) return true;
        uint8_t flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;//SMOOTH_ENABLE
        flight->control(flag, 0, 0, 0, angleDegree);
        return false;
    }
#undef MAXRATE
};

// 绘制五角星的Action
class OurStarAction :public OurAction{
public:
    typedef struct way_point
    {
        fp64 lati_r;
        fp64 longti_r;
        fp32 alti_m;
    }way_point_t;
    way_point_t way_point_data[5];

    void getWayPoint(Flight * flight){
        PositionData pos = flight->getPosition();
        DJI_CalPos(pos.latitude,pos.longitude,pos.altitude,\
                   0, 1.902113*5, 0,\
                   &way_point_data[0].lati_r,&way_point_data[0].longti_r,&way_point_data[0].alti_m);

        DJI_CalPos(way_point_data[0].lati_r,way_point_data[0].longti_r,way_point_data[0].alti_m,\
                   -1.1180339*5, -1.538842*5, 0,\
                   &way_point_data[1].lati_r,&way_point_data[1].longti_r,&way_point_data[1].alti_m);

        DJI_CalPos(way_point_data[1].lati_r,way_point_data[1].longti_r,way_point_data[1].alti_m,\
                   1.8090169*5, 0.587785*5, 0,\
                   &way_point_data[2].lati_r,&way_point_data[2].longti_r,&way_point_data[2].alti_m);

        DJI_CalPos(way_point_data[2].lati_r,way_point_data[2].longti_r,way_point_data[2].alti_m,\
                   -1.8090169*5, 0.587785*5, 0,\
                   &way_point_data[3].lati_r,&way_point_data[3].longti_r,&way_point_data[3].alti_m);

        DJI_CalPos(way_point_data[3].lati_r,way_point_data[3].longti_r,way_point_data[3].alti_m,\
                   1.1180339*5, -1.538842*5, 0,\
                   &way_point_data[4].lati_r,&way_point_data[4].longti_r,&way_point_data[4].alti_m);
    }

    bool isFirst;
    int count;
    OurStarAction(){
        isFirst = true;
        count = 0;
    }

    OurActionArray actionArray;

    bool stepOver(Flight* flight){
        if(isFirst){
            getWayPoint(flight);
            for(int i = 0; i < 5; i++)
            {
                actionArray.push_back(new OurMoveToUsePositionAction(way_point_data[i].lati_r, way_point_data[i].longti_r,\
                            way_point_data[i].alti_m,0.3));
                actionArray.push_back(new OurStableAction(30));
            }
            //actionArray.push_back(new OurYawRotateToAction(360, 20));
            isFirst = false;
        }

        if(actionArray.getCurrentAction()->stepOver(flight)){
            actionArray.currentActionId ++;
        }
        if(actionArray.currentActionId == actionArray.getActionCount()) return true;
        return false;
    }

    void DJI_CalOffset(const fp64 Cur_lati_r, const fp64 Cur_longti_r, const fp32 Cur_alti_m, \
                fp64* const lati_offset_m, fp64* const longti_offset_m, fp32* const alti_offset_m,\
                const fp64 Tar_lati_r, const fp64 Tar_longti_r, const fp32 Tar_alti_m)
    {
        *lati_offset_m = (Tar_lati_r - Cur_lati_r)*(double)6378137.0;
        *longti_offset_m = (Tar_longti_r - Cur_longti_r)*((double)6378137.0*cos(Cur_lati_r));
        *alti_offset_m = Tar_alti_m - Cur_alti_m;
    }

    void DJI_CalPos(const fp64 Cur_lati_r, const fp64 Cur_longti_r, const fp32 Cur_alti_m, \
                const fp64 lati_offset_m, const fp64 longti_offset_m, const fp32 alti_offset_m, \
                fp64* const Tar_lati_r, fp64* const Tar_longti_r, fp32* const Tar_alti_m)
    {
        *Tar_lati_r = Cur_lati_r + (lati_offset_m/(double)6378137.0);
        *Tar_longti_r = Cur_longti_r + (longti_offset_m/((double)6378137.0*cos(Cur_lati_r)));
        *Tar_alti_m = Cur_alti_m + alti_offset_m;
    }

};

//
class OurOneAction : public OurAction{
public:
    int count;
    OurOneAction(){
        count = 0;
    }

    bool stepOver(Flight * flight){
        count ++;
        if(count <= 1){
            uint8_t flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
            flight->control(flag, 10, 10, 0, 0);
        }
        if(count <= 1000) return false;
        return true;
    }
};

// 绘制圆的Action
class OurDrawCircleAction : public OurAction{
#define PI 3.1415926
    float V;
    float R;
    int count;
    int allCount;
public:
    OurDrawCircleAction(){
        V = 2;
        R = 2;
        count = 0;
        int interval = 25;
        allCount = 2 * PI * R / V * 1000 / interval;
        isFirst = true;
    }

    bool stepOver(Flight* flight){
        if(isFirst){
            lastTimeStap = getTimeStap(flight);
            isFirst = false;
        }

        float vx = V * sin((V/R)*count/50.0f);
        float vy = V * cos((V/R)*count/50.0f);
        uint8_t flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
        flight->control(flag, vx, vy, 0, 0);
        count++;
        if(count >= 400) return true;
        return false;
    }

    DJI::time_ms lastTimeStap;
    bool isFirst;
    DJI::time_ms getTimeStap(Flight* flight){
        return flight->getApi()->getDriver()->getTimeStamp();
    }

#undef PI
};

#define OurVerticalAction OurMoveByUseVerticalAction

class OurStarPipeline: public OurDJIFSMActionPipeline{
public:
    OurStarPipeline(){
        //addAction(new OurOneAction());
        addCircleTrace();
    }

    void addSquareTrace(){
        addAction(new OurVerticalAction(0, 0, 10));
        addAction(new OurStableAction());\
        addAction(new OurVerticalAction(10, 0, 0));
        addAction(new OurStableAction());
        addAction(new OurVerticalAction(0, 10, 0));
        addAction(new OurStableAction());
        addAction(new OurVerticalAction(-10, 0, 0));
        addAction(new OurStableAction());
        addAction(new OurVerticalAction(0, -10, 0));
        addAction(new OurStableAction());
    }

    void addStarTrace(){
        addAction(new OurStarAction());
    }

    void addCircleTrace(){
        addAction(new OurDrawCircleAction());
    }
};


#endif // OURSTARPIPELINE_H
