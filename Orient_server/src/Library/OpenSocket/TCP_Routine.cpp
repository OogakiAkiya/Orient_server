#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"TCP_Routine.h"

void TCP_Routine::Update(const std::shared_ptr<BaseSocket> _socket, std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList)
{
	char buf[TCP_BUFFERSIZE];
	int dataSize = _socket->Recv(buf, TCP_BUFFERSIZE);

	if (dataSize > 0) {
		//��M�f�[�^���i�[
		int nowSize = _recvData.size();
		_recvData.resize(nowSize + dataSize);
		memcpy((char*)&_recvData[nowSize], &buf[0], dataSize);

		while (_recvData.size() > sizeof(int)) {
			int dataSize;
			try {
				//�擪�p�P�b�g�̉��
				memcpy(&dataSize, &_recvData[0], sizeof(int));

				//�擪�p�P�b�g���z�肵�Ă�������������܂��͑傫���p�P�b�g�̏ꍇ�͕s���p�P�b�g�Ƃ��ĉ��߂���B
				if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
					//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					_recvData.clear();
					return;
				}

				//�G���h�}�[�J�[�̒l������l���`�F�b�N
				if (memcmp(&_recvData[dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
					//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					_recvData.clear();
					return;
				}

			}
			catch (std::exception e) {
				std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

				//TODO:�s���p�P�b�g�ȂǂŐ擪�f�[�^��int��memcpy�ł��Ȃ������ۂ̓p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
				_recvData.clear();
				return;
			}

			//��M�f�[�^����򕪂���΃L���[�ɒǉ�
			if (_recvData.size() > dataSize) {
				std::vector<char> addData;
				addData.resize(dataSize);
				memcpy(&addData[0], &_recvData[sizeof(int)], dataSize);
				_recvDataQueList.push(addData);
				_recvData.erase(_recvData.begin(), _recvData.begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
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

void TCP_Routine::Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<int, std::vector<char>>& _recvDataMap, std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList)
{
	std::list<int> deleteList;

	for (int i = 0; i < _clientList.size(); i++) {
		char buf[TCP_BUFFERSIZE];
		int socket = _clientList.at(i)->GetSocket();

		//�f�[�^����M�����ۂ͂��̃o�C�g��������ؒf���ꂽ�ꍇ��0,�m���u���b�L���O���[�h�Ńf�[�^����M���ĂȂ��Ԃ�-1��dataSize�ɓ���
		int dataSize = _clientList.at(i)->Recv(buf, TCP_BUFFERSIZE);

		if (dataSize > 0) {

			//��M�f�[�^���i�[
			int nowSize = _recvDataMap[socket].size();
			_recvDataMap[socket].resize(nowSize + dataSize);
			memcpy((char*)&_recvDataMap[socket][nowSize], &buf[0], dataSize);

			while (_recvDataMap[socket].size() > sizeof(int)) {

				int dataSize = 0;
				try {
					memcpy(&dataSize, &_recvDataMap[(B_SOCKET)socket][0], sizeof(int));

					//�擪�p�P�b�g���z�肵�Ă�������������܂��͑傫���p�P�b�g�̏ꍇ�͕s���p�P�b�g�Ƃ��ĉ��߂���B
					if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
						//TODO �s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
						_recvDataMap[(B_SOCKET)socket].clear();
						return;
					}

					//�G���h�}�[�J�[�̒l������l���`�F�b�N
					if (memcmp(&_recvDataMap[(B_SOCKET)socket][dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
						//TODO:�s���p�P�b�g�Ƃ݂Ȃ����ꍇ�p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
						_recvDataMap[(B_SOCKET)socket].clear();
						return;
					}
				}
				catch (std::exception e) {
					std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

					//TODO:�s���p�P�b�g�ȂǂŐ擪�f�[�^��int��memcpy�ł��Ȃ������ۂ̓p�P�b�g�����ׂč폜���Ă��邪���������肪�Ȃ����l����
					_recvDataMap[(B_SOCKET)socket].clear();
					return;
				}

				//��M�f�[�^����򕪂���΃L���[���X�g�ɒǉ�
				if (_recvDataMap[socket].size() > dataSize) {
					std::pair<B_SOCKET, std::vector<char>> addData;
					addData.first = socket;
					addData.second.resize(dataSize);
					memcpy(&addData.second[0], &_recvDataMap[socket][sizeof(int)], dataSize);
					_recvDataQueList.push(addData);
					_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
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
		int socket = _clientList.at(element)->GetSocket();

#ifdef _MSC_VER
		_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].end());
#else
		_recvDataMap.erase(socket);
#endif

		//�\�P�b�g�폜
		_clientList.erase(_clientList.begin() + element);

	}


}
