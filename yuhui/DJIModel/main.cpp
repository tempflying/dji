#include "mainwindow.h"
#include <QApplication>

#include "OUR_DJI/OurDJI_FUNC.h"
#include "OUR_DJI/DebugTool.h"
#include "DJI_DEMO/DJI_Pro_Sample.h"

int main(int argc, char *argv[])
{
    listSerialPort();                   // 列举串口
    OurSetUp();                         // init
    OurDoSerialPort(OUR_OPEN_PORT);     // 打开串口端口
    OurActivate();                      // 激活
    OurDoControl(OUR_OBTAIN_CONTROL);   // 获取控制权

/******************************************************************************************************/
    // 一下是飞控程序

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



