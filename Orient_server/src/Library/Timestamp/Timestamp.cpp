#ifdef _MSC_VER
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "Timestamp.h"

using namespace std;

Timestamp* Timestamp::timestamp = nullptr;

Timestamp& Timestamp::GetInstance()
{
	if (timestamp == nullptr) timestamp = new Timestamp();
	return *timestamp;
}

std::string Timestamp::GetNowTime()
{
	time_t t = time(nullptr);
	std::stringstream date;

#ifdef _MSC_VER
	tm now;
	errno_t error = localtime_s(&now, &t);

	SYSTEMTIME st;
	GetLocalTime(&st);

	date << now.tm_year + 1900;
	date << setw(2) << setfill('0') << now.tm_mon + 1;
	date << setw(2) << setfill('0') << now.tm_mday;
	date << setw(2) << setfill('0') << now.tm_hour;
	date << setw(2) << setfill('0') << now.tm_min;
	date << setw(2) << setfill('0') << now.tm_sec;
	date << to_string(st.wMilliseconds);


#else
	tm* now;
	now = localtime(&t);

	timespec ts;
	int ret;
	ret = clock_gettime(CLOCK_REALTIME, &ts);
	if (ret < 0) {
		return "Error:Can't get the time";
	}
	date << now->tm_year + 1900;
	date << setw(2) << setfill('0') << now->tm_mon + 1;
	date << setw(2) << setfill('0') << now->tm_mday;
	date << setw(2) << setfill('0') << now->tm_hour;
	date << setw(2) << setfill('0') << now->tm_min;
	date << setw(2) << setfill('0') << now->tm_sec;
	date << to_string(ts.tv_nsec / 1000000);

#endif
	return date.str();

}

Timestamp::Timestamp()
{
}

Timestamp::~Timestamp()
{
	delete timestamp;
}
