//COMP710 GP Framework 2025

#ifndef __LOGMANAGER_H__
#define __LOGMANAGER_H

#include <vector>
#include <string>

class LogManager {
	//Member methods:
public:
	static LogManager& GetInstance();
	static void DestroyInstance();
	void DebugDraw();

	void Log(const char* pcMessage);

protected:

private:
	LogManager();
	~LogManager();
	LogManager(const LogManager& logManager);
	LogManager& operator=(const LogManager& logManager);

	//Member data:

public:

protected:
	static LogManager* sm_pInstance;
	std::vector<std::string> m_logHistory;
	bool m_bShowLogMessages;

private:
};

#endif // __LOGMANAGER_H