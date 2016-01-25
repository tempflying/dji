#include "mainwindow.h"
#include <QApplication>
#include "DJI_LIB/DJI_Pro_App.h"    //DJI的官方总库

#include <QSerialPortInfo>          //listSerialPort()中使用，需要在配置文件（QtTest.pro）里加宏 QT+=...列举所有可用的串口
#include "DJI_LIB/DJI_Pro_Hw.h"     //被openSerialport(const char* serialPortName)使用，用来打开关闭串口

#include "unistd.h"                 //线程等待，sleep函数


int DJI_Sample_Setup(void);
void listSerialPort();
void openSerialport(const char* serialPortName);
void active();
void Activate_Callback(unsigned short res);          //激活函数的回调函数，看是否激活了
void obtainControl();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    DJI_Sample_Setup();              //连接机载设备的准备函数
    listSerialPort();                //列举所有的可用串口
    openSerialport("COM5");          //如果有串口打开则关闭；如果没有串口打开则打开串口（参数）
    active();                        //激活无人机的函数（账号密码等）
    sleep(3);
    obtainControl();                 //如果有控制权则释放控制权，如果没有控制权则获得控制权


    return a.exec();
}

int DJI_Sample_Setup(void)
{
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
    return 0;
}

void listSerialPort(){
    printf("list possible serial port : \n");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        printf("    portName     : %s\n",info.portName().toStdString().data());
        printf("    description  : %s\n",info.description().toStdString().data());
        printf("    manufacturer : %s\n",info.manufacturer().toStdString().data());
        printf("\n");
    }
}

bool openSerial = false;
void openSerialport(const char* serialPortName)
{
    // argument
    const char* serial = serialPortName;
    int baud = 230400;

    DJI_Pro_Hw *hw_instance = DJI_Pro_Hw::Pro_Hw_Get_Instance();
    // open serial port
    if(!openSerial)
    {
        if(true == hw_instance->Pro_Hw_Setup(serial,baud))
         {
             hw_instance->start();
             printf("serial port %s open success\n",serialPortName);
         }
        else
         {
             printf("serial port %s open failed\n",serialPortName);
        }
        openSerial = true;
    }
    else
    {
        hw_instance->Pro_Hw_Close();
        printf("close serial success\n");
        openSerial = false;
    }

}

// 该变量一定要声明为全局变量或动态变量(new),因为激活的程序官方源码是在独立的线程中运行的,
// 同一进程的不同线程能够共享的只有全局变量的动态变量
activate_data_t user_act_data;
void active(){
    printf("active : \n");
    user_act_data.app_id = 1023480;//QString("1023480").toInt();
    user_act_data.app_api_level = 2;//QString("2").toInt();
    user_act_data.app_ver = SDK_VERSION;
    user_act_data.app_bundle_id[0] = user_act_data.app_bundle_id[1] = 0x12;
    user_act_data.app_key = "1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94";//QByteArray("1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94").data();

    DJI_Pro_Activate_API(&user_act_data,Activate_Callback);
}
void Activate_Callback(unsigned short res)
{
    char result[][50]={{"SDK_ACTIVATION_SUCCESS"},{"SDK_ACTIVE_PARAM_ERROR"},{"SDK_ACTIVE_DATA_ENC_ERROR"},\
                       {"SDK_ACTIVE_NEW_DEVICE"},{"SDK_ACTIVE_DJI_APP_NOT_CONNECT"},{" SDK_ACTIVE_DIJ_APP_NO_INTERNET"},\
                       {"SDK_ACTIVE_SERVER_REFUSED"},{"SDK_ACTIVE_LEVEL_ERROR"},{"SDK_ACTIVE_SDK_VERSION_ERROR"}};

    if(res >= 0 && res < 9)
    {
        printf("%s\n",*(result+res));
    }
    else
    {
        printf("Unkown ERROR\n");
    }
}

bool obtain = false;

void obtainControl()
{
    if(obtain){
        DJI_Pro_Control_Management(0,NULL);
        obtain = false;
        printf("release control success\n");
    }
    else{
        DJI_Pro_Control_Management(1,NULL);
        obtain = true;
        printf("obtain control success\n");
    }
}
