#include "dji_sample.h"
#include "stdio.h"

static int DJI_Sample_Create_Thread(void *(* func)(void *), void *arg)
{
    pthread_t A_ARR;
    //创建新线程去进行飞控执行，如果开辟失败，则返回-1，否则0；
    if(pthread_create(&A_ARR,0,func,arg) != 0)
    {
        return -1;
    }
    return 0;
}

/*
 * This method is responsible for testing atttitude
 */
static void * DJI_Sample_Atti_Ctrl_Thread_Func(void *arg)
{
    int i;
    attitude_data_t user_ctrl_data;

    /* takeoff */
    DJI_Pro_Status_Ctrl(4,0);
    sleep(8);
    /* attitude control, go up */
    for(i = 0; i < 100; i ++)
    {
        /*控制模式标志字节*/
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        if(i < 90)
            user_ctrl_data.thr_z = 2.0;
        else
            user_ctrl_data.thr_z = 0.0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        if(i < 180)
            user_ctrl_data.roll_or_x = 2;
        else
            user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        if(i < 180)
            user_ctrl_data.roll_or_x = -2;
        else
            user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        if(i < 180)
            user_ctrl_data.pitch_or_y = 2;
        else
            user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        if(i < 180)
            user_ctrl_data.pitch_or_y = -2;
        else
            user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        if(i < 180)
            user_ctrl_data.thr_z = 0.5;
        else
            user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        if(i < 180)
            user_ctrl_data.thr_z = -0.5;
        else
            user_ctrl_data.thr_z = 0;
        user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0xA;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        if(i < 180)
            user_ctrl_data.yaw = 90;
        else
            user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    for(i = 0; i < 200; i ++)
    {
        user_ctrl_data.ctrl_flag = 0xA;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        user_ctrl_data.thr_z = 0;
        if(i < 180)
            user_ctrl_data.yaw = -90;
        else
            user_ctrl_data.yaw = 0;
        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);
    }
    sleep(1);
    /* gohome */
    DJI_Pro_Status_Ctrl(1,0);

    atti_ctrl_sample_flag = -1;
    return (void*)NULL;
}

//此函数用来做一个demo形式的飞行控制，如果atti_ctrl_sample_flag == 0，归为-1
//如果不等于0话，则归为0，然后去执行飞行控制demo的函数，如果执行失败，返回-1，成功返回0；
int DJI_Sample_Atti_Ctrl()
{
    if(atti_ctrl_sample_flag == 0)
    {
        return -1;
    }
    atti_ctrl_sample_flag = 0;

    if(DJI_Sample_Create_Thread(DJI_Sample_Atti_Ctrl_Thread_Func,NULL) != 0)
    {
        return -1;
    }
    return 0;
}
