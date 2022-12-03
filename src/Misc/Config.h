#pragma once

#include <sstream>
#include <string>
#include <map>

class ConfigSettingNotFoundException : public std::exception
{
public:
	ConfigSettingNotFoundException(const char* settingName)
		: std::exception((std::string("configuration setting not found: ") + settingName).c_str())
	{
	}
};

/**
 * @brief 
 * A general purpose configuration file reader.
 * 
 * Config settings must be in the form
 * # a comment
 * <setting_name> = <setting value>
 */
class Config
{
	/**
	 * @brief 
	 * Configuration settings by name.
	 * 
	 */
	std::map<std::string, std::string> m_settings;

	/**
	 * @brief
	 * Lines of special purpose data
	 */
	//std::map<std::string, std::vector<std::string>> m_data;

	std::vector<std::string> m_data;

public:
	Config();
	Config(const boost::filesystem::path& path);
	~Config();
	
	void LoadFromFile(const boost::filesystem::path& path);

	/**
	 * @brief
	 * Returns true if the named setting exists.
	 *
	 * @param name
	 */
	bool HasSetting(std::string name) { return m_settings.find(name) != m_settings.end(); } 

	/**
	 * @brief 
	 * Template function for retreiving a setting value.
	 * 
	 * @param name
	 * @exception ConfigSettingNotFoundException if the setting does not exist
	 * @return the setting value
	 */
	template <class T> T GetSetting(const char* name) const
	{
		if (m_settings.count(name) != 0)
		{
			std::istringstream s(m_settings.find(name)->second);
			T value;
			s >> value;
			return value;
		}
		else
			throw ConfigSettingNotFoundException(name);
	}

	/**
	 * @brief 
	 * Template function for retreiving a setting value, or a default value if it does not exist.
	 * 
	 * @param name
	 * @param default value
	 * @return the setting value
	 */
	template <class T> T GetSetting(const char* name, T defaultValue) const
	{
		if (m_settings.count(name) != 0)
		{
			std::istringstream s(m_settings.find(name)->second);
			T value;
			s >> value;
			return value;
		}
		else
			return defaultValue;
	}

	/**
	 * @brief 
	 * Template function for setting a setting.
	 * 
	 * @param name
	 * @param value
	 */
	template <class T> void SetSetting(const char* name, const T& value)
	{
		std::ostringstream s;
		s << value;
		m_settings[name] = s.str();
	}

	/**
	 * @brief 
	 * Template function to retreive the named string setting value.
	 * 
	 * @param name
	 * @exception ConfigSettingNotFoundException
	 * @return the setting value
	 */
	template<> std::string GetSetting<std::string>(const char* name) const
	{
		if (m_settings.count(name) != 0)
			return m_settings.find(name)->second;
		else
			throw ConfigSettingNotFoundException(name);
	}

	/**
	 * @brief 
	 * Template function to retreive the named string setting value, or default value if not found.
	 * 
	 * @param name
	 * @exception ConfigSettingNotFoundException
	 * @return the setting value
	 */
		template<> std::string GetSetting<std::string>(const char* name, std::string defaultValue) const
		{
		if (m_settings.count(name) != 0)
			return m_settings.find(name)->second;
		else
			return defaultValue;
		}

	/**
	 * @brief 
	 * Template function to retreive the named boolean setting value.
	 * 
	 * @param name
	 * @exception ConfigSettingNotFoundException
	 * @return the setting value
	 */
	template<> bool GetSetting<bool>(const char* name) const
	{
		if (m_settings.count(name) != 0)
		{
			std::istringstream s(m_settings.find(name)->second);
			bool value;
			s >> std::boolalpha >> value;
			return value;
		}
		else
			throw ConfigSettingNotFoundException(name);
	}

	/**
	 * @brief 
	 * Template function to retreive the named boolean setting value, or the default value if not found.
	 * 
	 * @param name
	 * @param defaultValue
	 * @return the setting value
	 */
	template<> bool GetSetting<bool>(const char* name, bool defaultValue) const
	{
		if (m_settings.count(name) != 0)
		{
			std::istringstream s(m_settings.find(name)->second);
			bool value;
			s >> std::boolalpha >> value;
			return value;
		}
		else
			return defaultValue;
	}

	/**
	 * @brief
	 * All raw lines of specialized data
	 */
	std::vector<std::string>& GetData()
	{
		return m_data;
	}
	__declspec(property(get=GetData)) std::vector<std::string>& Data;
};
