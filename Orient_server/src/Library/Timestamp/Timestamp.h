#ifndef TIMESTAMP_h
#define TIMESTAMP_h

class Timestamp {
public:
	static Timestamp& GetInstance();
	std::string GetNowTime();

private:
	static Timestamp* timestamp;
	Timestamp();
	~Timestamp();

};
#define TIMESTAMP Timestamp::GetInstance()
#endif