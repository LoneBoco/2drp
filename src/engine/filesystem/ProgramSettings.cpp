#include <algorithm>
#include <locale>
#include <sstream>

#include <boost/program_options.hpp>

#include "engine/common.h"
#include "engine/filesystem/File.h"

#include "ProgramSettings.h"

namespace tdrp::settings
{

/////////////////////////////

bool ProgramSettings::LoadFromFile(const filesystem::path& filename)
{
	tdrp::fs::File f{ filename };

	std::string category{ "Global" };

	while (!f.Finished())
	{
		std::string line = f.ReadLine();

		// Erase any carriage returns.
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

		// Trim.
		boost::trim(line);

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
				boost::trim_right(setting);
				boost::trim_left(value);

				// Save it.
				Set(category + "." + setting, value);
			}
		}
	}

	return true;
}

bool ProgramSettings::LoadFromCommandLine(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("package", po::value<std::string>()->implicit_value("login"), "game package")
		("host", po::value<uint16_t>()->implicit_value(13131), "host on port")
		("connect", po::value<std::vector<std::string>>()->multitoken(), "connect to server on port")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
	po::notify(vm);

	if (vm.count("package"))
	{
		Set("game.package", vm["package"].as<std::string>());
	}

	if (vm.count("host"))
	{
		Set("game.starthosting", true);
		Set("server.peers", 32);
		Set("network.port", vm["host"].as<uint16_t>());
	}

	if (vm.count("connect"))
	{
		auto connect_opts = vm["connect"].as<std::vector<std::string>>();
		if (!connect_opts.empty())
		{
			std::string server = connect_opts.at(0);
			uint16_t port = 13131;

			if (connect_opts.size() == 2)
			{
				std::istringstream ss(connect_opts.at(1));
				ss >> port;
			}

			Set("network.server", server);
			Set("network.port", port);
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
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	//std::transform(lower.begin(), lower.end(), lower.begin(), [](auto ch) { return std::tolower(ch, std::locale("")); });

	auto i = m_settings.find(lower);
	if (i == m_settings.end()) return false;
	return true;
}

std::string ProgramSettings::Get(const std::string& setting, const std::string& def) const
{
	return GetAs<std::string>(setting, def);
}

void ProgramSettings::Set(const std::string& setting, const std::string& value)
{
	std::string lower{ setting };
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

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

} // end namespace tdrp::settings
