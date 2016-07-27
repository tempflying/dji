#include <QCoreApplication>
#include "signal_slot_test.h"
#include "function_point.h"
#include "qthread_test.h"
#include "timer_test.h"
#include "unistd.h"
#include "MacroTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // test1
    printf("test 1: test QT signal slot\n");
    Signal_Slot_Test sstest;                    // 不依赖于QCoreApplication 的Event Loop
    sstest.emitSignal();
    emit sstest.mySignal();
    printf("\n");

    // test2
    printf("test 2: test function point and call back\n");
    function_point::run_test();
    printf("\n");

    // test3 timer
    printf("test 3: test QObjct timeEvent and QTimer\n");
    timer_test t1;
    t1.runTest(3,1000);        // 执行三遍,间隔为1s. 依赖于不依赖于QCoreApplication的Event Loop
    QTimer_test t2;
    t2.runTest(3, 1000);        // 虽然使用signal-slot,但Qtimer依赖event Loop

    // test4
    printf("test 4: test Macro, typedef, enum\n");
    MacroTest::runTest();
    printf("\n");

    // test5
    printf("test 5: test QThread\n");
    QThread_test mythread;
    mythread.start();

    printf("start Event Loop\n");
    return a.exec();
}
