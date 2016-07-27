#ifndef OURDJIPIPELINE_H
#define OURDJIPIPELINE_H

#include <QSerialPort>
#include <DJI_API.h>
#include <QtCore/QStringList>
#include "QonboardSDK.h"
#include "stdlib.h"
#include "unistd.h"
#include "OurByteConvertUtil.h"

typedef void* UserData;

struct ReceivedData{
    char* data;
    size_t length;

    ReceivedData(){
        data = 0;
        length =0;
    }

    void destroy(){
        if(data){
            delete[] data;
            data = 0;
        }
    }
};



class OurDJIPipelineInterface : public QObject
{
    Q_OBJECT
public:
    explicit OurDJIPipelineInterface(QObject *parent = 0);

    ~OurDJIPipelineInterface(){
        receivedDataBuffer.destroy();
        if(flight) {
            delete flight;
            flight = 0;
        }
        if(api) {
            delete api;
            api = 0;
        }
        if(port) {
            delete port;
            port = 0;
        }
    }

    void start(int interval=2, const char* portName=NULL, int baudrate=230400,int id = 1023480);
/*
 一下函数5个已经放弃使用，使用update中的有限状态机代替了
 用户继承该类可能需要的功能,用户可以重写这些函数,使用多态调用
private:
    // 激活成功会调用的回调函数
    virtual void activeSuccess(unsigned short ack_data){
        API_LOG(driver, STATUS_LOG, "ack_data=%x\n", ack_data);
        printf("***************************************************\n");
    }

    // 获得控制权第一步成功时调用
    virtual void obtainControlRunning(unsigned short ack_data){
        API_LOG(driver, STATUS_LOG, "ack_data=%x\n", ack_data);
        printf("***************************************************\n");
    }
    // 获得控制权时调用
    virtual void obtainControlSuccess(unsigned short ack_data){
        API_LOG(driver, STATUS_LOG, "ack_data=%x\n", ack_data);
        printf("***************************************************\n");
    }

    // 释放控制权第一步成功时调用
    virtual void releaseControlRunning(unsigned short ack_data){
        API_LOG(driver, STATUS_LOG, "ack_data=%x\n", ack_data);
        printf("***************************************************\n");
    }
    // 释放控制权成功时调用
    virtual void releaseControlSuccess(unsigned short ack_data){
        API_LOG(driver, STATUS_LOG, "ack_data=%x\n", ack_data);
        printf("***************************************************\n");
    }
*/

protected:
    // 飞行主程序
    virtual void mainFlight(){}
    virtual void whenInMainFlight();
    virtual void sendDataToMobileCallback(DJI::onboardSDK::CoreAPI * This, Header * header, UserData user){
        API_LOG(This->getDriver(), STATUS_LOG, "------------send data from mobile. header=%p, user=%p\n", header, user);
    }

// 一些工具
protected:
    CoreAPI* api;
    QSerialPort* port;
    QHardDriver* driver;
    Flight* flight;

    enum OurState{
        OUR_WAITTING, OUR_BEGIN, OUR_ACTIVATED, OUR_OBTAINCONTROL_RUNNING,
        OUR_OBTAINCONTROL_SUCCESS, OUR_TAKEOFF, OUR_MAINFLIGHT, OUR_LANDDING,
        OUR_RELEASECONTROL_RUNNING, OUR_RELEASECONTROL_SUCCESS
    };
    OurState ourCurrentState;

    // 从报文中获得应答值工具函数
    static unsigned short getAckFromHeader(Header *header);
    static unsigned short getAckFromFrame(Header *header);
    // 从报文中获得Data区域的函数
    static size_t getDataFromFrame(Header* header, char* data);
    // 创建新线程执行某个函数的工具函数,其实是一个备忘的辅助函数,传入的函数必须是静态
    bool createThread(void *(* func)(void *), void *arg);               // 暂时无用
    // 列举所有可用的串口的信息
    void listSerialPort();

protected:
    // 获得第一个USB-Serial的串口名,用于自动识别串口
    const char* getSerialPort();
    // 打开串口
    bool reOpenPort(const char* portName, int baudrate);
    // 关闭串口
    void closePort();

    // 激活
    ActivateData acdata;
    void active(int id);

    // 控制权
#define OBTAIN_CONTROL true
#define RELEASE_CONTROL false
    void setControl(bool operate);

    // 自动起飞和降落
    OurState ourFlightState;
    void takeoff();
    void landing();
    virtual void initAutoFlightSafeCondition(){times = 0;}

    // 因为pthread的执行函数必须是静态的,且必须是void* f(void*)，所以使用他作为一个
    // mainFlight的中间处理函数
    static void* mainFlightThread(void*);

    // 向手机发数据和收数据
    private:
    OurState ourBackState; // 处理完后应返回的上一个状态
    ReceivedData receivedDataBuffer;
    protected:
#define MAXFRAMESIZE 100
    // 以下两个函数只能发送和接受 < 100B的数据
    void sendDataTOMobile(char* data, uint8_t size);
    ReceivedData getReceiveDataFromMobile();               // 尝试去获得数据，如果数据接收完毕，会返回接收到的数据，

    void printReceiveDataInInt(ReceivedData rData){
        printf("data point : %p\n", rData.data);
        for(size_t i = 0; i < rData.length; i++){
            printf("%d ", int(rData.data[i]));
        }
        printf("\n");
    }

// 中间处理过程
protected:
    // overwrite of QObject,和 QTimer 有相同的效果
    // 从串口中轮询的读写数据, startTimer 设置该函数的调用间隔,单位是毫秒
    // 帧刷新函数
    int times;
    int interval;
    virtual void update();
private:
    void timerEvent(QTimerEvent *);

// DJI API的直接回调函数，在这里完整各种预处理过程的逻辑判断，和ourCurrentState的更新
private:

    // 激活回调函数
    static void pActivationCallback(CoreAPI *This, Header *header, UserData userData);
    // 控制权回调函数
    static void CoreSetControlCoreback(CoreAPI *This, OurDJIPipelineInterface* pipeline, unsigned short ack_data);
    static void pSetControlCallBack(CoreAPI *This, Header *header, UserData userData);
    // 自动飞行 takeoff gohome landding的回调函数,注意回调并不是在执行完成的时候调用,而是在开始执行的时候调用
    static void pAutoFlightCallBack(CoreAPI *This, Header *header, UserData userData);
    // 从mobile收到数据时的回调函数
    static void pFromMobileCallBack(CoreAPI * This, Header * header, UserData user);
    // 发送数据到手机时的回调函数
    static void pSendMobileCallBack(CoreAPI * This, Header * header, UserData user);
//signals:
//public slots:
};

#endif // OURDJIPIPELINE_H
