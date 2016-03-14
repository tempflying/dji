#include "mainwindow.h"
#include <QApplication>
#include <math.h>
#include "OUR_DJI/OurDJI_FUNC.h"
#include "OUR_DJI/DebugTool.h"
#include "DJI_DEMO/DJI_Pro_Sample.h"

//CalPos函数
void DJI_CalPos1(const fp64 Cur_lati_r, const fp64 Cur_longti_r, const fp32 Cur_alti_m, \
            const fp64 lati_offset_m, const fp64 longti_offset_m, const fp32 alti_offset_m, \
            fp64* const Tar_lati_r, fp64* const Tar_longti_r, fp32* const Tar_alti_m)
{
    *Tar_lati_r = Cur_lati_r + (lati_offset_m/(double)6378137.0);
    *Tar_longti_r = Cur_longti_r + (longti_offset_m/((double)6378137.0*cos(Cur_lati_r)));
    *Tar_alti_m = Cur_alti_m + alti_offset_m;
}

//CalOffSet函数开始

void DJI_CalOffset1(const fp64 Cur_lati_r, const fp64 Cur_longti_r, const fp32 Cur_alti_m, \
            fp64* const lati_offset_m, fp64* const longti_offset_m, fp32* const alti_offset_m,\
            const fp64 Tar_lati_r, const fp64 Tar_longti_r, const fp32 Tar_alti_m)
{
    *lati_offset_m = (Tar_lati_r - Cur_lati_r)*(double)6378137.0;
    *longti_offset_m = (Tar_longti_r - Cur_longti_r)*((double)6378137.0*cos(Cur_lati_r));
    *alti_offset_m = Tar_alti_m - Cur_alti_m;
}

//CalOffSet函数结束

//GotoPos函数开始

void DJI_GotoPos(fp64 lati, fp64 longti, fp32 alti,fp32 err)
{
    sdk_std_msg_t UavInfo;
    attitude_data_t user_ctrl_data;
    fp64 lati_offset_m = 0;
    fp64 longti_offset_m = 0;
    fp32 alti_offset_m = 0;
    while(1)
    {
        DJI_Pro_Get_Broadcast_Data(&UavInfo);

        DJI_CalOffset1(UavInfo.pos.lati,UavInfo.pos.longti,UavInfo.pos.alti,\
               &lati_offset_m,&longti_offset_m,&alti_offset_m,\
               lati,longti,alti);


        printf("lati = %f longti = %f alti = %f sqrt = %f\r",lati_offset_m,longti_offset_m,alti,\
               sqrt(lati_offset_m*lati_offset_m + longti_offset_m*longti_offset_m + alti_offset_m*alti_offset_m));

        if(sqrt(lati_offset_m*lati_offset_m + longti_offset_m*longti_offset_m + alti_offset_m*alti_offset_m) <= err)
        {
            user_ctrl_data.ctrl_flag = HORIZ_POS|VERT_VEL|YAW_RATE|HORIZ_BODY|YAW_BODY;
            user_ctrl_data.roll_or_x = 0;
            user_ctrl_data.pitch_or_y = 0;
            user_ctrl_data.thr_z = 0;
            user_ctrl_data.yaw = 0;
            DJI_Pro_Attitude_Control(&user_ctrl_data);
            break;
        }

        user_ctrl_data.ctrl_flag = HORIZ_POS|VERT_POS|YAW_ANG|HORIZ_BODY|YAW_BODY;
        user_ctrl_data.roll_or_x = lati_offset_m;
        user_ctrl_data.pitch_or_y = longti_offset_m;
        user_ctrl_data.thr_z = alti;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
}

//GotoPos函数结束


//去指定位置点的函数
void DJI_Sample_Way_Point1()
{
    typedef struct way_point
    {
        fp64 lati_r;
        fp64 longti_r;
        fp32 alti_m;
    }way_point_t;

    way_point_t way_point_data[5];

    sdk_std_msg_t UavInfo;
    DJI_Pro_Get_Broadcast_Data(&UavInfo);


    DJI_CalPos1(UavInfo.pos.lati,UavInfo.pos.longti,UavInfo.pos.alti,\
               0, 1.902113*5, 0,\
               &way_point_data[0].lati_r,&way_point_data[0].longti_r,&way_point_data[0].alti_m);

    DJI_CalPos1(way_point_data[0].lati_r,way_point_data[0].longti_r,way_point_data[0].alti_m,\
               -1.1180339*5, -1.538842*5, 0,\
               &way_point_data[1].lati_r,&way_point_data[1].longti_r,&way_point_data[1].alti_m);

    DJI_CalPos1(way_point_data[1].lati_r,way_point_data[1].longti_r,way_point_data[1].alti_m,\
               1.8090169*5, 0.587785*5, 0,\
               &way_point_data[2].lati_r,&way_point_data[2].longti_r,&way_point_data[2].alti_m);

    DJI_CalPos1(way_point_data[2].lati_r,way_point_data[2].longti_r,way_point_data[2].alti_m,\
               -1.8090169*5, 0.587785*5, 0,\
               &way_point_data[3].lati_r,&way_point_data[3].longti_r,&way_point_data[3].alti_m);

    DJI_CalPos1(way_point_data[3].lati_r,way_point_data[3].longti_r,way_point_data[3].alti_m,\
               1.1180339*5, -1.538842*5, 0,\
               &way_point_data[4].lati_r,&way_point_data[4].longti_r,&way_point_data[4].alti_m);

    for(int i = 0; i < 5; i++)
    {
        DJI_GotoPos(way_point_data[i].lati_r, way_point_data[i].longti_r,\
                    way_point_data[i].alti_m,0.3);
    }
}

//去指定位置点的函数结束



int main(int argc, char *argv[])
{
    listSerialPort();                   // 列举串口
    OurSetUp();                         // init
    OurDoSerialPort(OUR_OPEN_PORT);     // 打开串口端口
    OurActivate();                      // 激活
    OurDoControl(OUR_OBTAIN_CONTROL);   // 获取控制权

/******************************************************************************************************/
    // 以下是飞控程序

    DJI_Pro_Status_Ctrl(4,0);          //起飞

    sleep(8);

    attitude_data_t user_ctrl_data;
    user_ctrl_data.ctrl_flag = 0x90;   //按位置进行控制的飞行方式

    DJI_Sample_Way_Point1();            //调用原来的函数，按制定位置飞行，未设置参数

    sleep(8);

    DJI_Pro_Status_Ctrl(1,0);          //返航


    //结束

    DJI_Sample_Funny_Ctrl(DRAW_SQUARE_SAMPLE);
    sleep(6);

    DJI_Pro_Status_Ctrl(4,0);

/******************************************************************************************************/
    OurDoControl(OUR_RELEASE_CONTROL);  // 释放控制权
    OurDoSerialPort(OUR_CLOSE_PORT);    // 关闭串口端口
    sleep(2);                           // 等待所有操作完成
    //getchar();
    return 0;
}



