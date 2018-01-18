#pragma once

#include "engine/common.h"

namespace tdrp
{
namespace settings
{

class ProgramSettings
{
public:
	ProgramSettings() = default;
	~ProgramSettings() = default;

	bool LoadFromFile(const filesystem::path& filename);
	// bool WriteToFile(const io::path& filename, io::IFileSystem* FileSystem);

	bool Exists(const std::string& setting) const;

	std::string Get(const std::string& setting, const std::string& def = "") const;
	int32_t GetInt(const std::string& setting, const int32_t def = 0) const;
	float GetFloat(const std::string& setting, const float def = 0.0f) const;
	bool GetBool(const std::string& setting, const bool def = false) const;

	void Set(const std::string& setting, const std::string& value);
	void Set(const std::string& setting, const int32_t value);
	void Set(const std::string& setting, const float value);
	void Set(const std::string& setting, const bool value);

private:
	std::map<std::string, std::string> m_settings;
	std::vector<std::string> m_settings_order;
};

} // end namespace settings
} // end namespace tdrp
