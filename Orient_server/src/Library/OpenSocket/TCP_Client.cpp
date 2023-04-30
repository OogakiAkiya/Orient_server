#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseClient.h"
#include"TCP_Client.h"


void TCP_Client::Update()
{
	DataProcessing();
}

std::shared_ptr<BaseClient> TCP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<TCP_Client> temp = std::make_shared<TCP_Client>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_socket->Init(_addrs, _port);						//IP�A�h���X�ƃ|�[�g�ԍ��̐ݒ�
	SwitchIpv(temp->m_socket, _ipv);							//IPv�̐ݒ�
	temp->m_socket->SetProtocol_TCP();							//TCP�ʐM�ɐݒ�
	if (!temp->m_socket->AddressSet())return nullptr;			//�\�P�b�g����
	if (!temp->m_socket->Connect())return nullptr;				//�R�l�N�g����
	if (_asynchronous)temp->m_socket->SetAsynchronous();

	return temp;
}

std::vector<char> TCP_Client::GetRecvData()
{
	std::vector<char> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}

int TCP_Client::SendServer(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//�w�b�_�[��t��
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//�G���h�}�[�J�[��t�^
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		//���M
		sendDataSize = m_socket->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

void TCP_Client::DataProcessing()
{
	//�t�@�C���f�B�X�N���v�^���ݒ肳��Ă���r�b�g�t���O�������Ă��Ȃ��ꍇ������悤�ɂ���
	if (fds != nullptr) {
		if (!FD_ISSET(m_socket->GetSocket(), fds)) {
			return;
		}
	}


	char buf[TCP_BUFFERSIZE];
	int dataSize = m_socket->Recv(buf, TCP_BUFFERSIZE);

	if (dataSize > 0) {
		//��M�f�[�^���i�[
		int nowSize = recvData.size();
		recvData.resize(nowSize + dataSize);
		memcpy((char*)&recvData[nowSize], &buf[0], dataSize);

		while (recvData.size() > sizeof(int)) {
			int dataSize;
			try {
				//�擪�p�P�b�g�̉��
				memcpy(&dataSize, &recvData[0], sizeof(int));

				//�擪�p�P�b�g���z�肵�Ă�������������܂��͑傫���p�P�b�g�̏ꍇ�͕s���p�P�b�g�Ƃ��ĉ��߂���B
				if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
					//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					recvData.clear();
					return;
				}

				//�G���h�}�[�J�[�̒l������l���`�F�b�N
				if (memcmp(&recvData[dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
					//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					recvData.clear();
					return;
				}

			}
			catch (const std::exception& e) {
				std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

				//TODO:�s���p�P�b�g�ȂǂŐ擪�f�[�^��int��memcpy�ł��Ȃ������ۂ̓p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
				recvData.clear();
				return;
			}

			//��M�f�[�^����򕪂���΃L���[�ɒǉ�
			if (recvData.size() > dataSize) {
				std::vector<char> addData;
				addData.resize(dataSize);
				memcpy(&addData[0], &recvData[sizeof(int)], dataSize);
				recvDataQueList.push(addData);
				recvData.erase(recvData.begin(), recvData.begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
			}
		}
	}
	else if (dataSize == 0) {
		//�ڑ����I������Ƃ�
		std::cout << "connection is lost" << std::endl;
	}
#ifdef _MSC_VER
	else if (WSAGetLastError() == WSAEWOULDBLOCK) {
		//client��send���Ă��Ȃ������Ƃ��ɂ�����G���[(return�ŗǂ�����)
	}
#endif
	else {
#ifdef _MSC_VER
		//�ڑ��G���[���N��������
		std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
#else
		if (errno == EAGAIN)
		{
			//�񓯊����Ƃ�������{�͒ʂ�
			return;
		}

		//�ڑ��G���[���N��������
		std::cerr << "recv failed" << std::endl;

#endif

	}
}
