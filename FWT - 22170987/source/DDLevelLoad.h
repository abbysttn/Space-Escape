#ifndef _DDLEVELLOAD_H__
#define _DDLEVELLOAD_H__

#include <string>
#include <vector>

using namespace std;

class DDLevelLoad {
public:
	DDLevelLoad();
	~DDLevelLoad();

	bool LoadLevelFile(const string& filename);
	const vector<string>& GetLevelData() const;
	size_t GetWidth() const;
	size_t GetHeight() const;
	char GetTileAt(size_t x, size_t y) const;

protected:

private:
	DDLevelLoad(const DDLevelLoad& ddLevelLoad);
	DDLevelLoad& operator=(const DDLevelLoad& ddLevelLoad);

public:

protected:

private:
	vector<string> m_data;
	size_t m_width;
	size_t m_height;
	void processLine(string& line);
	string trim(string& str) const;
};

#endif // !_DDLEVELLOAD_H__
