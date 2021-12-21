#include <iostream>
#include <thread>

#include "control_server.h"
#include "GPUStats.h"

void communication_handler(const std::string& cmd, std::string& response);

int main()
{
    // Initialize GPU Utility Manager
    GPUStats stats;
    
    if (!stats.init())
    {
        printf("Failed to initialize: %s\n", stats.get_last_error().c_str());
        system("pause");
        return -1;
    }

    if (!stats.get_device_count())
    {
        printf("---- No GPU devices found ----\n\n");
        system("pause");
        return 3;
    }

    for (size_t i = 0; i < stats.get_device_count(); ++i)
        printf("Device [%zi]: %s\n", i, stats.get_device(i).get_name().c_str());
    
    printf("\n");

    control_server server;

    while (true)
    {
        printf("Waiting for connections...\n");
        server.wait_for_connection();

        printf("[*] Control center connected [*]\n");
        server.start_communication_loop(communication_handler);

        printf("Connection closed!\n\n");
        server.close_connection();
    }

    return 0;
}

void communication_handler(const std::string& cmd, std::string& response)
{
    if (cmd._Equal("hello"))
        response = "Hello from MineGuard!";
}
