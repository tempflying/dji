#include "OurDJIPipelineInterface.h"
#include <pthread.h>

OurDJIPipelineInterface::OurDJIPipelineInterface(QObject *parent) //: QObject(parent)
{
    port = new QSerialPort();
    driver = new QHardDriver(port);

    api = new CoreAPI(driver);
    api->setVersion(versionM100_31);
    api->setFromMobileCallback(OurDJIPipelineInterface::pFromMobileCallBack, this);

    receivedDataBuffer.data = new char[101];

    flight = new Flight(api);
    ourCurrentState = OUR_WAITTING;
}

void OurDJIPipelineInterface::start(int interval,const char *portName, int baudrate, int id)
{
    this->interval = interval;
    if(reOpenPort(portName, baudrate)){
        active(id);
    }

    ourCurrentState = OUR_BEGIN;
    // 开始进行帧刷新
    startTimer(interval);
//    while(true){
//        timerEvent(NULL);
//        usleep(interval * 1000);
//    }

//    APIThread *send = new APIThread(api, 1);
//    APIThread *read = new APIThread(api, 2);
//    send->setInterval(interval);
//    read->setInterval(interval);
//    send->start();
    //    read->start();
}

void OurDJIPipelineInterface::whenInMainFlight()
{
    mainFlight();
    landing();
    ourCurrentState = OUR_LANDDING;
}

bool OurDJIPipelineInterface::reOpenPort(const char *portName, int baudrate)
{
    if(portName == NULL) portName = getSerialPort();
    if(portName == NULL) {
        API_LOG(driver, ERROR_LOG, "[OUR] : open serial open failed cause portName is NULL");
        return false;
    }

    port->setPortName(portName);
    port->setBaudRate(baudrate);
    driver->setBaudrate(baudrate);
    driver->init();
    return driver->initresult;
}

void OurDJIPipelineInterface::closePort()
{
    if(port) port->close();
}

void OurDJIPipelineInterface::active(int id)
{
    acdata.ID = id;
    QByteArray* key = new QByteArray;
    // key已经是没有用的了
    *key = QByteArray("1bee7d67ad5cd35364f334f399d524046c39fca18f50b7ce98d98bce1f556f94");
    acdata.encKey = key->data();

    //api->activate(&data);
    api->activate(&acdata, OurDJIPipelineInterface::pActivationCallback, this);
}

void OurDJIPipelineInterface::setControl(bool operate){
    api->setControl(operate, OurDJIPipelineInterface::pSetControlCallBack, this);
}

void OurDJIPipelineInterface::takeoff()
{
    ourFlightState = OUR_TAKEOFF;
    flight->task(Flight::TASK_TAKEOFF, OurDJIPipelineInterface::pAutoFlightCallBack, this);
    initAutoFlightSafeCondition();
}

void OurDJIPipelineInterface::landing()
{
    ourFlightState = OUR_LANDDING;
    printf("********************************************* landding  \n");
    flight->task(Flight::TASK_LANDING, OurDJIPipelineInterface::pAutoFlightCallBack, this);
    initAutoFlightSafeCondition();
}

void* OurDJIPipelineInterface::mainFlightThread(void* This)
{
    OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*) This;
    pipeline->mainFlight();
    pipeline->landing();
    pipeline->ourCurrentState = OUR_LANDDING;
    return 0;
}

ReceivedData OurDJIPipelineInterface::getReceiveDataFromMobile()
{
    ReceivedData rData;
    if(receivedDataBuffer.length > 0){
        rData.data= new char[receivedDataBuffer.length];
        memcpy(rData.data, receivedDataBuffer.data, receivedDataBuffer.length);
        rData.length = receivedDataBuffer.length;
        receivedDataBuffer.length = 0;
    }
    return rData;
}

void OurDJIPipelineInterface::sendDataTOMobile(char *data, uint8_t len)
{
    api->sendToMobile((uint8_t*)data, len, OurDJIPipelineInterface::pSendMobileCallBack, this);
}

bool OurDJIPipelineInterface::createThread(void *(* func)(void *), void *arg)
{
    int res;
    pthread_t a_thread;

    res = pthread_create(&a_thread, NULL, func, arg);
    if (res != 0)
    {
        API_LOG(driver, ERROR_LOG, "Thread creation failed!");
        return false;
    }

    //printf("Waiting for thread to finish.../n");

//    void *thread_result;
//    res = pthread_join(a_thread, &thread_result);
//    if (res != 0)
//    {
//        API_LOG(driver, ERROR_LOG, "Thread join failed!/n");
//        return false;
//    }
//    printf("Thread joined, it returned %s/n", (char *)thread_result);
    return true;
}

void OurDJIPipelineInterface::timerEvent(QTimerEvent *)
{
    // 尝试从串口中读取信息,并根据信息调用回到函数
    api->readPoll();

    // 根据当前的状态更新信息,由于要保证‘帧率’，所以如果操作比较耗时,请把操作放到异步线程中
    // 并切换当前状态到OUR_WAITTING,使得update等待异步线程执行,可以参考状态为OUR_MAINFLIGHT的使用
    this->update();

    // 尝试把缓存里存下的指令写入串口（每次发送指令都会把数据指令存入缓存，
    // 每次接收到指令的ack指令信号,都会把该指令缓存中移除）
    api->sendPoll();
    //printf("times : %d\n", times++);
}

void OurDJIPipelineInterface::update()
{
    printf("ourcurrentState: %d %d\n", int(ourCurrentState), times * interval);
    switch(ourCurrentState){            // 当前所处的状态

    case OUR_WAITTING:
        break;

    case OUR_BEGIN:                     // 暂时无用 1
        ourCurrentState = OUR_WAITTING;
        break;

    case OUR_ACTIVATED:                 // 刚刚激活成功 2
        setControl(OBTAIN_CONTROL);
        ourCurrentState = OUR_WAITTING;
        break;

    case OUR_OBTAINCONTROL_RUNNING:     // 控制权第一步成功 3
        ourCurrentState = OUR_WAITTING;
        break;

    case OUR_OBTAINCONTROL_SUCCESS:     // 获得控制权成功 4
        takeoff();
        ourCurrentState = OUR_TAKEOFF;
        break;

    case OUR_TAKEOFF:                   // 起飞中 5
        if(times * interval > 8000){
            ourCurrentState = OUR_MAINFLIGHT;
            times = 0;
        }
        times ++;
        break;

    case OUR_MAINFLIGHT:                // 核心飞控程序 6
//        if(!createThread(OurDJIPipelineInterface::mainFlightThread, this)){
//            landing();
//            ourCurrentState = OUR_LANDDING;
//        }
        whenInMainFlight();

        break;

    case OUR_LANDDING:                  // 降落中 7
        if(times * interval > 6000){
            setControl(RELEASE_CONTROL);
            ourCurrentState = OUR_WAITTING;
            times = 0;
        }
        times ++;
        break;

    case OUR_RELEASECONTROL_RUNNING:    // 释放控制权第一步成功
        ourCurrentState = OUR_WAITTING;
        break;

    case OUR_RELEASECONTROL_SUCCESS:    // 释放控制权成功
        closePort();
        ourCurrentState = OUR_WAITTING;
        break;
    }
}

// 串口相关的函数
void OurDJIPipelineInterface::listSerialPort(){
    printf("list possible serial port : \n");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        printf("    portName     : %s\n",info.portName().toStdString().data());
        printf("    description  : %s\n",info.description().toStdString().data());
        printf("    manufacturer : %s\n",info.manufacturer().toStdString().data());
        printf("\n");
    }
}

const char* OurDJIPipelineInterface::getSerialPort(){
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

unsigned short OurDJIPipelineInterface::getAckFromHeader(Header *header)
{
    unsigned short ack_data = ACK_COMMON_NO_RESPONSE;
    if (header->length - EXC_DATA_SIZE <= 2)
    {
        memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
               (header->length - EXC_DATA_SIZE));
    }
    return ack_data;
}

unsigned short OurDJIPipelineInterface::getAckFromFrame(Header *header){
    unsigned short ack_data = ACK_COMMON_NO_RESPONSE;
    if (header->length - EXC_DATA_SIZE > 0)
    {
        unsigned int length = header->length - EXC_DATA_SIZE;
        if(length > 2) length = 2;
        memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
               length);
    }
    return ack_data;
}

size_t OurDJIPipelineInterface::getDataFromFrame(Header *header, char* ack_data)
{
    size_t len = 0;
    //printf("%d, %d\n", sizeof(Header), header->length);
    if(ack_data == 0)
        ack_data = new char[100];                         // 单次数据不会超过100B
    if (header->length - EXC_DATA_SIZE > 2)
    {
        // 协议侦除去data后长度为16B,data前12B,data后的CRC32 4B
        // data的开头两个byte一般表示应答或CMD_SET+CMD_ID,真正的数据在后面,所以要加2
        len = header->length - EXC_DATA_SIZE - 2;
        memcpy((unsigned char *)ack_data, ((unsigned char *)header) + sizeof(Header) + 2,
               (len));
        ack_data[header->length - EXC_DATA_SIZE - 2] = '\0';
    }else{
        ack_data[0] = '\0';
    }
    return len;
    //printf("%d\n",strlen(ack_data));
}

// 激活回调函数
void OurDJIPipelineInterface::pActivationCallback(CoreAPI *This, Header *header, UserData userData){
    // 获得ack_data
    unsigned short ack_data = getAckFromHeader(header);

    // API 自带的回调函数，这个结束了才是真正激活完成
    This->activateCallback(This, header);

    // 通过多态调用用户设置的回调函数, 成功则尝试获得控制权
    OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*)userData;
    if(pipeline != NULL && ack_data == ACK_ACTIVE_SUCCESS){
        //pipeline->activeSuccess(ack_data);
        pipeline->ourCurrentState = OUR_ACTIVATED;
    }
}

// 控制权回调函数, CoreSetControlCoreback 来自于对Flight::taskCallback的修改
// pSetControlCallBack是自己定义的回调函数
// 注意根据官方文档,获取和释放控制权都是要发送两次请求的，一次返回running，一次返回success
// 实际测试时发现有时候会第一次就返回success
void OurDJIPipelineInterface::CoreSetControlCoreback(CoreAPI* This,OurDJIPipelineInterface* pipeline, unsigned short ack_data)
{
    unsigned char data = 0x1;
    switch (ack_data)
    {
        case ACK_SETCONTROL_NEED_MODE_F:
            API_LOG(pipeline->driver, STATUS_LOG, "Obtain control failed, Conditions did not "
                                              "satisfied");
            break;
        case ACK_SETCONTROL_RELEASE_SUCCESS:
            API_LOG(pipeline->driver, STATUS_LOG, "release control successfully\n");
            break;
        case ACK_SETCONTROL_OBTAIN_SUCCESS:
            API_LOG(pipeline->driver, STATUS_LOG, "obtain control successfully\n");
            break;
        case ACK_SETCONTROL_OBTAIN_RUNNING:
            API_LOG(pipeline->driver, STATUS_LOG, "obtain control running\n");
            This->send(2, DJI::onboardSDK::encrypt, SET_CONTROL, CODE_SETCONTROL, &data, 1, 500,
                       2, OurDJIPipelineInterface::pSetControlCallBack, pipeline);
            break;
        case ACK_SETCONTROL_RELEASE_RUNNING:
            API_LOG(pipeline->driver, STATUS_LOG, "release control running\n");
            data = 0;
            This->send(2, DJI::onboardSDK::encrypt, SET_CONTROL, CODE_SETCONTROL, &data, 1, 500,
                       2, OurDJIPipelineInterface::pSetControlCallBack, pipeline);
            break;
        case ACK_SETCONTROL_IOC:
            API_LOG(pipeline->driver, STATUS_LOG, "IOC mode opening can not obtain control\n");
            break;
        default:
            if (!This->decodeACKStatus(ack_data))
            {
                API_LOG(pipeline->driver, ERROR_LOG, "While calling this function");
            }
            break;
    }
}
void OurDJIPipelineInterface::pSetControlCallBack(CoreAPI *This, Header *header, UserData userData){    
    // 获取pipeline对象
    OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*)userData;
    if(pipeline == NULL){
        printf("[Our] big error pipeline is NULL in OurDJIPipelineInterface line 184\n");
        return;
    }

    // 如果ack_Data无效则终止函数
    unsigned short ack_data = getAckFromHeader(header);
    if(ack_data == ACK_COMMON_NO_RESPONSE)
    {
        API_LOG(pipeline->driver, ERROR_LOG, "ACK is exception,seesion id %d,sequence %d\n",
                header->sessionID, header->sequenceNumber);
        return;
    }
    if(ack_data == ACK_SETCONTROL_NEED_MODE_F){
        API_LOG(pipeline->driver, ERROR_LOG, "RC is not in F mode\n");
        return;
    }
    if(ack_data == ACK_SETCONTROL_IOC){
        API_LOG(pipeline->driver, ERROR_LOG, "Go APP is not close IOC\n");
        return;
    }

    API_LOG(pipeline->driver, STATUS_LOG, "[OUR] setControlResult***************** %d\n", int(ack_data));
    // 通过多态调用用户设置的回调函数
    if(ack_data == ACK_SETCONTROL_OBTAIN_RUNNING){
        //pipeline->obtainControlRunning(ack_data);
        pipeline->ourCurrentState = OUR_OBTAINCONTROL_RUNNING;
        CoreSetControlCoreback(This, pipeline, ack_data);
    }
    else if(ack_data == ACK_SETCONTROL_RELEASE_RUNNING){
        //pipeline->releaseControlRunning(ack_data);
        pipeline->ourCurrentState = OUR_RELEASECONTROL_RUNNING;
        CoreSetControlCoreback(This, pipeline, ack_data);
    }

    if(ack_data == ACK_SETCONTROL_OBTAIN_SUCCESS){
        //pipeline->obtainControlSuccess(ack_data);
        pipeline->ourCurrentState = OUR_OBTAINCONTROL_SUCCESS;

    }else if(ack_data == ACK_SETCONTROL_RELEASE_SUCCESS){
        //printf("***********************release control\n");
        //pipeline->releaseControlSuccess(ack_data);
        pipeline->ourCurrentState = OUR_RELEASECONTROL_SUCCESS;
    }
}

// 自动飞行 takeoff gohome landding的回调函数,注意回调并不是在执行完成的时候调用,而是在开始执行的时候调用
void OurDJIPipelineInterface::pAutoFlightCallBack(CoreAPI *This, Header *header, UserData userData)
{   
    printf("task callback \n");
    Flight::taskCallback(This, header);
    unsigned short ack_data = getAckFromHeader(header);
    if(ack_data != ACK_COMMON_NO_RESPONSE){
        printf("****************************************************************\n");
        OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*)userData;
        if(pipeline == NULL){
            printf("[Our] big error pipeline is NULL in OurDJIPipelineInterface line 223\n");
            return;
        }

        if(pipeline->ourFlightState == OUR_TAKEOFF){
            printf("task callback TAKEOFF\n");
        }else if(pipeline->ourFlightState == OUR_LANDDING){
            printf("task callback LANDDING\n");
        }
    }
}

// 与手机进行数据交互的回调函数
void OurDJIPipelineInterface::pFromMobileCallBack(CoreAPI *This, Header *header, UserData userData)
{
    // 获取pipeline对象
    OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*)userData;
    if(pipeline == NULL){
        printf("[Our] big error pipeline is NULL in OurDJIPipelineInterface pFromMobileCallBack\n");
        return;
    }

    API_LOG(This->getDriver(), STATUS_LOG, "------------------------------------------ receive data from mobile\n");

    pipeline->receivedDataBuffer.length = getDataFromFrame(header, pipeline->receivedDataBuffer.data);
    if(pipeline->receivedDataBuffer.length > 0){
        printf("receive data: ");
        //pipeline->printReceiveDataInInt(pipeline->receivedDataBuffer);
        API_LOG(pipeline->driver, STATUS_LOG, "%d\n", pipeline->receivedDataBuffer.length);
    }
    else{
        API_LOG(pipeline->driver, STATUS_LOG, "data is null\n");
    }
}

void OurDJIPipelineInterface::pSendMobileCallBack(CoreAPI *This, Header *header, UserData userData)
{
    // 获取pipeline对象
    OurDJIPipelineInterface* pipeline = (OurDJIPipelineInterface*)userData;
    if(pipeline == NULL){
        printf("[Our] big error pipeline is NULL in OurDJIPipelineInterface line pSendMobileCallBack\n");
        return;
    }

    This->sendToMobileCallback(This, header);
    pipeline->sendDataToMobileCallback(This, header, userData);

    //getchar();
}
