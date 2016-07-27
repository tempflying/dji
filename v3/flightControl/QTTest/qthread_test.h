#ifndef QTHREAD_TEST_H
#define QTHREAD_TEST_H

#include <QThread>

class QThread_test : public QThread
{
    Q_OBJECT
public:
    explicit QThread_test(QObject *parent = 0);

    void run();
signals:

public slots:

};

#endif // QTHREAD_TEST_H
