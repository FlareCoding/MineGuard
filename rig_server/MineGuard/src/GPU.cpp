#include "GPU.h"
#include <nvml.h>

#pragma warning(push)
#pragma warning(disable: 26812)

unsigned int GPU::invalid_value = -1;

GPU::GPU(unsigned int deviceIndex)
	: m_DeviceIndex(deviceIndex)
{
    nvmlReturn_t result = nvmlDeviceGetHandleByIndex(0, &m_Device);
    if (NVML_SUCCESS != result)
    {
        m_LastResultError = nvmlErrorString(result);
        m_FailedToInitialize = true;
        return;
    }

    m_DeviceName.resize(NVML_DEVICE_NAME_V2_BUFFER_SIZE);
    result = nvmlDeviceGetName(m_Device, &m_DeviceName[0], NVML_DEVICE_NAME_V2_BUFFER_SIZE);
    if (NVML_SUCCESS != result)
    {
        m_LastResultError = nvmlErrorString(result);
        m_FailedToInitialize = true;
        return;
    }
}

unsigned int GPU::get_temperature()
{
    unsigned int temperature = 0;
    nvmlReturn_t result = nvmlDeviceGetTemperature(m_Device, NVML_TEMPERATURE_GPU, &temperature);
    if (NVML_SUCCESS != result)
    {
        m_LastResultError = nvmlErrorString(result);
        return GPU::invalid_value;
    }

    return temperature;
}

unsigned int GPU::get_fan_speed()
{
    unsigned int fanSpeed = 0;
    nvmlReturn_t result = nvmlDeviceGetFanSpeed(m_Device, &fanSpeed);
    if (NVML_SUCCESS != result)
    {
        m_LastResultError = nvmlErrorString(result);
        return GPU::invalid_value;
    }

    return fanSpeed;
}

unsigned int GPU::get_power_usage()
{
    unsigned int powerUsage = 0;
    nvmlReturn_t result = nvmlDeviceGetPowerUsage(m_Device, &powerUsage);
    if (NVML_SUCCESS != result)
    {
        m_LastResultError = nvmlErrorString(result);
        return GPU::invalid_value;
    }

    return powerUsage / 1000;
}

#pragma warning(pop)
