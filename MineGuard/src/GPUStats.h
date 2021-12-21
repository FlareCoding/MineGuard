#pragma once
#include <vector>
#include "GPU.h"

class GPUStats
{
public:
	GPUStats() = default;
	~GPUStats() = default;

	bool init();
	bool shutdown();

	inline const std::string get_last_error() const { return m_LastResultError; }

	inline size_t get_device_count() const { return m_Devices.size(); }
	inline GPU& get_device(size_t idx) { return m_Devices[idx]; }

private:
	std::string m_LastResultError = "Success";

	std::vector<GPU> m_Devices;
};
