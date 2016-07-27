#ifndef OURDEFAULTDJIPIPELINE_H
#define OURDEFAULTDJIPIPELINE_H

#include "../pipeline/OurDJIFSMPipeline.h"
#include "../pipeline/OurDJIPipelineInterface.h"
#include "cmath"

// 该例子不可行,使用了sleep阻塞了帧

class OurDefaultDJIPipeline : public OurDJIPipelineInterface
{
    virtual void mainFlight(){
        for(int i = 0; i < 3; i++){
            sleep(1);
            printf("mainFlight\n");
        }

        //drawSquar();
    }

    void drawSquar(){
        move(0, 0, 10); usleep(2000);
//        move(5, 0, 0);  usleep(2000);
//        move(0, 5, 0);  usleep(2000);
//        move(-5, 0, 0); usleep(2000);
//        move(0, -5, 0); usleep(2000);
    }

    void move(float64_t x, float64_t y, float32_t z, float64_t err = 0.3){
        PositionData offset_m;
        offset_m.latitude = x;
        offset_m.longitude = y;
        offset_m.altitude = z;
        move(offset_m, err);
    }

    void move(PositionData offset_m, float64_t err=0.3){
        FlightData flightData;
        PositionData beginPos_r = flight->getPosition();
        PositionData tag_offset_m  = offset_m;
        //while(1)
        {
            if(calLengthInMeter(offset_m) < err)
            {
                flightData.flag = 0x53;
                flightData.x = 0;
                flightData.y = 0;
                flightData.z = 0;
                flightData.yaw = 0;
                flight->setFlight(&flightData);
                //break;
            }

            flightData.flag = 0x53;
            flightData.x = offset_m.latitude;
            flightData.y = offset_m.longitude;
            flightData.z = offset_m.altitude;
            flightData.yaw = 0;
            flight->setFlight(&flightData);
            usleep(20000);

            offset_m = calOffsetInMeter(beginPos_r, flight->getPosition()); // 已经移动了的位移
            offset_m.altitude = tag_offset_m.altitude - offset_m.altitude;
            offset_m.latitude = tag_offset_m.latitude - offset_m.latitude;
            offset_m.longitude = tag_offset_m.longitude - offset_m.longitude;
        }
    }

    void gotoPos(PositionData tagPos,float64_t err)
    {
        PositionData currentPos;
        PositionData offset_m;
        FlightData flightData;
        while(1)
        {
            currentPos = flight->getPosition();
            offset_m = calOffsetInMeter(currentPos, tagPos);

            if(calLengthInMeter(offset_m) < err)
            {
                flightData.flag = 0x53;
                flightData.x = 0;
                flightData.y = 0;
                flightData.z = 0;
                flightData.yaw = 0;
                flight->setFlight(&flightData);
                break;
            }

            flightData.flag = 0x53;
            flightData.x = offset_m.latitude;
            flightData.y = offset_m.longitude;
            flightData.z = offset_m.altitude;
            flightData.yaw = 0;
            flight->setFlight(&flightData);
            usleep(20000);
        }
    }

    // xxx_r 表示该变量的经纬度的距离单位是弧度制, 就是常说的经纬度
    // xxx_m 表示改变量的经纬表示在经纬度上的距离, 单位是米

    // 返回两个经纬度坐标之间的距离, 参数的单位是弧度,返回值的单位是米
    /*
     * curPos_r : 弧度
     * tagPos_r : 弧度
     * offset_m : 米
    */
    PositionData calOffsetInMeter(PositionData curPos_r, PositionData tagPos_r)
    {
        PositionData offset_m;
        offset_m.latitude = (tagPos_r.latitude - curPos_r.latitude)*(double)6378137.0;
        offset_m.longitude = (tagPos_r.longitude - curPos_r.longitude)*((double)6378137.0*cos(curPos_r.latitude));
        offset_m.altitude = tagPos_r.altitude - curPos_r.altitude;
        return offset_m;
    }

    // 返回一个经纬度加上一个距离的偏移量,
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

    //
    float64_t calLengthInMeter(PositionData offset_m){
        return sqrt(offset_m.latitude * offset_m.latitude + offset_m.longitude * offset_m.longitude
                + offset_m.altitude * offset_m.altitude);
    }
};

#endif // OURDEFAULTDJIPIPELINE_H
