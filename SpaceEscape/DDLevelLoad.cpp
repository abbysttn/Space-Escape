#include "DDLevelLoad.h"

#include <fstream>

DDLevelLoad::DDLevelLoad() : m_width(0), m_height(0) {}

DDLevelLoad::~DDLevelLoad()
{
}

bool DDLevelLoad::LoadLevelFile(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open()) {
		return false;
	}

	m_data.clear();
	m_width = 0;
	m_height = 0;

	string line;

	while (getline(file, line)) {
		processLine(line);
	}

	file.close();

	m_height = m_data.size();

	for (const auto& row : m_data) {
		if (row.length() > m_width) {
			m_width = row.length();
		}
	}


	return true;
}

const vector<string>& DDLevelLoad::GetLevelData() const
{
	return m_data;
}

size_t DDLevelLoad::GetWidth() const
{
	return m_width;
}

size_t DDLevelLoad::GetHeight() const
{
	return m_height;
}

char DDLevelLoad::GetTileAt(size_t x, size_t y) const
{
	if (y >= m_data.size()) {
		return ' ';
	}

	if (x >= m_data[y].size()) {
		return ' ';
	}

	return m_data[y][x];
}

void DDLevelLoad::processLine(string& line)
{
	string trimmed = trim(line);

	if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#') {
		return;
	}

	m_data.push_back(trimmed);
}

string DDLevelLoad::trim(string& str) const
{
	size_t first = str.find_first_not_of(" \t");
	if (first == string::npos) {
		return "";
	}

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}
