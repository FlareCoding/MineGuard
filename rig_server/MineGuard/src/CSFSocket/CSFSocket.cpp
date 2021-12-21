//
//  CSFSocket.cpp
//  CrossplatformSocketFramework
//
//  Created by Albert Slepak on 7/14/19.
//  Copyright © 2019 Albert Slepak. All rights reserved.
//

#include "CSFSocket.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#pragma comment (lib, "Ws2_32.lib")
#include "WindowsSocket.h"
std::shared_ptr<CSFSocket> CreateSocket()
{
    return std::make_shared<WindowsSocket>();
}
#else
#include "LinuxSocket.h"
std::shared_ptr<CSFSocket> CreateSocket()
{
    return std::make_shared<LinuxSocket>();
}
#endif
