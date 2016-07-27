#ifndef SIGNAL_SLOT_TEST_H
#define SIGNAL_SLOT_TEST_H
#include "QObject"
#include "stdio.h"
// 参考链接 http://blog.chinaunix.net/uid-20348984-id-1964206.html
// 1. signal slot 机制是所有QT对象的基类 QObject 的机制,所有继承自QObject的类都可以使用该机制
// 2. 使用方法:
//    a. 继承任何一个QT类,
//    b. 使用Q_OBJECT宏(这个宏好像是表示把当前文中的所有内容先编译一遍)
//       (使用这个宏时类函数的实现必须和声明写在一起或者写到.cpp文件中,不能写在同一个文件的类外)
//    c. 声明一个signal,不用实现; 声明一个slot,需要实现
//    d. 使用connect把signal和slot关联起来
//       (singal 和 slot参数要匹配,否则connect到一起会报一个参数不兼容的警告,并且connect无效)
//    e. emit signalFunction(args..) 发射信号,效果是调用和该信号关联起来的所有slot函数
//    f. (可选)使用 disconnect 断开连接, 注意可以使用0作为通配符 disconnect(myObject,0,0,0)
//       或 myObject->disconnect()断开一切使用MyObject的signal的connect
// 3. 注意事项
//    a. signals的函数返回只能是void型,不用写函数体,QT会自动生成；而slots的函数和一般函数一样,有函数体且分public/protected/private
//       signals函数可以带参数; emit时需要把参数带上;slot可以接受到emit的signal带的参数值.(因此slot和signal的参数要是一致的)
//    b. 注意 SIGNAL 和 SLOT 的宏参数并不是取函数指针，而是除去返回值的函数声明，这是moc的机制，并且 const 这种参数修饰符是忽略不计的.
//       static bool QObject::connect(const QObject* sender,const char* signal,const QObject* receiver,const char* member)
//       函数中对应的参数类型是const char *,也就是一堆字符,底层应该用到了反射机制
//    c. 为什么会用到 和 在哪里会用到 signal 和 slot机制？
//          一般signal函数是继承的父类提供的,含有某些功能,slot函数是自己写的对应的操作，
//          比如 connect(qButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(on_btg_flight_HL(QAbstractButton *)));
//              signal 函数是button被点击时发出的信号,是控件类提供的; slot函数是写的处理事件的函数。其实这就是对可视化观察者模式的一种实现。
//          比如 connect(qTimer, SIGNAL(timeout()), this, SLOT(on_tmr_Broadcast())); qTimer->start(300);
//              signal 函数是计时器每过一定时间发出的信号,有QTimer提供; slot函数是自己写的每隔一定时间就执行一次的函数
//
//    ---------------下面是一些不常用的细节-----------------------------------------------------------------------------------------------------------
//
//    d. signal-slot机制可以代替回调函数机制
//       信号与槽的效率是非常高的，但是同真正的回调函数比较起来，由于增加了灵活性，因此在速度上还是有所损失，当然这种损失相对来说是比较小的，
//    e. 信号与槽机制与普通函数的调用一样，如果使用不当的话，在程序执行时也有可能产生死循环。因此，在定义槽函数时一定要注意避免间接形成无限循环，即在槽中再次发射所接收到的同样信号。
//       例如,在下面给出的例子中如果在mySlot()槽函数中加上语句emit mySignal()即可形成死循环。
//    f. 一个signal可以关联多个slot,一个slot也可以关联多个signal,signal甚至可以关联signal(前者触发后者也触发)
//       如果一个信号与多个槽相联系的话，那么，当这个信号被发射时，与之相关的槽被激活的顺序将是随机的。
//    g. 构造函数不能用在signals或者slots声明区域内。
//    h. 宏定义不能用在signal和slot的参数中。因为moc工具不扩展#define,而SIGNAL,SLOT正是moc宏
//    i. 函数指针不能作为信号或槽的参数,但可以使用typedef绕过这个限制
//          void apply(void (*applyFunction)(QList*, void*), void*); // 不合语法
//          typedef void (*ApplyFunctionType)(QList*, void*);void apply( ApplyFunctionType, void *);   // 合法
//    j. 信号与槽不能有缺省参数。 既然signal->slot绑定是发生在运行时刻，那么，从概念上讲使用缺省参数是困难的。
//          void someSlot(int x=100); // 将x的缺省值定义成100，在槽函数声明中使用是错误的
//    k. 信号与槽也不能携带模板类参数。如果将信号、槽声明为模板类参数的话，即使moc工具不报告错误，也不可能得到预期的结果。
//          public signals: void MyObject::moved (pair location); // 不合法pair模板类
//       可以使用typedef绕过这个问题。
//          typedef pair IntPair;public signals: void MyObject::moved (IntPair location);// 合法
//    l. 嵌套的类不能位于信号或槽区域内，也不能有信号或者槽。
//    m. 友元声明不能位于信号或者槽声明区内. signals: friend class ClassTemplate; // 此处定义不合语法

class Signal_Slot_Test : public QObject
{
// 凡是 QObject 类（不管是直接子类还是间接子类），都应该在第一行代码写上 Q_OBJECT。不管是不是使用信号槽，都应该添加这个宏。
// 这个宏的展开将为我们的类提供信号槽机制、国际化机制以及 Qt 提供的不基于 C++ RTTI 的反射能力。
//      因此，如果你觉得你的类不需要使用信号槽，就不添加这个宏，就是错误的。其它很多操作都会依赖于这个宏。
// 注意，这个宏将由 moc（这里你可以将moc理解为一种预处理器，是比 C++ 预处理器更早执行的预处理器。） 做特殊处理，不仅仅是宏展开这么简单。
//      moc 会读取标记了 Q_OBJECT 的头文件，生成以 moc_ 为前缀的文件
Q_OBJECT        // 这个宏必不可少,否则就还报错找不到定义的signal

signals :
    void mySignal_to_signal();
    void mySignal();
    void mySignal(int x);
    void mySignalParam(int x, int y);

public slots:
    void mySlot();
    void mySlot(int x);
    void mySlotParam(int x, int y);

public:
    Signal_Slot_Test(QObject* parent=0);
    void emitSignal(){
        emit mySignal_to_signal();
        emit mySignal(5);

        emit mySignalParam(4, 5);
    };

};
#endif // SIGNAL_SLOT_TEST_H
