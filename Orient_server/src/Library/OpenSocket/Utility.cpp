#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"Utility.h"

void OpenSocket_Select(fd_set* _fds,int _maxfds)
{
	
#ifdef _MSC_VER
	//winsock�ł͑������͖��������
	select(0, _fds, NULL, NULL, NULL);

#else
	//�ő�̃t�@�C���f�B�X�N���v�^���������Ă���ꍇ
	if (_maxfds != -1) {
		select(_maxfds + 1, _fds, NULL, NULL, NULL);
		return;
	}

	//�ő�̃t�@�C���f�B�X�N���v�^���擾(�t�@�C���f�B�X�N���v�^�̂Ƃ肤��͈͂�T��)
	int maxfds = -1;
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(i,_fds)) {
			maxfds = i;
		}
	}
	select(maxfds + 1, _fds, NULL, NULL, NULL);

#endif


}
