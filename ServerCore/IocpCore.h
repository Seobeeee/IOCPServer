#pragma once

class IocpObject
{
public:
    virtual HANDLE GetHandle() abstract;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE      GetHandle() { return _iocpHandle; }

    bool        Register(class IocpObject* iocpObject);
    bool        Dispatcher(uint32 timeoutMs = INFINITE);

private:
    HANDLE      _iocpHandle;
};


// 임시로 전역으로 선언
extern IocpCore GIocpCore;
