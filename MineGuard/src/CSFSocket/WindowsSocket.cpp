//
//  WindowsSocket.cpp
//  CrossplatformSocketFramework
//
//  Created by Albert Slepak on 7/14/19.
//  Copyright © 2019 Albert Slepak. All rights reserved.
//

#include "WindowsSocket.h"
#ifdef _WIN32

WindowsSocket::WindowsSocket() {}

void WindowsSocket::Connect(std::string host, int port)
{
    WSAData wsdata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsdata);
    if (result != 0) {
        throw "Failed to initialize WinSock.";
        return;
    }

    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd == INVALID_SOCKET) {
        WSACleanup();
        throw "Failed to create socket.";
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    result = inet_pton(AF_INET, host.c_str(), &hint.sin_addr);
    if (result <= 0) {
        WSACleanup();
        throw "Invalid host address.";
        return;
    }

    result = connect(connfd, (sockaddr*)&hint, sizeof(hint));
    if (result == SOCKET_ERROR)
    {
        throw "Failed to connect to server.";
        return;
    }
}

void WindowsSocket::Bind(std::string host, int port)
{
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        throw "Failed to initialize WinSock.";
        return;
    }

    struct addrinfo* addrinforesult = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &addrinforesult);
    if (result != 0) {
        WSACleanup();
        throw "Failed to resolve host address or port.";
        return;
    }

    listenfd = socket(addrinforesult->ai_family, addrinforesult->ai_socktype, addrinforesult->ai_protocol);
    if (listenfd == INVALID_SOCKET) {
        freeaddrinfo(addrinforesult);
        WSACleanup();
        throw "Failed to create listening socket.";
        return;
    }

    result = bind(listenfd, addrinforesult->ai_addr, (int)addrinforesult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        freeaddrinfo(addrinforesult);
        closesocket(listenfd);
        WSACleanup();
        throw "Failed to bind listening socket.";
        return;
    }

    freeaddrinfo(addrinforesult);
}

void WindowsSocket::Listen(int connections)
{
    int result = listen(listenfd, connections);
    if (result == SOCKET_ERROR) {
        closesocket(listenfd);
        WSACleanup();
        throw "Failed to listen for connections.";
        return;
    }
}

void WindowsSocket::Accept()
{
    connfd = accept(listenfd, NULL, NULL);
    if (connfd == INVALID_SOCKET) {
        closesocket(listenfd);
        WSACleanup();
        throw "Failed to accept connection.";
        return;
    }
}

void WindowsSocket::Close()
{
    closesocket(listenfd);
    closesocket(connfd);
}

void WindowsSocket::SetBlocking(bool blocking)
{
    unsigned long mode = (unsigned long)blocking;
    ioctlsocket(connfd, FIONBIO, &mode);
}

size_t WindowsSocket::Recv(void* result, int bufsize)
{
    return recv(connfd, (char*)result, bufsize, 0);
}

std::string WindowsSocket::Recv(int bufsize, size_t* outbytes)
{
    std::string result = "";

    char* buffer = new char[bufsize];
    size_t bytes = recv(connfd, buffer, bufsize, 0);

    if (bytes > 0)
        result = std::string(buffer, 0, bytes);

    if (outbytes != nullptr)
        *outbytes = bytes;


    delete[] buffer;
    return result;
}

size_t WindowsSocket::Send(void* data, int len)
{
    return send(connfd, (char*)data, len, 0);
}

size_t WindowsSocket::Send(std::string data)
{
    return send(connfd, data.c_str(), data.size(), 0);
}

#endif /* WIN32 */