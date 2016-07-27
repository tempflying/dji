#include <QCoreApplication>
#include <QSerialPort>
#include <QonboardSDK.h>
#include "OurDJI/OurWayPointPipeline.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);         // 使用startTimer 必须要创建该对象,也就是start event loop；否则报错如下
                                            //   QObject::startTimer: Timers can only be used with threads started with QThread

//    OurDJIPipelineInterface* pipeline = new OurStarPipeline();
    OurDJIPipelineInterface* pipeline = new OurWayPointPipeline();
    pipeline->start(20);

    //while(true);
    printf("Main thread enter Event Loop\n");
    return a.exec();                        // 进入本线程（不能跨线程）主事件循环,所有的Qtimer startTimer都属于事件。要使用startTimer的第二个条件就是 调用它的exec函数，否则不报错，但函数不会被执行
}
