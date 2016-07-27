#ifndef FUNCTION_POINT_H
#define FUNCTION_POINT_H

#include "stdio.h"

// namespace 可用于解决重名问题
// 注意如果做参数的 回调函数 是类的成员函数，那么他必须是static
namespace function_point{

    void function1(int a){
        printf("this is function1. a=%d\n",a);
    }

    void function2(void (*func)(int)){
        printf("call back in function2: ");
        // 回调函数
        (*func)(2);
    }

    typedef void (*CallBack)(int);
    void function3(CallBack callback){
        printf("call back in function2: ");
        // 回掉函数
        (*callback)(3);
    }

    void run_test(){
        // 函数指针的赋值
        void (*func)(int);
        func = &function1;
        (*func)(1);                         // function1(1);

        // 把函数指针作为参数传入
        function2(&function1);
        function3(func);
    }
}

#endif // FUNCTION_POINT_H
