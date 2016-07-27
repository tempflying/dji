#include "QonboardSDK.h"
#include <QDateTime>

QHardDriver::QHardDriver()
{
    port = 0;
    baudrate = 9600;
}

QHardDriver::QHardDriver(QSerialPort *Port)
{
    port = Port;
    baudrate = 9600;
}

void QHardDriver::init()
{
    if (port != 0)
    {
        if (port->isOpen())
            port->close();
        port->setBaudRate(baudrate);
        port->setParity(QSerialPort::NoParity);
        port->setDataBits(QSerialPort::Data8);
        port->setStopBits(QSerialPort::OneStop);

        if (port->open(QIODevice::ReadWrite))
        {
            API_LOG(this, STATUS_LOG, "port %s open success",
                    port->portName().toLocal8Bit().data());
            initresult = true;
        }
        else
        {
            API_LOG(this, ERROR_LOG, "fail to open port %s",
                    port->portName().toLocal8Bit().data());
            initresult = false;
        }
        API_LOG(this, STATUS_LOG, "BaudRate: %d", port->baudRate());
    }
}

DJI::time_ms QHardDriver::getTimeStamp() { return QDateTime::currentMSecsSinceEpoch(); }

void OurPrintFrame(const uint8_t* buf, size_t len){
    if(len < 16){
        printf("error frame\n");
        return;
    }
    printf("Name/Size(bit)          : Data(HEX) Byte seperate by space\n");
    printf("SOF/8                   : %02x\n", buf[0]);                     // %02x 按十六进制打印,保持两位数字格式，不够前面补零
    printf("LEN/10 VER/6            : %02x %02x\n", buf[1], buf[2]);
    printf("SESSION/5 ACK/1 RESO/2  : %02x\n", buf[3]);
    printf("PADDING/5 ENC/3         : %02x\n", buf[4]);
    printf("RES1/24                 : %02x %02x %02x\n", buf[5],buf[6],buf[7]);
    printf("SEQ/16                  : %02x %02x\n",buf[8],buf[9]);
    printf("CRC16/16                : %02x %02x\n", buf[10], buf[11]);
    unsigned int i = 12;
    printf("Data/%d * 8:            : ", (len-16));
    for(;i < len - 4; i++){
        if((i - 12) % 8 == 0)
            printf("\n                          ");     // 数据是小端派编址，而打印是从地地址开始的，所以对于一个字段如果包含多个字节，他的顺序是反的
        printf("%02x ",buf[i]);
    }
    printf("\n");
    printf("CRC32               : %02x %02x %02x %02x\n", buf[len-4], buf[len-3], buf[len-2],buf[len-1]);
}

size_t QHardDriver::send(const uint8_t *buf, size_t len)
{
    //OurPrintFrame(buf, len);
    sendlock.lock();
    size_t sent = 0;
    if (port != 0)
    {
        if (port->isOpen())
            while (sent != len)
            {
                sent += port->write((char *)(buf + sent), len);
                port->waitForBytesWritten(2);
            }
        sendlock.unlock();
        return sent;
    }
    else
    {
        sendlock.unlock();
        return 0;
    }
    sendlock.unlock();
    return sent;
}

size_t QHardDriver::readall(uint8_t *buf, size_t maxlen)
{
    size_t ans = 0;
    if (port != 0)
    {
        if (port->isOpen())
            if (port->bytesAvailable() > 0)
                ans = port->read((char *)buf, maxlen);
    }
    return ans;
}

void QHardDriver::lockMemory() { memory.lock(); }

void QHardDriver::freeMemory() { memory.unlock(); }

void QHardDriver::lockMSG() { msg.lock(); }

void QHardDriver::freeMSG() { msg.unlock(); }

void QHardDriver::setBaudrate(int value) { baudrate = value; }

APIThread::APIThread(CoreAPI *API, int Type, QObject *parent) : QThread(parent)
{
    api = API;
    type = Type;
    interval = 1;
}

void APIThread::run()
{
    while (1)
    {
        if (type == 1)
            api->sendPoll();
        else if (type == 2)
            api->readPoll();
        msleep(interval);
    }
}

APIThread::APIThread()
{
    api = 0;
    type = 0;
}
