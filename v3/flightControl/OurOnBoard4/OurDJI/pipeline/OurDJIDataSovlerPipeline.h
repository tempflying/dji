#ifndef OurDJIDataSovlerPipeline_H
#define OurDJIDataSovlerPipeline_H

#include "OurByteConvertUtil.h"
#include "OurDJIFSMActionPipeline.h"

#define MyProtocolEnum const static char
class OurDJIDataSovlerPipeline : public OurDJIFSMActionPipeline{
public:

//    MyProtocolEnum OUR_TYPE_BYTE = 0x00;
//    MyProtocolEnum OUR_TYPE_SHORT = 0x01;
//    MyProtocolEnum OUR_TYPE_INT = 0x02;
//    MyProtocolEnum OUR_TYPE_LONG = 0x03;
//    MyProtocolEnum OUR_TYPE_FLOAT = 0x04;
//    MyProtocolEnum OUR_TYPE_DOUBLE = 0x05;

//    MyProtocolEnum OUR_TYPE_BYTEARRAY = 0x06;
//    MyProtocolEnum OUR_TYPE_SHORTARRAY = 0x07;
//    MyProtocolEnum OUR_TYPE_INTARRAY = 0x08;
//    MyProtocolEnum OUR_TYPE_LONGARRAY = 0x09;
//    MyProtocolEnum OUR_TYPE_FLOATARRAY = 0x0a;
//    MyProtocolEnum OUR_TYPE_DOUBLEARRAY = 0x0b;

//    char getByte(char* bytes){
//        if(bytes[0] & 0x7f == OUR_TYPE_BYTE){
//            return bytes[1];
//        }
//    }

//    int getInt(char* bytes){
//        if(bytes[0] & 0x7f == OUR_TYPE_INT){

//        }
//    }

// 第一帧前两个字节表示完整传过来的数据的长度
//    OurDJITypeConvert(){
//        isFirstGetData = true;
//        receiveDataByteSize = 0;
//    }
//    ReceivedData receiveDataSyncBuffer;
//    short receiveDataByteSize;
//    bool isFirstGetData;
//    bool step(){
//        ReceivedData rData = getReceiveDataFromMobile();
//        if(rData.length > 0){
//            if(isFirstGetData){
//                isFirstGetData = false;
//                short receiveDataByteSize = OurByteConvertUtil::getShort(rData.data);
//                receiveDataSyncBuffer.data = new char[receiveDataByteSize];

//                memcpy(receiveDataSyncBuffer.data,
//                       rData.data + 2, rData.length - 2);
//                receiveDataSyncBuffer.length = rData.length - 2;
//            }else{
//                memcpy(receiveDataSyncBuffer.data + receiveDataSyncBuffer.length,
//                       rData.data, rData.length);
//                receiveDataSyncBuffer.length += rData.length;
//            }
//            rData.destroy();
//            if(receiveDataSyncBuffer.length == receiveDataByteSize){
//                isFirstGetData = true;
//                receiveDataByteSize = 0;
//                return true;
//            }
//        }
//        return false;
//    }


    // 第一个字节的最高位表示是否是最后一帧
    std::vector<ReceivedData> receiveDataSyncBuffer;
    #define OVER true;
    bool receiveStepOver(){
        ReceivedData rData = getReceiveDataFromMobile();
        if(rData.length > 0){
            receiveDataSyncBuffer.push_back(rData);
            bool flag = ((bool)(rData.data[0] & 80));
            return flag == OVER;
        }
        return false;
    }
    #undef OVER

    // 尝试去获得数据，如果数据接收完毕，会返回接收到的数据，
    // 只能接收到同步发送的数据，也是没有做对接收到的数据座重排序,因为数据量不好，带宽又低，所以异步范松数据没有意义
    // 同步的含义是，上一次发送的数据被接收成功了，才能继续发送新的数据
    ReceivedData getDataSyncFromMobile(){
        ReceivedData rData;
        if(receiveStepOver()){
            // 统计收到的字节数
            size_t byteSize = 0;
            for(size_t i = 0; i < receiveDataSyncBuffer.size(); i++){
                byteSize += receiveDataSyncBuffer.at(i).length - 1;
            }

            // 申请内存空间
            rData.data = new char[byteSize];

            // 将数据拷贝到rData
            for(size_t i = 0; i < receiveDataSyncBuffer.size(); i++){
                memcpy(rData.data + rData.length, receiveDataSyncBuffer.at(i).data + 1,
                       receiveDataSyncBuffer.at(i).length - 1);
            }

            // 释放receiveDataSyncBuffer的内存
            for(size_t i = 0; i < receiveDataSyncBuffer.size(); i++){
                receiveDataSyncBuffer.at(i).destroy();
            }
            receiveDataSyncBuffer.clear();
        }
        return rData;
    }

    void sendStepOver(char* bytes, size_t length){
        if(sendDataFrameSuccess){
            sendDataFrameSuccess = false;
            sendDataTOMobile(bytes, length);
        }
    }

    bool sendDataFrameSuccess;
    virtual void sendDataToMobileCallback(DJI::onboardSDK::CoreAPI * This, Header * header, UserData user){
        API_LOG(This->getDriver(), STATUS_LOG, "------------send data from mobile. header=%p, user=%p\n", header, user);
        unsigned short ack_data = getAckFromHeader(header);
        if(ack_data == ACK_COMMON_SUCCESS){
            sendDataFrameSuccess = true;
        }
    }

    OurDJIDataSovlerPipeline(){
        sendDataFrameSuccess =  true;
    }
};

#endif // OurDJIDataSovlerPipeline.h
