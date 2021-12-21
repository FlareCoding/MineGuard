#include "GPUStats.h"
#include <nvml.h>

#pragma warning(push)
#pragma warning(disable: 26812)

bool GPUStats::init()
{
    // Initialize NVML environment
	nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS)
    {
        m_LastResultError = nvmlErrorString(result);
        return false;
    }

    // Get device count
    unsigned int deviceCount = 0;
    result = nvmlDeviceGetCount(&deviceCount);
    if (result != NVML_SUCCESS)
    {
        m_LastResultError = nvmlErrorString(result);
        return false;
    }

    // Parse devices
    for (unsigned int deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
    {
        GPU device = GPU(deviceIndex);
        if (!device.is_valid())
        {
            m_LastResultError = nvmlErrorString(result);
            return false;
        }

        m_Devices.push_back(device);
    }

	return true;
}

bool GPUStats::shutdown()
{
    // Shutdown NVML environment
    nvmlReturn_t result = nvmlShutdown();
    if (result != NVML_SUCCESS)
    {
        m_LastResultError = nvmlErrorString(result);
        return false;
    }

	return true;
}

#pragma warning(pop)
