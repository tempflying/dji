#include "mainwindow.h"
#include <QApplication>

#include "OUR_DJI/OurDJI_FUNC.h"
#include "OUR_DJI/DebugTool.h"
#include "DJI_DEMO/DJI_Pro_Sample.h"
#include "OUR_DJI/DebugTool.h"

#include <pthread.h>

// 线程创建函数，该函数基本不变，只是有时候选择 join 还是 sleep,sleep的好处是在这个线程函数外也可以使用
int create_thread(void* (*func)(void*), void* arg){
    pthread_t A_ARR;
    int thread_id = pthread_create(&A_ARR, 0, func, arg);
    if(thread_id == 0) // 父进程
    {
        pthread_join(A_ARR, 0);     // 等待子线程, 根据需要决定是否等待,第二个参数是,子线程执行函数的return
        printf("thread finshed\n");
        return 0;
    }
    else return -1;
}

void* flyFunc(void*){
    attitude_data_t user_ctrl_data;
    int i;
    for(i = 0; i < 100; i ++)
    {
        user_ctrl_data.ctrl_flag = HORIZ_VEL|VERT_VEL|YAW_ANG|HORIZ_GND|YAW_GND;     // 0x40
        user_ctrl_data.ctrl_flag = 0x40;
        user_ctrl_data.roll_or_x = 0;
        user_ctrl_data.pitch_or_y = 0;
        if(i < 90)
            user_ctrl_data.thr_z = 2.0;
        else
            user_ctrl_data.thr_z = 0.0;
        user_ctrl_data.yaw = 0;

        DJI_Pro_Attitude_Control(&user_ctrl_data);
        usleep(20000);                              // 50HZ
    }
    sleep(1);
}

// 县城启动该的函数
int fly(){
    return create_thread(flyFunc, 0);
}

int main(int argc, char *argv[])
{
    OurAllocConsole();
    listSerialPort();                   // 列举串口
    OurSetUp();                         // init
    OurDoSerialPort(OUR_OPEN_PORT);     // 打开串口端口
    OurActivate();                      // 激活
    OurDoControl(OUR_OBTAIN_CONTROL);   // 获取控制权

/******************************************************************************************************/
    // 以下是飞控程序
    // 自动起飞,在异步线程中进行,sleep(8) 保证执行完成
    DJI_Pro_Status_Ctrl(4, 0);
    //sleep(8);

    // fly();

    sleep(2);

    // ***待修改,应当根据返回的高度参数确认是否releaseControl,不应使用sleep
    // 自动降落,也可选择自动返航
    //DJI_Pro_Status_Ctrl(6, 0);
    //sleep(8);
    //while(true);

/******************************************************************************************************/
    OurDoControl(OUR_RELEASE_CONTROL);  // 释放控制权
    OurDoSerialPort(OUR_CLOSE_PORT);    // 关闭串口端口
    sleep(2);                           // 等待所有操作完成
    //getchar();
    return 0;
}



