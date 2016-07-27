#include "timer_test.h"

timer_test::timer_test(QObject *parent) :
    QObject(parent)
{
    timerid = -1;
    count = 0;
    execTimes = 3;
}

void timer_test::runTest(int execTimes,int interval)
{
    this->execTimes = execTimes;
    timerid = startTimer(interval);
}

void timer_test::timerEvent(QTimerEvent *)
{
    count ++;
    printf("timer: timerEvent from QObject exec %d times\n", count);
    if(timerid > 0 && count >= execTimes){
        killTimer(timerid);
        timerid = -1;
        count = 0;
    }
}


QTimer_test::QTimer_test(QObject *parent)
{
    timer = 0;
    count = 0;
    execTimes = 3;
}

void QTimer_test::runTest(int execTimes,int interval)
{
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(myTimeEvent()));
    timer->setInterval(interval);
    timer->start();
}

void QTimer_test::myTimeEvent()
{
    count ++;
    printf("qtimer: mytimerEvent from QTimer exec %d times\n", count);
    if(timer && count >= execTimes){
        timer->stop();
    }
}
