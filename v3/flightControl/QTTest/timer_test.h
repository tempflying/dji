#ifndef TIMER_TEST_H
#define TIMER_TEST_H

#include <QTimer>

class timer_test : public QObject
{
    Q_OBJECT
public:
    explicit timer_test(QObject *parent = 0);

    void runTest(int execTimes=3, int interval=1000);
protected:
    virtual void timerEvent(QTimerEvent *);
    int timerid;        // 开启的计时器的id
    int count;          // 程序的计时器函数已经执行了多少次
    int execTimes;      // 程序的计时器函数执行多少次

signals:

public slots:

};

class QTimer_test : public QObject
{
     Q_OBJECT
public :
    explicit QTimer_test(QObject *parent = 0);
    void runTest(int execTimes=3,int interval=1000);
public slots:
    void myTimeEvent();
protected:
    QTimer* timer;
    int count;
    int execTimes;
};

#endif // TIMER_TEST_H
