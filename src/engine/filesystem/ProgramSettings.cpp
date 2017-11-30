#include <map>
#include <vector>

#include "engine/common.h"

#include "ProgramSettings.h"

namespace tdrp
{
namespace settings
{

/////////////////////////////

ProgramSettingsCategory::ProgramSettingsCategory(const std::string& category)
: m_category_name(category)
{
}

ProgramSettingsCategory::~ProgramSettingsCategory()
{
}

bool ProgramSettingsCategory::Exists(const std::string& setting) const
{
	auto i = m_settings.find(setting);
	if (i == m_settings.end()) return false;
	return true;
}

std::string ProgramSettingsCategory::Get(const std::string& setting) const
{
	auto i = m_settings.find(setting);
	if (i == m_settings.end()) return std::string();
	return i->second;
}

int32_t ProgramSettingsCategory::GetInt(const std::string& setting) const
{
	auto i = m_settings.find(setting);
	if (i == m_settings.end()) return 0;

	std::istringstream str((char*)i->second.c_str());
	int32_t v;
	str >> v;
	return v;
}

float ProgramSettingsCategory::GetFloat(const std::string& setting) const
{
	auto i = m_settings.find(setting);
	if (i == m_settings.end()) return 0.0f;

	std::istringstream str((char*)i->second.c_str());
	float v;
	str >> v;
	return v;
}

void ProgramSettingsCategory::Set(const std::string& setting, const std::string& value)
{
	auto i = m_settings.find(setting);
	if (i == m_settings.end())
	{
		m_settings[setting] = value;
		m_setting_order.push_back(setting);
	}
	else i->second = value;
}

void ProgramSettingsCategory::Set(const std::string& setting, int value)
{
	std::stringstream str;
	str << value;
	Set(setting, str.str());
}

void ProgramSettingsCategory::Set(const std::string& setting, float value)
{
	std::stringstream str;
	str << value;
	Set(setting, str.str());
}

/////////////////////////////

ProgramSettings::ProgramSettings()
{
}

// ProgramSettings::ProgramSettings(rha::fs::file& f)
// {
// 	load_from_file(f);
// }

ProgramSettings::~ProgramSettings()
{
	for (auto i = m_categories.begin(); i != m_categories.end(); ++i)
		delete i->second;
	m_categories.clear();
}

/*
bool ProgramSettings::LoadFromFile(rha::fs::file& f)
{
	ProgramSettingsCategory* category = 0;
	std::string filedata;
	filedata.loadDataStream(&f.vector()[0], f.vector().size());
	
	uchar32_t delimeter[2] = { '\r', '\n' };
	core::list<std::string> lines;
	filedata.split<core::list<std::string> >(lines, delimeter, 2);

	for (auto i = lines.begin(); i != lines.end(); ++i)
	{
		// Read the line and trim it.
		std::string line = *i;
		line.trim();
		if (line.empty()) continue;

		// Check if this is a category or an item.
		if (line[0] == '[' && line.lastChar() == ']')
		{
			// Save the old category.
			if (category != nullptr)
			{
				categories[category->get_category_name()] = category;
				category_order.push_back(category);
			}

			// Create a new category.
			category = new ProgramSettingsCategory(line.subString(1, line.size() - 2));
		}
		else
		{
			if (category == nullptr) continue;

			// Parse our string.
			s32 pos_e = line.findFirst('=');
			std::string setting = line.subString(0, pos_e).trim();
			std::string value = line.subString(pos_e + 1, line.size_raw()).trim();

			// Save it to our current category.
			category->set(setting, value);
		}
	}

	// Save the category.
	if (category != nullptr)
	{
		categories[category->get_category_name()] = category;
		category_order.push_back(category);
	}

	return true;
}
*/

/*
bool ProgramSettings::WriteToFile(const io::path& filename, io::IFileSystem* FileSystem)
{
	if (FileSystem == 0)
		return false;

	// Assemble the file.
	std::string filedata;
	for (auto i = category_order.begin(); i != category_order.end(); ++i)
	{
		ProgramSettingsCategory* category = *i;
		if (category == nullptr) continue;

		filedata += '[';
		filedata += category->get_category_name();
		filedata += "]\r\n";
		
		auto settings_map = category->get_settings();
		auto settings_vector = category->get_settings_order();
		for (auto j = settings_vector.begin(); j != settings_vector.end(); ++j)
		{
			auto s = settings_map.find(*j);
			if (s == settings_map.end())
				continue;

			filedata += s->first;
			filedata += " = ";
			filedata += s->second;
			filedata += "\r\n";
		}

		filedata += "\r\n";
	}
	if (filedata.empty()) return true;

	// Save the file.
	core::array<uchar8_t> utf8 = filedata.toUTF8(true);
	rha::fs::file f((const char*)utf8.pointer(), (const char*)utf8.pointer() + utf8.size() - 1);
	f.set_filename(filename);
	f.save(FileSystem);
	return true;
}
*/

ProgramSettingsCategory* ProgramSettings::Get(const std::string& category)
{
	auto i = m_categories.find(category);
	if (i == m_categories.end()) return nullptr;
	return i->second;
}

ProgramSettingsCategory* ProgramSettings::Add(const std::string& category)
{
	auto i = m_categories.find(category);
	if (i != m_categories.end()) return i->second;

	ProgramSettingsCategory* c = new ProgramSettingsCategory(category);
	categories[category] = c;
	return c;
}

} // end namespace settings
} // end namespace tdrp
