#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
�������֤�豸����ӿ�

*/

typedef vector< pair<CString, CString> > VPair;

typedef struct _stIdentityCard
{
	/********************************************
	* ��  �ܣ������ӿڡ�
	* ��  ����@outlist - �������֤��������
	* ��  ����@nPort - �˿ںš�
    * ��  ����@isUSB - �Ƿ�ΪUSB�豸��
	* ��  ����@errcd - �������
	* ��  ����@errmsg - ������Ϣ��
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *ReadIdentityCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);

	/********************************************
	* ��  �ܣ��˲�ӿڡ�
	* ��  ����@outlist - ����˲�������Ϣ�������˲�����
	* ��  ����@nPort - �˿ںš�
    * ��  ����@isUSB - �Ƿ�ΪUSB�豸��
	* ��  ����@errcd - �������
	* ��  ����@errmsg - ������Ϣ��
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *CheckIdentityCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);
	
	/********************************************
	* ��  �ܣ���ӡ�˲�����
	* ��  ����@outlist - �����ӡ�������ݡ�
	* ��  ����@nPort - �˿ںš�
    * ��  ����@isUSB - �Ƿ�ΪUSB�豸��
	* ��  ����@errcd - �������
	* ��  ����@errmsg - ������Ϣ��
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *PrintCHKIDInfo)(VPair &outlist, char *errcd, char *errmsg);


	/********************************************
	* ��  �ܣ��豸���á�
	* ��  �����ޡ�
	* ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *ResetDev)();

	int (WINAPI *LoadDriver)(char *ThirdDrv);
	int (WINAPI *FreeDriver)();

}IDENT_CARD_INTERFACE;

/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
*********************************************/
BOOL WINAPI GetIdentityCardCOM(IDENT_CARD_INTERFACE *pIdentCardInterf);