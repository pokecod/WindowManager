
#include "EventsHandler.h"
#include "Window.h"
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[]){


    QApplication a(argc, argv);

    ThreadHandler* ThHandler = new ThreadHandler();

    DWORD Res = ThHandler->Initialize();

    qDebug() << "Res:"<<Res;

    ThHandler->Start();

    Window w;
    w.show();

    int res = a.exec();

    CoUninitialize();

    return res;
}
