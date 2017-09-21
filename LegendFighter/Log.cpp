#include "pch.h"
#include "Log.h"
#include <time.h>
#include <iostream>
using namespace std;

Log::LogLevel Log::messageLevel = Log::LogLevel::INFO;
wofstream Log::of = wofstream();

Log::Log()
{
}

Log::~Log()
{
	if (messageLevel >= Log::ReportingLevel())
	{
		wclog << os.str() << endl;

#ifdef _DEBUG
		OutputDebugString(os.str().c_str());
#endif
	}
}

std::wostringstream& Log::Get(LogLevel level)
{
	os << L"- " << NowTime();

	os << L" " << level << L": ";

	os << GetCurrentThreadId() << L'\t';

	messageLevel = level;

	return os;
}

Log::LogLevel& Log::ReportingLevel()
{
	return messageLevel;
}

void Log::Rebuf(std::wstring name)
{
	of.open(name, ios::ate);
	
	wclog.rdbuf(of.rdbuf());
}

wchar_t *  Log::NowTime()
{
	time_t t;
	tm local;

	t = time(NULL);
	localtime_s(&local, &t);

	static wchar_t szBuffer[64];

	//memset(szBuffer, 0, sizeof(szBuffer));
	wcsftime(szBuffer, __crt_countof(szBuffer), L"%x %X %p", &local);

	return szBuffer;
}