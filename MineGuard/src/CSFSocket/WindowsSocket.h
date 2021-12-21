//
//  WindowsSocket.hpp
//  CrossplatformSocketFramework
//
//  Created by Albert Slepak on 7/14/19.
//  Copyright © 2019 Albert Slepak. All rights reserved.
//

#ifndef WindowsSocket_h
#define WindowsSocket_h

#ifdef _WIN32
#include "CSFSocket.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

class WindowsSocket : public CSFSocket
{
public:
    WindowsSocket();

    void Connect(std::string host, int port) override;

    void Bind(std::string host, int port) override;

    void Listen(int connections) override;

    void Accept() override;

    void Close() override;

    void SetBlocking(bool blocking) override;

    size_t Recv(void* result, int bufsize) override;

    std::string Recv(int bufsize, size_t* outbytes) override;

    size_t Send(void* data, int len) override;

    size_t Send(std::string data) override;

    void* GetNativeHandle() override { return (void*)&connfd; };

private:
    SOCKET listenfd = 0, connfd = 0;
};

#endif /* WIN32 */
#endif /* WindowsSocket_h */
