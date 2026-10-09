#include "stdAfx.h"
#include <direct.h>
#include <algorithm>
#include "Configger.h"

namespace Chaos
{

Configger::Configger(const std::string& fileName)
{   
	LoadConfig();

	char path[1024];
	::getcwd(path, 1024);
	_fullPath = path;
	std::replace(_fullPath.begin(), _fullPath.end(), '\\', '/');
	_fullPath += '/';
	_fullPath += fileName;
}

Configger::~Configger()
{
	SaveConfig();
}

int
Configger::GetInt(const std::string& section, const std::string& key) const
{
	return GetIntWithDefault(section, key, 0);
}

int
Configger::GetIntWithDefault(const std::string& section, 
							 const std::string& key, 
							 const int& defValue) const
{
	return ::GetPrivateProfileInt(section.c_str(), key.c_str(), defValue, _fullPath.c_str());
}


bool
Configger::SetInt(const std::string& section, 
				  const std::string& key, 
				  const int& value) const
{
	std::ostringstream trans;
	
	if (trans << value)
	{
		return SetString(section, key, trans.str());
	}
	else
	{
//		assert(false);
		return false;
	}                
}

std::string
Configger::GetString(const std::string& section, const std::string& key) const
{
	return GetStringWithDefault(section, key, std::string());
}

std::string
Configger::GetStringWithDefault(const std::string& section, 
								const std::string& key, 
								const std::string& defValue) const
{
	
	char result[1024];
	
	if (0 != ::GetPrivateProfileString(section.c_str(), 
								  key.c_str(), 
								  defValue.c_str(), 
								  result,
								  sizeof(result),								  
								  _fullPath.c_str()))
	{
		return result;
	}

	return defValue;
}

bool 
Configger::SetString(const std::string& section, 
					 const std::string& key, 
					 const std::string& value) const
{
	int ret = ::WritePrivateProfileString(section.c_str(),
					    						 key.c_str(), 
											   value.c_str(), 
											_fullPath.c_str());
	return ret != 0;
}

} // Chaos namespace.