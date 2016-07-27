#include <QCoreApplication>
#include <QonboardSDK.h>
#include "unistd.h"

typedef void* UserData;
class MyObject : public QObject{
public :
    MyObject(){
        // 创建串口和驱动对象
        port = new QSerialPort();
        port->setPortName("com4");
        driver = new QHardDriver(port);
        driver->setBaudrate(230400);

        // 打开串口
        driver->init();

        // 创建onboard SDK 的API 接口对象
        api = new CoreAPI(driver);
        api->setVersion(versionM100_31);

        // 创建控制飞行的对象
        flight = new Flight(api);

        startTimer(2);
    }

    void active(){
        data.ID = 1023480;
        QByteArray* key = new QByteArray;
        *key = QByteArray("1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94");
        data.encKey = key->data();
        api->activate(&data, MyObject::activateCallback, this);
    }

    void setControl(bool obtain){
        api->setControl(obtain, MyObject::setControlCallBack, this);
    }

    void takeoff(){
        flight->task(Flight::TASK_TAKEOFF, MyObject::takeoffCallBack, this);
        sleep(1);
    }

    void land(){
        flight->task(Flight::TASK_LANDING, MyObject::landCallBack, this);
    }

protected:
    CoreAPI* api;
    QSerialPort* port;
    QHardDriver* driver;
    ActivateData data;
    Flight* flight;

    void timerEvent(QTimerEvent *){
        api->sendPoll();
        api->readPoll();
    }

    static void activateCallback(CoreAPI *This, Header *header, UserData userData)
    {
        This->activateCallback(This, header);

        MyObject* m = (MyObject*)userData;
        volatile unsigned short ack_data;
        if (header->length - EXC_DATA_SIZE <= 2)
        {
            memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
                   (header->length - EXC_DATA_SIZE));
            if (ack_data == ACK_ACTIVE_NEW_DEVICE)
            {
                printf("New Device");
            }
            else
            {
                if (ack_data == ACK_ACTIVE_SUCCESS)
                {
                    m->setControl(true);
                    printf("active Success\n");
                }
                else
                {
                    printf("Error");
                }
            }
        }
        else
        {
            printf("Decode Error");
        }
    }

    static void setControlCallBack(CoreAPI *This, Header *header, UserData userData){
        This->setControlCallback(This, header);


        MyObject *m = (MyObject *)userData;
        unsigned short ack_data = ACK_COMMON_NO_RESPONSE;

        if (header->length - EXC_DATA_SIZE <= 2)
        {
            memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
                   (header->length - EXC_DATA_SIZE));
        }
        else
        {
            printf("ACK is exception,seesion id %d,sequence %d\n",
                    header->sessionID, header->sequenceNumber);
        }

        switch (ack_data)
        {
            case ACK_SETCONTROL_RELEASE_SUCCESS:
                printf("relese control success\n");
                break;
            case ACK_SETCONTROL_OBTAIN_SUCCESS:
                printf("obtain control sucess\n");
                m->takeoff();
                break;
            default:
                printf("fail to setControl\n");
        }
    }

    static void takeoffCallBack(CoreAPI* This, Header* header, UserData userData){
        Flight::taskCallback(This, header, userData);

        MyObject* m = (MyObject*)userData;
        unsigned short ack_data;
        if (header->length - EXC_DATA_SIZE <= 2)
        {
            memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
                   (header->length - EXC_DATA_SIZE));

            m->land();
        }
    }

    static void landCallBack(CoreAPI* This, Header* header, UserData userData){
        Flight::taskCallback(This, header, userData);

        MyObject* m = (MyObject*)userData;
        unsigned short ack_data;
        if (header->length - EXC_DATA_SIZE <= 2)
        {
            memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
                   (header->length - EXC_DATA_SIZE));

            m->setControl(false);
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyObject* m = new MyObject();
    m->active();
    return a.exec();                // Start Event Loop(这是一个死循环)
}
