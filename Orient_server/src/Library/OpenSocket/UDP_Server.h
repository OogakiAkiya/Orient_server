#ifndef UDP_SERVER_h
#define UDP_SERVER_h

class UDP_Server :public BaseServer {
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<UDP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;																	//��M�f�[�^�̐����擾
	std::pair<B_ADDRESS_IN, std::vector<char>> GetRecvData();												//�N���C�A���g�����M�����f�[�^�����o��

	int SendOnlyClient(const B_ADDRESS_IN* _addr, const char* _buf, const int _bufSize);						//����̃N���C�A���g�ɑ��M����ꍇ�g�p����
	int SendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize);

private:
	unsigned int sequence = 0;																				//�V�[�P���X�ԍ�
	std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>> recvDataQueList;									//�N���C�A���g�����M������񂪓���

};

#endif