
#include "EventsHandler.h"
#include <oleacc.h>
#include <Winternl.h>
#include <Psapi.h>

#include <QDebug>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "OleAut32.lib")
#pragma comment(lib, "Oleacc.lib")

#define MAX_PATH 256

static UINT   GlobalBuffSize = 0;
static PCHAR GlobalBuff = NULL;

void CALLBACK HandleWinEvent(HWINEVENTHOOK inHook, DWORD inEvent, HWND inHwnd,
                                LONG inIdObject, LONG inIdChild,
                                    DWORD inEventThread, DWORD inMsEventTime){

    if(!inEvent || !inHwnd){

        return;

    }

    IAccessible* Accessible = NULL;
    VARIANT Child;

    if((AccessibleObjectFromEvent(inHwnd, inIdObject, inIdChild, &Accessible, &Child))||(!Accessible)){

        return;

    }

    VARIANT Role;

    if(Accessible->get_accRole(Child, &Role)){

        return;

    }

    Accessible->Release();

    DWORD WinFlag = Role.lVal;

    switch(Role.lVal){

        case ROLE_SYSTEM_CLIENT:

            WinFlag = true;

        break;
        default:

        break;

    }

    DWORD Pid = 0;

    GetWindowThreadProcessId(inHwnd, &Pid);

    if(!Pid){

        return;

    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, Pid);

    if(!hProcess){

        return;

    }

    DWORD Result = 0;
    DWORD BuffSize = GlobalBuffSize;

    while(!QueryFullProcessImageName((HMODULE)hProcess, 0, (LPWSTR)GlobalBuff, (PDWORD)&BuffSize)) {

        if((Result = GetLastError()) != ERROR_INSUFFICIENT_BUFFER){

            break;

        }

        if(GlobalBuff){

            delete [] GlobalBuff;

        }

        BuffSize = GlobalBuffSize += MAX_PATH;

        GlobalBuff = new CHAR [GlobalBuffSize];

    };

    CloseHandle(hProcess);

    if(Result){

        qDebug() << QString("Err: x%1").arg(Result);

        return;

    }

    QString Name = QString::fromWCharArray((wchar_t*)GlobalBuff);

    qDebug() << QString("Name:%1 evt:0x%2 hwnd:0x%3 Window: %4").arg(Name).arg((ulong)inEvent, 0, 16).arg((ulong)inHwnd, 0, 16).arg(WinFlag,0,16);

}


EventsHandler::EventsHandler(QObject *parent):QObject(parent),m_hook(NULL){


}

EventsHandler::~EventsHandler(){

    DeInitialize();

}

DWORD EventsHandler::Initialize(){

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    m_hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_REORDER,
                                NULL,
                                    HandleWinEvent,
                                        0, 0,
                                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    return (m_hook)?NO_ERROR:GetLastError();

}

VOID EventsHandler::DeInitialize(){

    if(m_hook)

        UnhookWinEvent(m_hook);

    m_hook = NULL;

}

void EventsHandler::Process(){

    MSG Message;

    qDebug() << "Process thread started";

    DWORD Result = Initialize();

    if(Result!=NO_ERROR){

        emit Finished();

        return;

    }

    while (DWORD Result = GetMessage(&Message, NULL, 0, 0) != 0){

        if (Result == -1){

            emit Finished();

            return;

        }

        qDebug() << "Process hwnd:"<< Message.hwnd;

    };

    emit Finished();

}

void EventsHandler::Stop(){

    qDebug() << "Stop";

}

ThreadHandler::ThreadHandler(QObject *parent) : QObject(parent), m_Thread(NULL){

}

ThreadHandler::~ThreadHandler(){

    Stop();

}

void ThreadHandler::Start(){

    m_Thread->start();

}

void ThreadHandler::Stop(){

    emit stopAll();

}

DWORD ThreadHandler::Initialize(){

    EventsHandler* EvtHandler = new EventsHandler();

    /*
    DWORD Res = EvtHandler->Initialize();

    if(Res){

        delete EvtHandler;

        return Res;

    }*/

    m_Thread = new QThread;

    EvtHandler->moveToThread(m_Thread);

    connect(m_Thread, SIGNAL(started()), EvtHandler, SLOT(Process()));

    connect(EvtHandler, SIGNAL(Finished()), m_Thread, SLOT(quit()));

    connect(this, SIGNAL(stopAll()), EvtHandler, SLOT(Stop()));

    connect(EvtHandler, SIGNAL(Finished()), EvtHandler, SLOT(deleteLater()));
    connect(m_Thread, SIGNAL(finished()), m_Thread, SLOT(deleteLater()));

    return NO_ERROR;

}





