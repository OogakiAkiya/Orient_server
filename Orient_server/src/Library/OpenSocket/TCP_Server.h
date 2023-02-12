#ifndef TCP_SERVER_h
#define TCP_SERVER_h

class TCP_Server :public BaseServer {
public:
	TCP_Server() {}
	~TCP_Server() { m_socket->Close(); }
	static std::shared_ptr<TCP_Server> GetInstance(
		const std::string _addrs,
		const std::string _port,
		const int _ipv,
		const bool _asynchronous = false
	);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;
	std::pair<int, std::vector<char>> GetRecvData();													//�N���C�A���g�����M�����f�[�^�����o��

	int SendOnlyClient(const int _socket, const char* _buf, const int _bufSize);						//����̃N���C�A���g�ɑ��M����ꍇ�g�p����
	int SendAllClient(const char* _buf, const int _bufSize);											//�S�ẴN���C�A���g�ɑ��M����ꍇ�g�p����

private:
	//�����o�֐�
	void DataProcessing();

	//�����o�ϐ�
	std::unordered_map<int, std::vector<char>> recvDataMap;		   //�e�N���C�A���g���Ƃ�recvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;		   //�N���C�A���g�̃\�P�b�g�����Ǘ�
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList; //�N���C�A���g�����M������񂪓���

};

#endif