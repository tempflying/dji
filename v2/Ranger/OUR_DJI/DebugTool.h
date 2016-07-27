#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H
/*
    该DEBUGTOOL 如果用于对官方源代码中数据的输出，建议直接拷贝函数过去并重命名，
    不建议#include，因为该文件#include "DJI_LIB/DJI_Pro_App.h"，可能形成include环，过不了编译
*/

#include "DJI_LIB/DJI_Pro_App.h"

// 该函数用于输出官方的USB-SERIAL的通信数据
// 沿着函数 DJI_Pro_Activate_API 的调用链，最后一层就是port->write(..)调用的system call
// 该函数的上一层，也就是应用程序的最后一层时函数 int DJI_Pro_Hw::Pro_Hw_Send(unsigned char *buf, int len)
// 他传入的参数 buf 和 len 就是该函数可以处理的东西
// 该函数的实现完全参照官方文档中的开放协议篇幅
void OurPrintFrame(unsigned char* buf, int len);

// 该系列函数用于打印一切调试数据
void OurPrint(activate_data_t user_act_data);

#endif // DEBUGTOOL_H
