#ifndef OURACTION_H
#define OURACTION_H

#include "DJI_Flight.h"
#include "cmath"
using namespace DJI::onboardSDK;

#define PVABE Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE\
                | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;

class OurAction
{
public:
    // 返回是否结束
    virtual bool stepOver(Flight* flight)=0;

    // 计算一个三维向量的欧式长度,返回单位是米
    static float64_t calLengthInMeter(PositionData offset_m){
        return sqrt(offset_m.latitude * offset_m.latitude + offset_m.longitude * offset_m.longitude
                + offset_m.altitude * offset_m.altitude);
    }

    // 返回两个经纬度坐标之间的距离, 参数的单位是弧度,返回值的单位是米
    /*
     * curPos_r : 弧度
     * tagPos_r : 弧度
     * offset_m : 米
    */
    static PositionData calOffsetInMeter(PositionData curPos_r, PositionData tagPos_r)
    {
        PositionData offset_m;
        offset_m.latitude = (tagPos_r.latitude - curPos_r.latitude)*(double)6378137.0;
        offset_m.longitude = (tagPos_r.longitude - curPos_r.longitude)*((double)6378137.0*cos(curPos_r.latitude));
        offset_m.altitude = tagPos_r.altitude - curPos_r.altitude;
        return offset_m;
    }

    // 返回一个经纬度加上一个距离偏移量后的经纬度,
    /*
     * curPos_r : 弧度
     * offset_m : 米
     * tagPos_r : 弧度
    */
    PositionData calPosInRadian(PositionData curPos_r, PositionData offset_m)
    {
        PositionData tagPos_r;
        tagPos_r.latitude = curPos_r.latitude + (offset_m.latitude/(double)6378137.0);
        tagPos_r.longitude = curPos_r.longitude + (offset_m.longitude/((double)6378137.0*cos(curPos_r.latitude)));
        tagPos_r.altitude = curPos_r.altitude + offset_m.altitude;
        return tagPos_r;
    }
};

#endif // OURACTION_H
