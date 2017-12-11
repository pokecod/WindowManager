#ifndef EVENTSHANDLER_H
#define EVENTSHANDLER_H

#include <qt_windows.h>
#include <QObject>
#include <QThread>

void CALLBACK HandleWinEvent(HWINEVENTHOOK inHook, DWORD inEvent, HWND inHwnd,
                                LONG inIdObject, LONG inIdChild,
                                    DWORD inEventThread, DWORD inMsEventTime);

class EventsHandler : public QObject{

    Q_OBJECT

public:

    explicit EventsHandler(QObject *parent = 0);

    ~EventsHandler();

    DWORD Initialize();

    VOID DeInitialize();

private:

    HWINEVENTHOOK m_hook;

public:
signals:

    void Finished();

public slots:

    void Process();
    void Stop();

};

class ThreadHandler : public QObject{

    Q_OBJECT

public:
    explicit ThreadHandler(QObject *parent = 0);

    ~ThreadHandler();

    DWORD Initialize();

    void Start();

    void Stop();

signals:

    void stopAll();

public slots:

private:

    QThread* m_Thread;

};

#endif // EVENTSHANDLER_H
