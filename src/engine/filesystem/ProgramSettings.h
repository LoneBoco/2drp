#pragma once

#include "engine/common.h"

namespace tdrp
{
namespace settings
{

class ProgramSettingsCategory
{
public:
	ProgramSettingsCategory(const std::string& category);
	~ProgramSettingsCategory();

	const std::string& GetCategoryName() const
	{
		return m_category_name;
	}

	std::map<std::string, std::string>& GetSettings()
	{
		return m_settings;
	}

	std::vector<std::string>& GetSettingsOrder()
	{
		return m_setting_order;
	}

	bool Exists(const std::string& setting) const;

	std::string Get(const std::string& setting) const;
	int32_t GetInt(const std::string& setting) const;
	float GetFloat(const std::string& setting) const;

	void Set(const std::string& setting, const std::string& value);
	void Set(const std::string& setting, int value);
	void Set(const std::string& setting, float value);

private:
	std::string m_category_name;
	std::map<std::string, std::string> m_settings;
	std::vector<std::string> m_setting_order;
};

class ProgramSettings
{
public:
	ProgramSettings();
	// ProgramSettings(rha::fs::file& f);
	~ProgramSettings();

	// bool LoadFromFile(rha::fs::file& f);
	// bool WriteToFile(const io::path& filename, io::IFileSystem* FileSystem);
	std::shared_ptr<ProgramSettingsCategory> Get(const std::string& category);
	std::shared_ptr<const ProgramSettingsCategory> Get(const std::string& category) const;
	std::weak_ptr<ProgramSettingsCategory> Add(const std::string& category);

private:
	std::map<std::string, std::shared_ptr<ProgramSettingsCategory>> m_categories;
	std::vector<std::shared_ptr<ProgramSettingsCategory>> m_category_order;
};

} // end namespace settings
} // end namespace tdrp
