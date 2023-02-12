#ifndef UDP_CLIENT_h
#define UDP_CLIENT_h

class UDP_Client :public BaseClient {
public:
	UDP_Client() {}
	~UDP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);

	virtual int GetRecvDataSize()override { return recvDataQueList.size(); }			//�T�[�o�[�����M�����f�[�^���������邩
	virtual std::vector<char> GetRecvData()override;									//�T�[�o�[�����M�����f�[�^�����o��
	virtual int SendServer(const char* _buf, const int _bufSize)override;				//����̃T�[�o�[�ɑ��M����ꍇ�g�p����

private:
	//�����o�֐�
	void DataProcessing();

	//�����o�ϐ�
	unsigned int sequence = 0;															//�V�[�P���X�ԍ�
	std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>> recvDataQueList;				//�N���C�A���g�����M������񂪓���

};

#endif