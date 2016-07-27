#ifndef OURBYTECONVERTUTIL_H
#define OURBYTECONVERTUTIL_H

class OurByteConvertUtil{
public :
    // 注意getBytes获得的都是动态的内存数据，因为数组不允许改变指针

    static char* getBytes(short s){
        short* temp = new short[1];
        temp[0] = s;
        char* bytes = (char*)temp;
        return bytes;
    }

    static char* getBytes(int n){
        int* temp = new int[1];
        temp[0] = n;
        char* bytes = (char*)temp;
        return bytes;
    }

    static char* getBytes(float f){
        float* temp = new float[1];
        temp[0] = f;
        char* bytes = (char*)temp;
        return bytes;
    }

    static char* getBytes(long l){
        long* temp = new long[1];
        temp[0] = l;
        char* bytes = (char*)temp;
        return bytes;
    }

    static char* getBytes(double d){
        double* temp = new double[1];
        temp[0] = d;
        char* bytes = (char*)temp;
        return bytes;
    }

    static short getShort(char* bytes){
        return *((short*)bytes);
    }

    static int getInt(char* bytes){
        return *((int*)bytes);
    }

    static float getFloat(char* bytes){
        return *((float*)bytes);
    }

    static double getDouble(char* bytes){
        return *((double*)bytes);
    }

    static long getLong(char* bytes){
        return *((long*)bytes);
    }

    static float* getFloatArray(char* bytes, unsigned int bytesLength){
        if(bytesLength < 4) return 0;
        float* array = new float[bytesLength / 4];
        for(unsigned int i = 0; i < bytesLength / 4 ; i++){
            array[i] = *((float*)(bytes + 4 * i));
        }
        return array;
    }

};

#endif // OURBYTECONVERTUTIL_H
