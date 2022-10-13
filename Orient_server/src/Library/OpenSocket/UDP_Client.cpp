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

	temp->m_socket->Init(_addrs, _port);						//IP�A�h���X�ƃ|�[�g�ԍ��̐ݒ�
	SwitchIpv(temp->m_socket, _ipv);							//IPv�̐ݒ�
	temp->m_socket->SetProtocol_UDP();							//TCP�ʐM�ɐݒ�
	if (!temp->m_socket->AddressSet())return nullptr;			//�\�P�b�g����
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
		//�w�b�_�[��t�������M
		memcpy(&sendBuf[0], &sequence, sizeof(unsigned int));
		memcpy(&sendBuf[sizeof(unsigned int)], &_buf[0], _bufSize);

		//���M����
		sendDataSize = m_socket->Sendto(&sendBuf[0], _bufSize + sizeof(unsigned int));

		//�V�[�P���X�ԍ��Ǘ�
		sequence++;
		if (sequence > SEQUENCEMAX) { sequence = 0; }
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
		return sendDataSize;
	}
	return sendDataSize;
}

