#pragma once
#include <optional>
#include <string>
#include "Computer.h"
#include "utils.h"

namespace
	{

	inline std::string load_na() { return "N/A %"; }
	inline std::string load(float value)
		{
		float tmp = round(value);
		if (tmp > 100 || tmp < 0) { return load_na(); }
		return utils::to_string((int)tmp, 3) + "%";
		}
	inline std::string load(std::optional<float> value) { if (value.has_value()) { return load(value.value_or(0)); } return load_na(); ; }
	
	inline std::string temperature_na() { return "N/A °C"; }
	inline std::string temperature(float value)
		{
		float tmp = round(value);
		if (tmp > 999 || tmp < -999) { return temperature_na(); }
		return utils::to_string((int)tmp, 3) + "°C";
		}
	inline std::string temperature(std::optional<float> value) { if (value.has_value()) { return temperature(value.value_or(0)); } return temperature_na(); }
	inline std::string power_na() { return "N/A W"; }
	inline std::string power(float value)
		{
		float tmp = round(value);
		return utils::to_string((int)tmp, 3) + "W";
		}
	inline std::string power(std::optional<float> value) { if (value.has_value()) { return power(value.value_or(0)); } return power_na(); }

	inline std::string fan_na() { return " N/A RPM"; }
	inline std::string fan(float value)
		{
		float tmp = round(value);
		if (tmp > 9999 || tmp < 0) { return fan_na(); }
		return utils::to_string((int)tmp, 4) + "RPM";
		}
	inline std::string fan(std::optional<float> value) { if (value.has_value()) { return fan(value.value_or(0)); } return fan_na(); }

	std::string dataTB(std::optional<float> value);
	std::string dataGB(std::optional<float> value);
	std::string dataMB(std::optional<float> value);
	std::string dataB (std::optional<float> value);

	//library's ???
	inline std::string dataTB_na() { return " N/A TB"; }
	inline std::string dataTB(float value)
		{
		if (value >= 1024) { return dataTB_na(); ; }
		if (value < 1) { return dataGB(value * 1024); }
		return utils::to_string(value, 4, 1) + "TB";
		}
	inline std::string dataTB(std::optional<float> value) { if (value.has_value()) { return dataTB(value.value_or(0)); } return dataTB_na(); }

	//library's data
	inline std::string dataGB_na() { return " N/A GB"; }
	inline std::string dataGB(float value)
		{
		if (value >= 1024) { return dataTB(value / 1024); }
		if (value < 1) { return dataMB(value * 1024); }
		return utils::to_string(value, 4, 1) + "GB";
		}
	inline std::string dataGB(std::optional<float> value) { if (value.has_value()) { return dataGB(value.value_or(0)); } return dataGB_na(); }

	//library's smalldata
	inline std::string dataMB_na() { return " N/A MB"; }
	inline std::string dataMB(float value)
		{
		if (value >= 1024) { return dataGB(value / 1024); }
		if (value < 1) { return dataB(value * 1024); }
		return utils::to_string(value, 4, 1) + "MB";
		}
	inline std::string dataMB(std::optional<float> value) { if (value.has_value()) { return dataMB(value.value_or(0)); } return dataMB_na(); }

	//library's ???
	inline std::string dataB_na() { return " N/A B "; }
	inline std::string dataB(float value)
		{
		if (value >= 1024) { return dataMB(value / 1024); }
		if (value < 1) { return dataB_na(); }
		return utils::to_string(value, 4, 1) + "B ";
		}
	inline std::string dataB(std::optional<float> value) { if (value.has_value()) { return dataB(value.value_or(0)); } return dataTB_na(); }
	}

std::string value_string(sensor_type type, std::optional<float> value)
	{
	switch (type)
		{
			case sensor_type::data: return dataGB(value);
			case sensor_type::smalldata: return dataMB(value);
			case sensor_type::fan: return fan(value);
			case sensor_type::level:
			case sensor_type::control:
			case sensor_type::load: return load(value);
			case sensor_type::temperature: return temperature(value);
			case sensor_type::power: return power(value);
			case sensor_type::unknown: return "N/A";

			default: return "TODO";
		}
	}

std::string value_string(Sensor& sensor) { return value_string(sensor.type(), sensor.value()); }