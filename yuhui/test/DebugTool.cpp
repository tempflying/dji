#include "DebugTool.h"
#include "stdio.h"
#include "string.h"

// 数据是小端派编址，而打印是从地地址开始的，所以对于一个字段如果包含多个字节，他的顺序是反的
// unsigned char 很多时候是用来存整数的，即码号
void printFrame(unsigned char* buf, int len){
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
    int i = 12;
    printf("Data/%d * 8:            : ", (len-16));
    for(;i < len - 4; i++){
        if((i - 12) % 8 == 0)
            printf("\n                          ");     // 数据是小端派编址，而打印是从地地址开始的，所以对于一个字段如果包含多个字节，他的顺序是反的
        printf("%02x ",buf[i]);
    }
    printf("\n");
    printf("CRC32               : %02x %02x %02x %02x\n", buf[len-4], buf[len-3], buf[len-2],buf[len-1]);
}

void print(activate_data_t user_act_data){
    printf("appId       : %d\n",user_act_data.app_id);
    printf("appLevel    : %d\n", user_act_data.app_api_level);
    printf("appVersion  : %d\n",user_act_data.app_ver);
    printf("appBundleId : ");
    for(int i = 0; i < 32; i ++){
        if(i % 8 == 0) printf("\n            : ");
        printf("%02x ",user_act_data.app_bundle_id[i]);
    }
    printf("\n");
    int len = strlen(user_act_data.app_key);
    printf("appKey      : ");
    for(int i = 0; i < len; i++){
        if(i % 8 == 0) printf("\n            : ");
        printf("%c",user_act_data.app_key[i]);
    }
    printf("\n");
}
