#include "../StdAfx.h"

#include "Config.h"

using namespace boost;
using namespace boost::filesystem;

/**
* Constructor.
*/
Config::Config()
{
}

/**
 * @brief 
 * Constructor.
 * 
 * @param path
 */
Config::Config(const path& path)
{
	LoadFromFile(path);
}

/**
 * @brief 
 * Destructor.
 * 
 */
Config::~Config()
{
}

/**
 * @brief 
 * Loads settings from a file.
 * 
 * @param path
 */
void Config::LoadFromFile(const path& path)
{
	debug << "Config: loading from " << path.file_string() << endl;

	boost::filesystem::ifstream file;
	file.open(path);

	if (!file.is_open())
		throw std::exception((String("Unable to open config file ") + path.file_string()).c_str());

	bool readingData = false;

	while(!file.eof())
	{
		std::string buf;
		getline(file, buf);
		std::istringstream stream(buf);

		// skip white space
		stream >> std::ws;
		if (stream.eof())
			continue;

		// comment?
		if (stream.peek() == '#')
			continue;

		std::string line;
		getline(stream, line);

		// reading raw data?
		if (readingData)
		{
			m_data.push_back(line);
			continue;
		}
	
		//// read setting name
		//string settingName;
		//getline(line, settingName, '=');
		//trim(settingName);

		//// read value as string
		//string settingValue;
		//getline(line, settingValue);
		//trim(settingValue);

		// split line into setting name and value
		size_t pos = line.find_first_of(":=");
		std::string settingName = line.substr(0, pos);
		trim(settingName);
		String settingValue = line.substr(pos+1, line.length() - pos);
		trim(settingValue);

		// start or end of raw data?
		if (settingName == "data")
		{
			if (settingValue == "end")
				readingData = false;
			else
				readingData = true;
			continue;
		}

		// store value
		m_settings[settingName] = settingValue;
		debug << settingName << " = " << settingValue << endl;
	}

	file.close();
}

