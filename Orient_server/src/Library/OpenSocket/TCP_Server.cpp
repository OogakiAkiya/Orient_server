#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseServer.h"
#include"TCP_Server.h"

std::shared_ptr<TCP_Server> TCP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<TCP_Server> temp = std::make_shared<TCP_Server>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_socket->Init(_addrs, _port);
	SwitchIpv(temp->m_socket, _ipv);
	temp->m_socket->SetProtocol_TCP();
	if (!temp->m_socket->AddressSet())return nullptr;
	if (!temp->m_socket->Bind())return nullptr;
	if (!temp->m_socket->Listen())return nullptr;
	if (_asynchronous)temp->m_socket->SetAsynchronous();
	return temp;
}

void TCP_Server::Update()
{
	std::shared_ptr<BaseSocket> client = nullptr;
	client = m_socket->Accept();
	if (client != nullptr) {
		clientList.push_back(client);
		std::vector<char> recvDataList;

		//�����\�P�b�g�̗L���m�F
		auto ite = recvDataMap.find(client->GetSocket());
		if (ite != recvDataMap.end()) recvDataMap.erase(ite);

		//�N���C�A���g�쐬
		recvDataMap.insert({ client->GetSocket(),recvDataList });

	}

	if (clientList.size() > 0) DataProcessing();
}

int TCP_Server::GetRecvDataSize()
{
	return recvDataQueList.size();
}

int TCP_Server::SendOnlyClient(const int _socket, const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//�w�b�_�[��t��
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//�G���h�}�[�J�[��t�^
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			if (clients->GetSocket() == _socket)sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Server::SendOnlyClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

int TCP_Server::SendAllClient(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//�w�b�_�[��t��
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//�G���h�}�[�J�[��t�^
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Server::SendAllClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

void TCP_Server::DataProcessing()
{
	std::list<int> deleteList;

	for (int i = 0; i < clientList.size(); i++) {
		char buf[TCP_BUFFERSIZE];
		int socket = clientList.at(i)->GetSocket();

		//�f�[�^����M�����ۂ͂��̃o�C�g��������ؒf���ꂽ�ꍇ��0,�m���u���b�L���O���[�h�Ńf�[�^����M���ĂȂ��Ԃ�-1��dataSize�ɓ���
		int dataSize = clientList.at(i)->Recv(buf, TCP_BUFFERSIZE);

		if (dataSize > 0) {

			//��M�f�[�^���i�[
			int nowSize = recvDataMap[socket].size();
			recvDataMap[socket].resize(nowSize + dataSize);
			memcpy((char*)&recvDataMap[socket][nowSize], &buf[0], dataSize);

			while (recvDataMap[socket].size() > sizeof(int)) {

				int dataSize = 0;
				try {
					memcpy(&dataSize, &recvDataMap[(B_SOCKET)socket][0], sizeof(int));

					//�擪�p�P�b�g���z�肵�Ă�������������܂��͑傫���p�P�b�g�̏ꍇ�͕s���p�P�b�g�Ƃ��ĉ��߂���B
					if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
						//TODO �s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
						recvDataMap[(B_SOCKET)socket].clear();
						return;
					}

					//�G���h�}�[�J�[�̒l������l���`�F�b�N
					if (memcmp(&recvDataMap[(B_SOCKET)socket][dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
						//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
						recvDataMap[(B_SOCKET)socket].clear();
						return;
					}
				}
				catch (std::exception e) {
					std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

					//TODO:�s���p�P�b�g�ȂǂŐ擪�f�[�^��int��memcpy�ł��Ȃ������ۂ̓p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					recvDataMap[(B_SOCKET)socket].clear();
					return;
				}

				//��M�f�[�^����򕪂���΃L���[���X�g�ɒǉ�
				if (recvDataMap[socket].size() > dataSize) {
					std::pair<B_SOCKET, std::vector<char>> addData;
					addData.first = socket;
					addData.second.resize(dataSize);
					memcpy(&addData.second[0], &recvDataMap[socket][sizeof(int)], dataSize);
					recvDataQueList.push(addData);
					recvDataMap[socket].erase(recvDataMap[socket].begin(), recvDataMap[socket].begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
				}
			}

		}
		else if (dataSize == 0) {
			//�ڑ����I������Ƃ�
			std::cout << "connection is lost" << std::endl;
			deleteList.push_back(i);
		}
#ifdef _MSC_VER
		else if (WSAGetLastError() == WSAEWOULDBLOCK) {
			//client��send���Ă��Ȃ������Ƃ��ɂ�����G���[
		}
#endif
		else {
#ifdef _MSC_VER

			//�ڑ��G���[���N��������
			std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
			deleteList.push_back(i);
#else
			//errno�̓V�X�e���R�[����W�����C�u�����̃G���[���i�[�����ϐ�
			if (errno == EAGAIN)
			{
				//�񓯊����Ƃ�����ʂ邱�Ƃ����邪�������ėǂ�
				break;
			}

			if (errno == EBADF)
			{
			}
			if (errno == ECONNRESET)
			{
				//�N���C�A���g�ڑ����Z�b�g
				std::cout << "connection is lost" << std::endl;
				deleteList.push_back(i);
				break;
			}
			//�ڑ��G���[���N��������
			std::cerr << "recv failed:" << errno << std::endl;
			deleteList.push_back(i);
#endif

		}
	}

	//�ؒf���ꂽ�\�P�b�g����
	for (auto element : deleteList) {
		//��M�f�[�^�폜
		int socket = clientList.at(element)->GetSocket();

#ifdef _MSC_VER
		recvDataMap[socket].erase(recvDataMap[socket].begin(), recvDataMap[socket].end());
#else
		recvDataMap.erase(socket);
#endif

		//�\�P�b�g�폜
		clientList.erase(clientList.begin() + element);

	}


}


std::pair<int, std::vector<char>> TCP_Server::GetRecvData()
{
	std::pair<int, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}
