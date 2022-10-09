#ifndef FILECONTROLLER_h
#define FILECONTROLLERT_h

class FileController {
public:
	static FileController& GetInstance();
	void Write(std::string _fileName, std::string _message);

private:
	static FileController* fileController;
	std::map<std::string, std::ofstream> fileList;
	FileController();
	~FileController();

};
#define FILE_CONTROLLER FileController::GetInstance()
#endif