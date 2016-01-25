#ifndef OURDJI_FUNC_H
#define OURDJI_FUNC_H

#include "DJI_LIB/DJI_Pro_App.h"    // DJI的总库

#include <QSerialPortInfo>          // listSerialPort()中使用，须在 .pro的配置文件中加上 QT+= serialport目录才能被引用到,列举可用串口时使用

#include "DJI_LIB/DJI_Pro_Hw.h"     // 用于openSerialport(..)函数中使用，用于打开或关闭串口

int DJI_Sample_Setup(void);         // DJI连接机载设备的准备函数
void listSerialPort();              // 列举可用串口
void openSerialport(const char* serialPortName);    // 如果有串口打开，则关闭；如果没有串口打开，则打开该串口（传入的参数）

void active();                              // 用于激活无人机
void Activate_Callback(unsigned short res); // 激活函数的回调用函数，用会激活的转态
void obtainControl();                       // 如果获得控制权则释放控制权 ，如果没有获得控股权则获得控制权，

void OurSetUp();
void OurDoSerialPort();
void OurActivate();
void OurActivateCallback(unsigned short res); // 激活函数的回调用函数，用会激活的转态
void OurDoObtain();

#endif // OURDJI_FUNC_H
