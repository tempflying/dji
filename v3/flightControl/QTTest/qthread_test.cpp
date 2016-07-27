#include "qthread_test.h"

QThread_test::QThread_test(QObject *parent) :
    QThread(parent)
{
}

void QThread_test::run()
{
    printf("first print hello in MyQThread run\n");
    sleep(1);
    printf("second print hello in MyQThread run\n\n");
}

