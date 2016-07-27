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


#define OUR_DJI_PRO // 当不需要时(发布时),注释掉该define
#ifdef OUR_DJI_PRO
// 以下代码是为了给DJI官方程序写入注释而存在的,对比文档的协议对数据结构进行了定义,并通过了测试

// ** control model, DJI_Pro提供的函数,功能分为接收数据和控制信号,
// ** 控制主要有四方面：
// **   1. 姿态控制;    2. 云台控制;    3. 相机控制（DJI_Pro_Camera_Control） 4. 自动返航起飞降落 (DJI_Pro_Status_Ctrl)

// **** 1. 姿态控制     （http://developer.dji.com/cn/onboard-sdk/documentation/Appendix/）
/* ******   typedef struct
            {
                unsigned char ctrl_flag;                    // 控制模式,决定下面的三个值使用哪个情况
                float 	roll_or_x;                          // 水平反向 HORIZ_ATT(00) || HORIZ_VEL(01) || HORIZ_POS(10)
                float	pitch_or_y;                         // 同上
                float	thr_z;                              // 竖直方向 VERT_VEL(00) || VERT_POS(01) || VERT_TRU(10)
                float	yaw;                                // 偏航    YAW_ANG(0) || YAW_RATE(1)
            }attitude_data_t;

            ctrl_flag 控制模式 是一个 8 bit的数据结构,组成是
                HORIZ_*(2b) | VERT_*(2b) | YAW_*(1b) | HORI坐标系(2b) | YAW坐标系,是否为增稳模式(1b)
            HORI坐标系的可能取值为 HORIZ_GND(00) || HORIZ_BODY(01)
            YAW坐标系的可能取值为 YAW_GND(0) || YAW_BODY(1)
    usage :
            控制模式可以参考开放协议中的内容，直接指定数字，也可以使用上面出现的 宏 的 位或 组合指定(注意括号里写的是使用该项时相应为的二进制数，不是宏的取值)
            比如常用的:
                模式编号    十六进制数                       宏
                   3        0x40    HORIZ_VEL|VERT_VEL|YAW_ANG|HORIZ_GND|YAW_GND
                  11        0x90    HORIZ_POS|VERT_POS|YAW_ANG|HORIZ_GND|YAW_GND

            数据取值范围及单位 :
             roll_or_x,pitch_or_y
                HORIZ_ATT : -30 度 ~ 30 度
                HORIZ_VEL : -10 m/s ~ 10 m/s
                HORIZ_POS : 米为单位的相对位置，数值无限制
             thr_z
                VERT_VEL  : -4 m/s ~ 4 m/s
                VERT_POS  : 0m 到最大飞行高度
                VERT_TRU  : 10 ~ 100（危险，请小心使用）
              yaw
                YAW_ANG   : -180 度 ~ 180 度
                YAW_RATE  : -100 度/s ~ 100 度/s
*/
int DJI_Pro_Attitude_Control(attitude_data_t *p_user_data);

// **** 2. 云台控制
// ******     角度控制（http://developer.dji.com/cn/onboard-sdk/documentation/OPENProtocol/ 命令码 0x1B 云台角度控制）
/* ******   typedef struct
            {
                signed short yaw_angle;                     // 单位0.1º，输入范围 [-3200, 3200]
                signed short roll_angle;                    // 单位0.1º，输入范围 [-350, 350]
                signed short pitch_angle;                   // 单位0.1º，输入范围 [-900, 300]
                struct 
                {
                    unsigned char base : 1;                 // 0: 增量控制,相对当前云台所处位置; 1: 绝对控制,角度基准与手机DJI Go App设置有关
                    unsigned char yaw_cmd_ignore : 1;       // yaw控制的有效位. 0 : 云台Yaw角度运动到命令位置 1 : 云台Yaw将维持上一时刻状态
                    unsigned char roll_cmd_ignore : 1;      // 类似上面(因为很多时候,我们只想改变一个方向,如果要保持其他方向，就要获得其他方向，麻烦且做了多余的控制操作)
                    unsigned char pitch_cmd_ignore : 1;     // 类似上面
                    unsigned char reserve : 4;              // 保留位
                }ctrl_byte;
                unsigned char duration;                     // 命令完成时间,单位0.1s，例如20代表云台在2s内匀速转动至命令位置,建议开发者控制速度不超过400º/秒
            }gimbal_custom_control_angle_t;
*/
int Our_DJI_Pro_Gimbal_Angle_Control(gimbal_custom_control_angle_t *p_user_data);
/* ******   角速度控制 （http://developer.dji.com/cn/onboard-sdk/documentation/OPENProtocol/ 命令码 0x1A 云台角速度控制）
             typedef struct
            {
                signed short yaw_angle_rate;                // 轴向的角速度,单位0.1º/s，输入范围[-1800, 1800]
                signed short roll_angle_rate;               // 同上
                signed short pitch_angle_rate;              // 同上
                struct
                {
                    unsigned char reserve : 7;              // 保留位
                    unsigned char ctrl_switch : 1;          // decide increment mode or absolute mode(和文档不太一致）
                }ctrl_byte;
            }gimbal_custom_speed_t;
 */
int Our_DJI_Pro_Gimbal_Speed_Control(gimbal_custom_speed_t *p_user_data);

// ** 接收数据 :

// ****     推送数据类型 （http://developer.dji.com/cn/onboard-sdk/documentation/OPENProtocol/#命令数据说明  命令集 0x02 推送数据类
// ****                 http://developer.dji.com/cn/onboard-sdk/documentation/Appendix/#飞行数据说明）
/* ******   typedef struct
            {
                unsigned int time_stamp;                    // 时间戳  : 100hz
                api_quaternion_data_t q;                    // 状态四元数 : 100hz
                api_common_data_t a;                        // 加速度, x, y, z 三个方向 : m/s2, 100hz
                api_vel_data_t v;                           // 速度,x,y,z三个方向,数据是否有效,反馈数据的传感器ID,保留位 : m/s, 100hz
                api_common_data_t w;                        // 角速度,x, y, z 三个方向 : rad/s, 100hz
                api_pos_data_t pos;                         // GPS 位置(经纬度 rad), 海拔高度(气压高度 m), 相对地面高度(m), GPS健康度(0-5) : 100hz
                api_mag_data_t mag;                         // 磁感计数值,三个方向的磁感值 ： 0hz
                api_rc_data_t rc;                           // 遥控器通道值
                api_common_data_t gimbal;                   // 云台姿态 50hz
                unsigned char status;                       // 飞行状态 10hz
                                                            // 1	standby	飞行器待机
                                                            // 2	take_off	飞行器起飞
                                                            // 3	in_air	飞行器在空中
                                                            // 4	landing	飞行器降落
                                                            // 5	finish_landing	飞行器降落完成
                unsigned char battery_remaining_capacity;   // 剩余电池百分比 % 1hz
                api_ctrl_info_data_t ctrl_info;             // 控制设备
                uint8_t obtained_control;
                uint8_t activation;
            }sdk_std_msg_t;

      // 是否选用融合值可在调参软件中进行
            a
            typedef struct
            {
                fp32 x;                                     //融合值（大地坐标系）	融合数据	m/s2
                                                            //融合值（机体坐标系）	融合数据	m/s2
                                                            //原始值（机体坐标系）	加速度计数据	G
                fp32 y;
                fp32 z;
            }api_common_data_t;                             // 100hz

            w
            typedef struct
            {
                fp32 x;                                     // m/s
                fp32 y;
                fp32 z;
                unsigned char health_flag         :1;       // 数据是否有效
                unsigned char feedback_sensor_id  :4;       // 反馈数据的传感器ID
                unsigned char reserve             :3;       // 保留位
            }api_vel_data_t;                                // 100hz

            typedef struct
            {
                fp32 x;                                     // (融合值（机体坐标系）: 融合数据; 原始值（机体坐标系）: 陀螺仪数据) rad/s
                fp32 y;
                fp32 z;
            }api_common_data_t;                             // 100hz

            typedef struct
            {
                fp64 lati;                                  // GPS 纬度 rad
                fp64 longti;                                // GPS 经度 rad
                fp32 alti;                                  // 气压高度 (融合值: 气压计、IMU融合数据; 原始值: 气压计数据) m : 由于室内气压不准确，所以高度有IMU计算
                fp32 height;                                // 对地高度 (融合值: 气压计、IMU和超声波融合数据; 原始值: 超声波数据（3米内有效）) m
                unsigned char health_flag;                  // GPS 健康度	0-5, 5 为最好
            }api_pos_data_t;                                // 100hz

            typedef struct
            {
                signed short roll;                          // [-10000,10000], 美国手 右杆左右
                signed short pitch;                         // [-10000,10000], 美国手 右杆上下
                signed short yaw;                           // [-10000,10000], 美国手 左杆左右
                signed short throttle;                      // [-10000,10000], 美国手 左杆上下
                signed short mode;                          // P: -8000; A: 0; F: 8000
                signed short gear;                          // 放下起落架: -4545 收起起落架: -10000;(暂时没什么用,可用来切换飞控程序)
            }api_rc_data_t;                                 // 50hz

            typedef struct
            {
                unsigned char cur_ctrl_dev_in_navi_mode   :3; // 当前控制飞控的设备 0->rc(遥控)  1->app  2->serial
                unsigned char serial_req_status           :1; // 1->opensd  0->close
                unsigned char reserved                    :4;
            }api_ctrl_info_data_t;
*/
int Our_DJI_Pro_Get_Broadcast_Data(sdk_std_msg_t *p_user_buf);
// ****     也可以选择部分数据获取
// 命令集ID
unsigned char Our_DJI_Pro_Get_CmdSet_Id(ProHeader *header);
// 命令码ID
unsigned char Our_DJI_Pro_Get_CmdCode_Id(ProHeader *header);
// ****** 电池电量
int Our_DJI_Pro_Get_Bat_Capacity(unsigned char *data);
// ****** 姿态四元数
int Our_DJI_Pro_Get_Quaternion(api_quaternion_data_t *p_user_buf);
// ****** 加速度
int Our_DJI_Pro_Get_GroundAcc(api_common_data_t *p_user_buf);
// ****** 速度
int Our_DJI_Pro_Get_GroundVo(api_vel_data_t *p_user_buf);
// ****** 控制数据
int Our_DJI_Pro_Get_CtrlInfo(api_ctrl_info_data_t *p_user_buf);


#endif //OUR_DJI_PRO

#endif // OURDJI_FUNC_H
