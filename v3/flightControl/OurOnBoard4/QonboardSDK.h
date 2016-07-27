#ifndef QONBOARDSDK_H
#define QONBOARDSDK_H

#include <DJI_HardDriver.h>
#include <DJI_Camera.h>
#include <DJI_Flight.h>
#include <DJI_HotPoint.h>
#include <DJI_Follow.h>
#include <DJI_WayPoint.h>
#include <DJI_VirtualRC.h>
#include <DJI_API.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QThread>

using namespace DJI::onboardSDK;

class QHardDriver : public HardDriver
{
  public:
    QHardDriver(QSerialPort *Port);

    bool initresult;
    void init();                    // 继承的虚函数不能修改声明，所以这里使用一个变量来返回结果
    DJI::time_ms getTimeStamp();
    size_t send(const uint8_t *buf, size_t len);
    size_t readall(uint8_t *buf, size_t maxlen);

    void lockMemory();
    void freeMemory();

    void lockMSG();
    void freeMSG();


    void setBaudrate(int value);

  private:
    QHardDriver();

  private:
    int baudrate;
    QSerialPort *port;
    QMutex memory;
    QMutex msg;
    QMutex sendlock;
};

class APIThread : public QThread
{
    Q_OBJECT

  public:
    APIThread();
    APIThread(CoreAPI *API, int Type, QObject *parent = 0);

    void setInterval(int interval){
        this->interval = interval;
    }

    void run();

  private:
    CoreAPI *api;
    int type;
    int interval;
};

#endif // QONBOARDSDK_H
