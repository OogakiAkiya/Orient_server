#ifndef OPENSOCKET_UTILITY_h
#define OPENSOCKET_UTILITY_h

/**
*  @brief �t�@�C���f�B�X�N���v�^�������Ƃ��ēn�����Ƃ�select�֐������s����
* 
*  @param _fds		select�ŊĎ�����t�@�C���f�B�X�N���v�^
**/
void OpenSocket_Select(fd_set* _fds, int _maxfds = -1);

#endif