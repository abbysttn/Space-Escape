#include "iniparser.h"
#include <fstream>


INIParser::INIParser() {}

INIParser::~INIParser() {}

bool INIParser::LoadIniFile(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open()) {
		return false;
	}

	m_data.clear();
	string currentSection;
	string line;

	while (getline(file, line)) {
		processLine(line, currentSection);
	}

	file.close();
	return true;
}

string INIParser::GetValueAsString(const string& iniSection, const string& key)
{
	string fullKey = makeKey(iniSection, key);

	if (m_data.count(fullKey) > 0) {
		return m_data.at(fullKey);
	}

	throw runtime_error("Key not found: " + fullKey);
}

int INIParser::GetValueAsInt(const string& iniSection, const string& key)
{
	string value = GetValueAsString(iniSection, key);
	try {
		return stoi(value);
	}
	catch (...) {
		throw runtime_error("Cannot convert to integer: " + value);
	}
}

float INIParser::GetValueAsFloat(const string& iniSection, const string& key)
{
	string value = GetValueAsString(iniSection, key);
	try {
		return stof(value);
	}
	catch (...) {
		throw runtime_error("Cannot convert to float: " + value);
	}
}

bool INIParser::GetValueAsBoolean(const string& iniSection, const string& key)
{
	string value = GetValueAsString(iniSection, key);
	string lowerValue;

	for (char ch : value) {
		lowerValue += tolower(ch);
	}

	if (lowerValue == "true" || lowerValue == "1") {
		return true;
	}

	if (lowerValue == "false" || lowerValue == "0") {
		return false;
	}

	throw runtime_error("Cannot convert to boolean: " + value);
}

void INIParser::processLine(string line, string& currentSelection)
{
	line = trim(line);

	if (line.empty() || line[0] == ';' || line[0] == '#') {
		return;
	}

	if (line[0] == '[' && line[line.length() - 1] == ']') {
		currentSelection = line.substr(1, line.length() - 2);
		return;
	}

	size_t equalsPos = line.find('=');
	if (equalsPos != string::npos) {
		string key = trim(line.substr(0, equalsPos));
		string value = trim(line.substr(equalsPos + 1));

		if (!key.empty()) {
			string fullKey = makeKey(currentSelection, key);
			m_data[fullKey] = value;
		}
	}
}

string INIParser::makeKey(const string& section, const string& name)
{
	return section + "|" + name;
}

string INIParser::trim(const string& str)
{
	size_t first = str.find_first_not_of(" \t");
	if (first == string::npos) {
		return "";
	}

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

vector<string> INIParser::split(const string& str, char delimiter)
{
	vector<string> tokens;

	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));

	return tokens;
}
