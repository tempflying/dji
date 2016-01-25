#include "mainwindow.h"
#include <QApplication>

#include "OurDJI_FUNC.h"

int main(int argc, char *argv[])
{
    DJI_Sample_Setup();
    listSerialPort();
    openSerialport("COM5");
    active();
    obtainControl();


}



