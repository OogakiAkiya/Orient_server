#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "FileController.h"

using namespace std;

FileController* FileController::fileController = nullptr;


FileController::FileController()
{
}

FileController::~FileController()
{
	for (auto& file : fileList) {
		file.second.close();
	}

	delete fileController;
}

FileController& FileController::GetInstance()
{
	if (fileController == nullptr) fileController = new FileController();
	return *fileController;
}

void FileController::Write(std::string _fileName, std::string _message)
{
	auto itr = fileList.find(_fileName);
	if (itr == fileList.end()) {
		fileList.insert(std::make_pair(_fileName, std::ofstream(_fileName,ios::app)));
		return;
	}
	
	fileList[_fileName] << _message << endl;
}
