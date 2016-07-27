#include "signal_slot_test.h"

Signal_Slot_Test::Signal_Slot_Test(QObject* parent)
{
    // 注意 SIGNAL 和 SLOT 的宏参数并不是取函数指针，而是除去返回值的函数声明，这是moc的机制，并且 const 这种参数修饰符是忽略不计的.
    connect(this, SIGNAL(mySignal()), this, SLOT(mySlot()));
    connect(this, SIGNAL(mySignal(int)), this, SLOT(mySlot(int)));
    connect(this, SIGNAL(mySignalParam(int , int )), this, SLOT(mySlotParam(int , int )));

    // signal甚至可以关联signal(前者触发后者也触发)
    connect(this, SIGNAL(mySignal_to_signal()), this, SIGNAL(mySignal()));

    // 参数不匹配的signal和slot connect到一起会报一个参数不兼容的警告,并且connect无效
    connect(this, SIGNAL(mySignal()), this, SLOT(mySlotParam(int , int )));

}

void Signal_Slot_Test::mySlot()
{
    printf("call void mySlot()\n");
}

void Signal_Slot_Test::mySlot(int x)
{
    printf("call void mySlot(int x). x=%d\n",x);
}

void Signal_Slot_Test::mySlotParam(int x, int y)
{
    printf("call void mySlotParam(int x, int y). x=%d, y=%d\n",x,y);
}
