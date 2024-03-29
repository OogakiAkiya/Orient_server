﻿// Orient_server.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#define NOMINMAX

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <cassert>
#include <algorithm>
#include <functional>
#include "src/Library/Timestamp/Timestamp.h"
#include "src/Library/FileController/FileController.h"
#include "src/Library/OpenSocket/OpenSocket.h"
#ifdef _MSC_VER
#else
#include <boost/version.hpp>
#endif


using namespace std;

void TcpUpdate(const shared_ptr<TCP_Server> _server) {
	_server->Update();
	
	//受信データ処理
	while (_server->GetRecvDataSize() > 0) {
		std::pair<int, std::vector<char>> recvData = _server->GetRecvData();
		int data = *(int*)&recvData.second[sizeof(char) + sizeof(char) + sizeof(int)];
		std::cout << "【TCP】DataRecv:size" << recvData.second.size() << ",data:" << data << std::endl;

		int sendDataSize = _server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
	}

}
void UdpUpdate(const shared_ptr<UDP_Server> _server) {
	_server->Update();

	//受信データ処理
	while (_server->GetRecvDataSize() > 0) {
		std::pair<B_ADDRESS_IN, std::vector<char>> recvData = _server->GetRecvData();
		unsigned int sequence;
		std::memcpy(&sequence, &recvData.second[0], sizeof(unsigned int));
		std::cout << "【UDP】DataRecv:sequence=" << sequence << std::endl;
		
		int sendDataSize = _server->SendOnlyClient(&recvData.first, &recvData.second[sizeof(unsigned int)], recvData.second.size() - sizeof(unsigned int));
		std::cout << "【UDP】destport=" << recvData.first.sin_port << "DataSend:size=" << sendDataSize << std::endl;
	}
}

int main()
{	
#ifdef _MSC_VER
#else

	cout << BOOST_VERSION << endl;
#endif
	/*
	int count = 0;
	while (true) {
		string msg = "[" + TIMESTAMP.GetNowTime() + "]" + "test" + to_string(count++);
		FILE_CONTROLLER.Write("error.log", msg);
		if (count > 100000) break;
	}
	*/
	fd_set readfds;
	auto tcpServer = TCP_Server::GetInstance("0.0.0.0", "17600", IPV4, false);
	auto udpServer = UDP_Server::GetInstance("0.0.0.0", "17700", IPV4, false);

	while (1) {
		FD_ZERO(&readfds);
		int maxfds = std::max(tcpServer->GetFileDescriptor(&readfds),udpServer->GetFileDescriptor(&readfds));
		//ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
		OpenSocket_Select(&readfds,maxfds);

		tcpServer->SetFileDescriptorPointer(&readfds);
		udpServer->SetFileDescriptorPointer(&readfds);
		TcpUpdate(tcpServer);
		UdpUpdate(udpServer);
	}
	
	
}
