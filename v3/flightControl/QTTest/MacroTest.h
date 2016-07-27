#ifndef MACROTEST_H
#define MACROTEST_H
#include "stdio.h"
// 测试宏和typedef
namespace MacroTest{
typedef void* UserData;
typedef void(*CallBack)(void*);

enum DIRECTION{
    EAST, WEST, SOURTH, NORTH, DIRECTION_COUNT
};
// 宏定义表达式
#define Max(a,b) ((a>b)?a:b)
// 宏定义泛函数, c++ 使用内联函数来代替宏声明函数的作用，但代替不了泛函的作用，可以根据name生成函数
#define create_func(name,args) \
    void name(){printf("hello world %d\n",args);}
create_func(sayHello, 10);
// 另外有些编译器定义了一些宏，可以指定当前代码执行在第几行,用于debug是的报错，定位很方便

    void runTest(){
        printf("enum  : EAST=%d\n", EAST);
        printf("Macro : Max(1,2)=%d\n",Max(1, 2));
        sayHello();
    }
}

#endif // MACROTEST_H
