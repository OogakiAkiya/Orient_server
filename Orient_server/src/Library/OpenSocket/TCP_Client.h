#ifndef TCP_CLIENT_h
#define TCP_CLIENT_h

class TCP_Client :public BaseClient {
public:
	TCP_Client() {}
	~TCP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual int GetRecvDataSize()override { return recvDataQueList.size(); }			//�T�[�o�[�����M�����f�[�^���������邩
	virtual std::vector<char> GetRecvData()override;									//�T�[�o�[�����M�����f�[�^�����o��
	virtual int SendServer(const char* _buf, const int _bufSize)override;				//����̃T�[�o�[�ɑ��M����ꍇ�g�p����

private:
	//�����o�֐�
	void DataProcessing();

	//�����o�ϐ�
	std::vector<char> recvData;
};

#endif