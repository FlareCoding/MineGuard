#pragma once
#include <string>

// Forward declarations
typedef struct nvmlDevice_st* nvmlDevice_t;

class GPU
{
public:
	GPU(unsigned int deviceIndex);
	~GPU() = default;
	inline unsigned int get_device_index() const { return m_DeviceIndex; }

	static unsigned int invalid_value;

	inline const bool is_valid() const { return !m_FailedToInitialize; }
	inline std::string get_name() const { return m_DeviceName; }

	unsigned int get_temperature();
	unsigned int get_fan_speed();
	unsigned int get_power_usage();

private:
	unsigned int m_DeviceIndex = 0;
	nvmlDevice_t m_Device = nullptr;
	std::string  m_DeviceName;

	bool m_FailedToInitialize = false;
	std::string m_LastResultError = "Success";
};
