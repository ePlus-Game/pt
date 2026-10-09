#ifndef CONFIGGER_H
#define CONFIGGER_H

#include <string>
#include <sstream>

namespace Chaos
{

class Configger
{
public:
	explicit Configger(const std::string& fileName);
	~Configger();
    
	int GetInt(const std::string& section, 
					   const std::string& key) const;
	int GetIntWithDefault(const std::string& section, 
								  const std::string& key, 
								  const int& defValue) const;
    
	std::string GetString(const std::string& section,
								  const std::string& key) const;
	std::string GetStringWithDefault(const std::string& section, 
											 const std::string& key, 
											 const std::string& defValue) const;

	bool SetInt(const std::string& section, 
						const std::string& key, 
						const int& value) const;
  	bool SetString(const std::string& section, 
						   const std::string& key, 
						   const std::string& value) const;

protected:                        
	bool LoadConfig() { return true; }
	bool SaveConfig() { return true; }
	
	std::string	_fullPath;
private:
	Configger(const Configger&);
	const Configger& operator=(const Configger&);
};

} // Chaos namespace

#endif // CONFIGGER_H

