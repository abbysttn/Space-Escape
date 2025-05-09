#ifndef _INIPARSER_H__
#define _INIPARSER_H__

#include <string>
#include <map>
#include <vector>

using namespace std;

class INIParser {
public:
	INIParser();
	~INIParser();

	bool LoadIniFile(const string& filename);
	string GetValueAsString(const string& iniSection, const string& key);
	int GetValueAsInt(const string& iniSection, const string& key);
	float GetValueAsFloat(const string& iniSection, const string& key);
	bool GetValueAsBoolean(const string& iniSection, const string& key);

protected:

private:
	INIParser(const INIParser& iniParser);
	INIParser& operator=(const INIParser& iniParser);

public:

protected:

private:
	map<string, string> m_data;
	void processLine(string line, string& currentSelection);
	string makeKey(const string& section, const string& name);
	string trim(const string& str);
	vector<string> split(const string& str, char delimiter);
};

#endif // !_INIPARSER_H__
