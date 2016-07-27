#ifndef OURMOVEBYINVERTICALACTION_H
#define OURMOVEBYINVERTICALACTION_H

#include "OurAction.h"

/*
    移动相应距离的action, 没有使用GPS数据
*/

// 纯粹使用速度累和

class OurMoveByUseVerticalAction : public OurAction{
protected:
#define MAX_HORI_VEL 10
#define MAX_VERT_VEL 4
#define ERROR 0.01

    bool isFirst;
    void init(){
        isFirst = true;
    }
public:
    uint8_t flag;
    DJI::time_ms lastTime;
    CommonData offset_m;                    // 这个位移是标量,表示偏移量
    CommonData velocityValue;               // 这个速度是矢量

public:

    // 偏移和速度的方向一定是相同的,所以只能有一个变量有方向
    /* 与成员变量相反, 这个位移是矢量 这个速度是标量,表示速度的大小 */
    OurMoveByUseVerticalAction(float x, float y, float z){
        // 设置默认速度, 使得航线是一条直线
        float vx, vy, vz;
        if(abs(x) < ERROR && abs(y) < ERROR){ vx = vy = 0;}
        else {
            if(abs(x) > abs(y)){
                vx = MAX_HORI_VEL / 2;
                vy = y / x * vx;
            }
            else{
                vy = MAX_HORI_VEL / 2;
                vx = x / y * vy;
            }
        }
        if(abs(z) < ERROR) vz = 0;
        else vz = MAX_VERT_VEL / 2;
        initConstrut(x, y, z, vx, vy, vz);
    }

    /* 与成员变量相反, 这个位移是矢量 这个速度是标量,表示速度的大小 */
    OurMoveByUseVerticalAction(float x, float y, float z, float vx, float vy, float vz){
        initConstrut(x, y, z, vx, vy, vz);
    }

    void initConstrut(float x, float y, float z, float vx, float vy, float vz){
        // 获取偏移量，不记录方向，方向会转嫁给速度
        offset_m.x = abs(x);
        offset_m.y = abs(y);
        offset_m.z = abs(z);

         // 获取速度矢量, 将输入的唯一的方向转嫁到速度上
        velocityValue.x = (abs(vx) > MAX_HORI_VEL ? MAX_HORI_VEL : abs(vx)) * sign(x);
        velocityValue.y = (abs(vy) > MAX_HORI_VEL ? MAX_HORI_VEL : abs(vy)) * sign(y);
        velocityValue.z = (abs(vz) > MAX_VERT_VEL ? MAX_VERT_VEL : abs(vz)) * sign(z);

        flag = Flight::HORIZONTAL_VELOCITY | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE |
                Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;

        init();
    }

     float abs(float xx){
         return xx > 0 ? xx : -xx;
     }

     float sign(float num){
         if(num > 0) return 1;
         else if(num < 0) return -1;
         else return 0;
     }


    bool stepOver(Flight* flight){
        if(isFirst) {
            isFirst = false;
            lastTime = getCurrentTime(flight);
            flight->control(flag, velocityValue.x, velocityValue.y, velocityValue.z, 0);
        }else{
            DJI::time_ms curTime = getCurrentTime(flight);
            float vx,vy,vz;

            offset_m.x = offset_m.x - (curTime - lastTime) * abs(velocityValue.x) / 1000;
            if(offset_m.x <= ERROR) vx = 0;
            else vx = velocityValue.x;

            offset_m.y = offset_m.y - (curTime - lastTime) * abs(velocityValue.y) / 1000;
            if(offset_m.y <= ERROR) vy = 0;
            else vy = velocityValue.y;

            offset_m.z = offset_m.z - (curTime - lastTime) * abs(velocityValue.z) / 1000;
            if(offset_m.z <= ERROR) vz = 0;
            else vz = velocityValue.z;

            flight->control(flag, vx, vy, vz, 0);

            lastTime = curTime;

            if(abs(vx) <= 0.000001 && abs(vy) <= 0.000001 && abs(vz) <= 0.000001) {
                init();
                return true;
            }
        }
        return false;
    }

    DJI::time_ms getCurrentTime(Flight* flight){
        return flight->getApi()->getDriver()->getTimeStamp();
    }
#undef MAX_HORI_VEL
#undef MAX_VERT_VEL
};


#endif // OURMOVEBYINVERTICALACTION_H
