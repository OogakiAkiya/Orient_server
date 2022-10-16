// Orient_server.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "src/Library/Timestamp/Timestamp.h"
#include "src/Library/FileController/FileController.h"
#include "src/Library/OpenSocket/OpenSocket.h"

using namespace std;

void TcpUpdate(const shared_ptr<TCP_Server> _server) {
	_server->Update();

	//受信データ処理
	while (_server->GetRecvDataSize() > 0) {
		auto recvData = _server->GetRecvData();
		int data = *(int*)&recvData.second[sizeof(char) + sizeof(char) + sizeof(int)];
		std::cout << "【TCP】DataRecv:size" << recvData.second.size() << ",data:" << data << std::endl;

		int sendDataSize = _server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
	}

}
void UdpUpdate(const shared_ptr<UDP_Server> _server) {
	_server->Update();

	//受信データ処理
	while (_server->GetRecvDataSize() > 0) {
		auto recvData = _server->GetRecvData();
		unsigned int sequence;
		std::memcpy(&sequence, &recvData.second[0], sizeof(unsigned int));
		std::cout << "【UDP】DataRecv:sequence=" << sequence << std::endl;
		
		int sendDataSize = _server->SendOnlyClient(&recvData.first, &recvData.second[sizeof(unsigned int)], recvData.second.size() - sizeof(unsigned int));
		std::cout << "【UDP】destport=" << recvData.first.sin_port << "DataSend:size=" << sendDataSize << std::endl;
	}
}


int main()
{	/*
	int count = 0;
	while (true) {
		string msg = "[" + TIMESTAMP.GetNowTime() + "]" + "test" + to_string(count++);
		FILE_CONTROLLER.Write("error.log", msg);
		if (count > 100000) break;
	}
	*/

	
	auto tcpServer = TCP_Server::GetInstance("0.0.0.0", "17600", IPV4, true);
	auto udpServer = UDP_Server::GetInstance("0.0.0.0", "17700", IPV4, true);
	while (1) {
		TcpUpdate(tcpServer);
		UdpUpdate(udpServer);
	}
	
}
