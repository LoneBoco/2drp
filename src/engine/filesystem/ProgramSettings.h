#pragma once

#include "engine/common.h"
#include "engine/filesystem/FileSystem.h"

namespace tdrp::settings
{

class ProgramSettings
{
public:
	ProgramSettings() = default;
	~ProgramSettings() = default;

	ProgramSettings(const ProgramSettings& other) = delete;
	ProgramSettings(ProgramSettings&& other) = delete;
	ProgramSettings& operator=(const ProgramSettings& other) = delete;
	ProgramSettings& operator=(ProgramSettings&& other) = delete;

	bool LoadFromFile(const filesystem::path& filename);
	bool LoadFromCommandLine(int argc, char** argv);
	// bool WriteToFile(const io::path& filename, io::IFileSystem* FileSystem);

	bool Exists(const std::string& setting) const;

	std::string Get(const std::string& setting, const std::string& def = "") const;
	template <typename T> T GetAs(const std::string& setting, const T def = T()) const;
	template <> bool GetAs<bool>(const std::string& setting, const bool def) const;

	void Set(const std::string& setting, const std::string& value);
	void Set(const std::string& setting, const int32_t value);
	void Set(const std::string& setting, const float value);
	void Set(const std::string& setting, const bool value);

private:
	std::map<std::string, std::string> m_settings;
	std::vector<std::string> m_settings_order;
};


template <typename T>
T ProgramSettings::GetAs(const std::string& setting, const T def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;

	std::istringstream str(i->second);
	T v;
	str >> v;
	return v;
}

template <>
inline
bool ProgramSettings::GetAs<bool>(const std::string& setting, const bool def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;

	std::string lowervalue{ i->second };
	std::transform(lowervalue.begin(), lowervalue.end(), lowervalue.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	if (lowervalue == "on")
		return true;
	if (lowervalue == "true")
		return true;
	if (lowervalue == "yes")
		return true;

	return false;
}

} // end namespace tdrp::settings
