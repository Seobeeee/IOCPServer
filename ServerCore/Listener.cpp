#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"

Listener::~Listener()
{
    SocketUtils::Close(_socket);
    for (AcceptEvent* acceptEvent : _acceptEvents)
    {
        xdelete(acceptEvent);
    }
}

bool Listener::StartAccept(NetAddress netAddress)
{
    if (_socket == INVALID_SOCKET)
        return false;

    if (GIocpCore.Register(this) == false)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Bind(_socket, netAddress) == false)
        return false;
    
    if (SocketUtils::Listen(_socket) == false)
        return false;

    const int32 acceptCount = 1;
    for (int32 i = 0; i < acceptCount; i++)
    {
        AcceptEvent* acceptEvent = xnew<AcceptEvent>();
        _acceptEvents.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }
    return false;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    Session* session = xnew<Session>();
    acceptEvent->Init();
    acceptEvent->SetSession(session);

    DWORD bytesRecved = 0;
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesRecved, static_cast<LPOVERLAPPED>(acceptEvent)))
    {
        const int32 errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            // 무언가의 문제가 발생. 에러로그 필요.
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    Session* session = acceptEvent->GetSession();
    if (SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket) == false)
    {
        RegisterAccept(acceptEvent);
        return;
    }


    // 접속한 유저의 정보를 추출.
    SOCKADDR_IN sockAddress;
    int32 sizeofSockAddr = sizeof(sockAddress);
    if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeofSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(NetAddress(sockAddress));
    cout << "Cliend Connected" << endl;
    RegisterAccept(acceptEvent);
}
