#ifndef OURDJI_FUNC_H
#define OURDJI_FUNC_H

#include "DJI_LIB/DJI_Pro_App.h"    // DJI的总库
#include <QSerialPortInfo>          // listSerialPort()中使用，须在 .pro的配置文件中加上 QT+= serialport目录才能被引用到,列举可用串口时使用
#include "DJI_LIB/DJI_Pro_Hw.h"     // 用于openSerialport(..)函数中使用，用于打开或关闭串口
#include "unistd.h"                 // 用于调用 sleep 和 usleep ,在激活函数 OurActivate 中

// DJI连接机载设备的准备函数，相当于init();
void OurSetUp();

// 用于打开会关闭串口,这个函数之所以行数略多，主要是在打印调试信息和获取端口，只是便于调试，所以是否使用自己决定即可。
// operate : OUR_OPEN_PORT 打开操作； OUR_CLOSE_PORT 关闭操作
// serialPortName : 串口名，如"COM5";如果不设置，则默认自动选取第一个USB-SERIAL端口；如果手动设置，可以使用listSerialPort 辅助查看所有串口。
// baud : 串口率， 需要和N1 Assit调参软件设置的保持一致
#define OUR_OPEN_PORT true
#define OUR_CLOSE_PORT false
void OurDoSerialPort(bool operate,const char* serialPortName=NULL, int baud = 230400);

// 用于激活无人机，由于考虑到实在异步线程中激活的，而且设置较多且无脑，不建议重写，建议直接调用.但要根据实际情况填写参数，默认的不一定符合你的现状
void OurActivate(int appId=1023480, int appLevel=2,
                 char* appKey="1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94");

// 获得或释放控制权，如果不是调试，建议直接使用DJI_Pro_Control_Management （该函数直接调用的DJI官方函数）
// operate : OUR_OBTAIN_CONTROL 表示获取控制权； OUR_RELEASE_CONTROL 表示释放控制权
#define OUR_OBTAIN_CONTROL true
#define OUR_RELEASE_CONTROL false
void OurDoControl(bool operate);

// 列举可用串口, 用于辅助选择USB-Serial端口名
void listSerialPort();

#endif // OURDJI_FUNC_H
