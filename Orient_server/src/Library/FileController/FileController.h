#ifndef FILECONTROLLER_h
#define FILECONTROLLERT_h

class FileContrller {
public:
	static FileContrller* GetInstance();
	static void Write(std::string _fileName, std::string _message);

private:
	static FileContrller* fileController;
	FileContrller();
	~FileContrller();

};

#endif