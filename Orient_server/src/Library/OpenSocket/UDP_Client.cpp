#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"base/BaseClient.h"
#include"UDP_Routine.h"
#include"UDP_Client.h"

void UDP_Client::Update()
{
	m_routine->Update(m_socket, recvDataQueList);
}

std::shared_ptr<BaseClient> UDP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<UDP_Client> temp = std::make_shared<UDP_Client>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<UDP_Routine>();

	temp->m_socket->Init(_addrs, _port);						//IPアドレスとポート番号の設定
	SwitchIpv(temp->m_socket, _ipv);							//IPvの設定
	temp->m_socket->SetProtocol_UDP();							//TCP通信に設定
	if (!temp->m_socket->AddressSet())return nullptr;			//ソケット生成
	if (_asynchronous)temp->m_socket->SetAsynchronous();
	return temp;
}

std::vector<char> UDP_Client::GetRecvData()
{
	std::pair<B_ADDRESS_IN, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData.second;
}

int UDP_Client::SendServer(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加し送信
		memcpy(&sendBuf[0], &sequence, sizeof(unsigned int));
		memcpy(&sendBuf[sizeof(unsigned int)], &_buf[0], _bufSize);

		//送信処理
		sendDataSize = m_socket->Sendto(&sendBuf[0], _bufSize + sizeof(unsigned int));

		//シーケンス番号管理
		sequence++;
		if (sequence > SEQUENCEMAX) { sequence = 0; }
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
		return sendDataSize;
	}
	return sendDataSize;
}

