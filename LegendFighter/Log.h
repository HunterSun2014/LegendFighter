#pragma once

class Log
{
public:

	Log();

	virtual ~Log();

	enum LogLevel {
		ERROR1,
		WARNING,
		INFO,
		DEBUG,
		DEBUG1,
		DEBUG2,
		DEBUG3,
		DEBUG4
	};
	
	std::wostringstream& Get(LogLevel level = INFO);

public:
	static LogLevel& ReportingLevel();
	
	/// <summary>
	///	Refuf
	/// </summary>
	static void Rebuf(std::wstring name);

protected:
	std::wostringstream os;
	
private:

	Log(const Log&);

	Log& operator =(const Log&) {};

	wchar_t * NowTime();
private:

	static LogLevel messageLevel;
	static std::wofstream	of;
};

