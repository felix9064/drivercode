#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
IC���豸����ӿ�

*/

typedef vector< pair<CString, CString> > VPair;

typedef struct _stMagCard
{
	/********************************************
	* ��  �ܣ���������
	* ��  ����@outlist - ��������������
	* ��  ����@nPort - �˿ںš�
    * ��  ����@isUSB - �Ƿ�ΪUSB�豸��
	* ��  ����@errcd - �������
	* ��  ����@errmsg - ������Ϣ��
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *ReadCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);




	/********************************************
	* ��  �ܣ�д������
	* ��  ����@outlist - ���뿨��������
	* ��  ����@nPort - �˿ںš�
    * ��  ����@isUSB - �Ƿ�ΪUSB�豸��
	* ��  ����@errcd - �������
	* ��  ����@errmsg - ������Ϣ��
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *WriteCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);
	/********************************************
	* ��  �ܣ��豸���á�
	* ��  �����ޡ�
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *ResetDev)();

}MAG_INTERFACE;

/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
*********************************************/
BOOL WINAPI GetCardCOM(MAG_INTERFACE *pMagInterf);