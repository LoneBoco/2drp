#include <algorithm>
#include <locale>
#include <sstream>

#include "engine/common.h"

#include "ProgramSettings.h"

namespace tdrp
{
namespace settings
{

/////////////////////////////

bool ProgramSettings::LoadFromFile(const tdrp::fs::File& f)
{
	std::string category{ "Global" };

	while (!f.Finished())
	{
		std::string line = f.ReadLine();

		// Erase any carriage returns.
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

		// Trim.
		trim(line);

		if (!line.empty())
		{
			// Check if this is a category.
			if (line[0] == '[' && *(line.rbegin()) == ']')
			{
				// Register the new category.
				category = line.substr(1, line.size() - 2);
			}
			else
			{
				// Parse our string.
				std::size_t sep = line.find('=');
				std::string setting = line.substr(0, sep);
				std::string value;
				if (sep != std::string::npos)
					value = line.substr(sep + 1, line.size() - sep - 1);
				rtrim(setting);
				ltrim(value);

				// Save it.
				Set(category + "." + setting, value);
			}
		}
	}

	return true;
}

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

bool ProgramSettings::Exists(const std::string& setting) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return false;
	return true;
}

std::string ProgramSettings::Get(const std::string& setting, const std::string& def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;
	return i->second;
}

int32_t ProgramSettings::GetInt(const std::string& setting, const int32_t def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;

	std::istringstream str(i->second);
	int32_t v;
	str >> v;
	return v;
}

float ProgramSettings::GetFloat(const std::string& setting, const float def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;

	std::istringstream str(i->second);
	float v;
	str >> v;
	return v;
}

bool ProgramSettings::GetBool(const std::string& setting, const bool def) const
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return def;

	std::string lowervalue{ i->second };
	std::transform(lowervalue.begin(), lowervalue.end(), lowervalue.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	if (lower == "on")
		return true;
	if (lower == "true")
		return true;
	if (lower == "yes")
		return true;

	return false;
}

void ProgramSettings::Set(const std::string& setting, const std::string& value)
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end())
	{
		m_settings[lower] = value;
		m_settings_order.push_back(setting);
	}
	else i->second = value;
}

void ProgramSettings::Set(const std::string& setting, const int32_t value)
{
	std::stringstream str;
	str << value;
	Set(setting, str.str());
}

void ProgramSettings::Set(const std::string& setting, const float value)
{
	std::stringstream str;
	str << value;
	Set(setting, str.str());
}

void ProgramSettings::Set(const std::string& setting, const bool value)
{
	Set(setting, value ? std::string("true") : std::string("false"));
}

} // end namespace settings
} // end namespace tdrp
