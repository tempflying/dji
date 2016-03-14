#include "OurDJI_FUNC.h"
#include "string"

void listSerialPort(){
    printf("list possible serial port : \n");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        printf("    portName     : %s\n",info.portName().toStdString().data());
        printf("    description  : %s\n",info.description().toStdString().data());
        printf("    manufacturer : %s\n",info.manufacturer().toStdString().data());
        printf("\n");
    }
}

void OurSetUp(){
#if (defined(PLATFORM_LINUX) && defined(TINYXML_CFG))
    int ret;
    int baudrate = 115200;
    char uart_name[32] = {"/dev/ttyUSB0"};

    if(DJI_Pro_Get_Cfg(&baudrate,uart_name,NULL,NULL,NULL) == 0)
    {
        /* user setting */
        printf("\n--------------------------\n");
        printf("uart_baud=%d\n",baudrate);
        printf("uart_name=%s\n",uart_name);
        printf("--------------------------\n");
    }
    ret = Pro_Hw_Setup(uart_name,baudrate);
    if(ret < 0)
        return ret;
#endif
    DJI_Pro_Setup(NULL);
}

const char* _OurGetSerialPort(){
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        const char* des = info.description().toStdString().data();
        const char* usb2serial = "USB-SERIAL";

        int len = strlen(usb2serial);
        int desLen = strlen(des);
        if(desLen < len) continue;                // descript 不可能是 usb2serial 接口，则切换下一个

        int i;
        for(i = 0; i < len; i++){
            if(toupper(des[i]) != usb2serial[i]) break; // descript 不可能是 usb2serial 接口，则不需再比较
        }

        if(i == len) return info.portName().toStdString().data(); // 全部相等时，则返回
    }
    return NULL;
}
void OurDoSerialPort(bool operate,const char* serialPortName, int baud){
    DJI_Pro_Hw *hw_instance = DJI_Pro_Hw::Pro_Hw_Get_Instance();
    // open serial port
    if(operate == OUR_OPEN_PORT)
    {
        if(serialPortName == NULL){ // 获取第一个USB-SERIAL的端口
            serialPortName = _OurGetSerialPort();
        }
        if(serialPortName == NULL){
            printf("cannot get enable USB-SERIAL port\n");
            return;
        }

        printf("try to open serial port %s. result :\n  ",serialPortName);
        if(true == hw_instance->Pro_Hw_Setup(serialPortName, baud))
         {
             hw_instance->start();

         }
        else
         {
             printf("serial port %s open failed\n",serialPortName);
        }
    }
    else if(operate == OUR_CLOSE_PORT)
    {
        printf("try to close serial\n");
        hw_instance->Pro_Hw_Close();
    }
}

void OurActivateCallback(unsigned short res) // 激活函数的回调用函数，用会激活的转态
{
    char result[][50]={{"SDK_ACTIVATION_SUCCESS"},{"SDK_ACTIVE_PARAM_ERROR"},{"SDK_ACTIVE_DATA_ENC_ERROR"},\
                       {"SDK_ACTIVE_NEW_DEVICE"},{"SDK_ACTIVE_DJI_APP_NOT_CONNECT"},{" SDK_ACTIVE_DIJ_APP_NO_INTERNET"},\
                       {"SDK_ACTIVE_SERVER_REFUSED"},{"SDK_ACTIVE_LEVEL_ERROR"},{"SDK_ACTIVE_SDK_VERSION_ERROR"}};

    if(res >= 0 && res < 9)
    {
        printf("  %s\n",*(result+res));
    }
    else
    {
        printf("  Unkown ERROR\n");
    }
}

// 该变量一定要声明为全局变量或动态变量(new),因为激活的程序官方源码是在独立的线程中运行的,
// 同一进程的不同线程能够共享的只有全局变量的动态变量
activate_data_t _Our_user_act_data;
void OurActivate(int appId, int appLevel, char* appKey){
    printf("try to active : \n  ");
    _Our_user_act_data.app_id = appId;                   //QString("1023480").toInt();
    _Our_user_act_data.app_api_level = appLevel;         //QString("2").toInt();
    _Our_user_act_data.app_ver = SDK_VERSION;
    _Our_user_act_data.app_bundle_id[0] = _Our_user_act_data.app_bundle_id[1] = 0x12;
    _Our_user_act_data.app_key = appKey;                // 这样直接指针复制的话，在异步线程中能访问到吗? char* 很可能是动态内存分配
    //memcpy(_Our_user_act_data.app_key, appKey, strlen(appKey)); //appKey = QByteArray("1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94").data();
    DJI_Pro_Activate_API(&_Our_user_act_data,OurActivateCallback);
    sleep(2);                                           // 保证异步线程中的激活有足够的时间完成，其实还是不太靠谱
}

void OurDoControl(bool operate){
    if(operate == OUR_RELEASE_CONTROL){
        printf("try to release control\n");
        DJI_Pro_Control_Management(0,NULL);             // 内部已经有usleep(5000)了
    }
    else if(operate == OUR_OBTAIN_CONTROL){
        printf("try to obtain control success, the result is:\n  ");
        DJI_Pro_Control_Management(1,NULL);
    }
}


#ifdef OUR_DJI_PRO
// 注释DJI_Pro的程序
int OUR_DJI_Pro_Attitude_Control(attitude_data_t *p_user_data){
    DJI_Pro_Attitude_Control(p_user_data);
}

int Our_DJI_Pro_Gimbal_Angle_Control(gimbal_custom_control_angle_t *p_user_data){
    DJI_Pro_Gimbal_Angle_Control(p_user_data);
}

int Our_DJI_Pro_Gimbal_Speed_Control(gimbal_custom_speed_t *p_user_data){
    DJI_Pro_Gimbal_Speed_Control(p_user_data);
}

int Our_DJI_Pro_Get_Broadcast_Data(sdk_std_msg_t *p_user_buf){
    DJI_Pro_Get_Broadcast_Data(p_user_buf);
}

// 命令集ID
unsigned char Our_DJI_Pro_Get_CmdSet_Id(ProHeader *header){ DJI_Pro_Get_CmdSet_Id(header);}
// 命令码ID
unsigned char Our_DJI_Pro_Get_CmdCode_Id(ProHeader *header){ DJI_Pro_Get_CmdCode_Id(header);}
// ****** 电池电量
int Our_DJI_Pro_Get_Bat_Capacity(unsigned char *data){ DJI_Pro_Get_Bat_Capacity(data);}
// ****** 姿态四元数
int Our_DJI_Pro_Get_Quaternion(api_quaternion_data_t *p_user_buf){ DJI_Pro_Get_Quaternion(p_user_buf);}
// ****** 加速度
int Our_DJI_Pro_Get_GroundAcc(api_common_data_t *p_user_buf){ DJI_Pro_Get_GroundAcc(p_user_buf);}
// ****** 速度
int Our_DJI_Pro_Get_GroundVo(api_vel_data_t *p_user_buf){ DJI_Pro_Get_GroundVo(p_user_buf);}
// ****** 控制数据
int Our_DJI_Pro_Get_CtrlInfo(api_ctrl_info_data_t *p_user_buf){ DJI_Pro_Get_CtrlInfo(p_user_buf);}

#endif //OUR_DJI_PRO
