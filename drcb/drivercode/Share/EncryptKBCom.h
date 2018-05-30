#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
  ���ܼ��������׼�ӿ�
*/

typedef vector< pair<CString, CString> > VPair;
typedef const char* TPCHR;
typedef char*       PCHR;
typedef HINSTANCE HDLL;

typedef struct _stKBAdapter
{
    /********************************************
    * ��  �ܣ�����������
    * ��  ����@path - ������·��
    * ����ֵ���ɹ����ط�0,����0
	*********************************************/
	HDLL (WINAPI *SFKBLoadDrv)(TPCHR path, PCHR ErrDesc);

    /********************************************
    * ��  �ܣ��ͷ�������
    * ��  ����@hDrv - ��������
    * ����ֵ����
	*********************************************/
	void (WINAPI *SFKBFreeDrv)(HDLL hDrv);

    /********************************************
    * ��  �ܣ�����Pin
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں� 5-���볤�� 6-������ 7-�����������
    * ��  ����@MKeyIdx - ����Կ������
    * ��  ����@WKeyIdx - ������Կ������
	* ��  ����@ACCTNO  - �ͻ��ʺ�
	* ��  ����@mode    - ���ܷ�ʽ 0 - Ĭ��ֵdes���� 1 - ������ 2-des���� 3-3des����
	* ��  ����@PIN     - ����������Pin����
	* ��  ����@TPKCheck - ��������������PIN��TPK��ԿУ��ֵ��
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *SFKBReadPin)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR ACCTNO, int mode, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc);
	/*
	������Ļȷ����Ϣ
	*/
	int (WINAPI *SFKBReadPin2)(HDLL hDrv, VPair &inList, TPCHR MKeyIdx, TPCHR WKeyIdx, TPCHR ACCTNO, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc);

    /********************************************
    * ��  �ܣ�����TMK��������̡�
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں� 5-���볤�� 
	* ��  ����@MKeyIdx - ����Կ������
	* ��  ����@lpOleMKey  - ����Կ��
	* ��  ����@lpMKey  - �����ò�����
	* ��  ����@mode    - ���ܷ�ʽ��
	* ��  ����@lpKeyCK - ��ԿУ��ֵ�����봫��������
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *SFKBLoadMasterKey)(HDLL hDrv, VPair &inList, int MKeyIdx, TPCHR lpOldMKey, TPCHR lpMKey, int mode, PCHR lpKeyCK, PCHR ErrDesc);

    /********************************************
    * ��  �ܣ�����TPK/TAK��������̡�
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
	* ��  ����@MKeyIdx - ����Կ������
	* ��  ����@WKeyIdx - ������Կ������
	* ��  ����@lpMKey  - ������Կ��
	* ��  ����@lpKeyCK - ��ԿУ��ֵ��
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *SFKBLoadWorkKey)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpWKey, TPCHR lpKeyCK, PCHR ErrDesc);

	    /********************************************
    * ��  �ܣ�����TPK/TAK��������̡�
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
		TMKIndex	�ַ�����ʹ��TMK�������ţ�����Ϊ3���ȵ��ַ�����ȡֵ��ΧΪ"001"-"007"���Ϸ��ַ�Ϊ0~9������"001"�� 
		KEYIndex	�ַ�����ʹ��TPK/TAK�������ţ�����Ϊ2���ȵ��ַ�����ȡֵ��ΧΪ"01"-"02"���Ϸ��ַ�Ϊ0~2��"01"��ʾTPK��"02"��ʾTAK��
		KeyLen		�ַ�������Կ���ȱ�־��ֻ��ȡ1/2/3������1�ַ�����"1"��������Կ��"2"˫������Կ��"3"��������Կ��
		keyByTMK	�ַ��������ն�����Կ���ܵ���Կ������ΪkeyLen���������ȿ���ȡ16/32/48��
		KeyValue	�ַ���������8�ַ�����Կ���ļ���64bit0ȡ���ǰ���֡�
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (*SFKBLoadWorkKey2)(HDLL hDrv, VPair &inList, const char *MKeyIdx, const char *WKeyIdx, const char * KeyLen, const char * keyByTMK, const char * KeyValue, PCHR ErrDesc);

    /********************************************
    * ��  �ܣ�����MAC��
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
	* ��  ����@MKeyIdx - ����Կ������
	* ��  ����@WKeyIdx - ������Կ������
	* ��  ����@lpData  - mac����
	* ��  ����@lpMAC  - ����������MACֵ��
	* ��  ����@lpTAKKCK - ����������TAKУ��ֵ��
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *SFKBGenerateMAC)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, PCHR lpMAC, PCHR lpTAKKCK, PCHR ErrDesc);

    /********************************************
    * ��  �ܣ���֤MAC��
    * ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
	* ��  ����@MKeyIdx - ����Կ������
	* ��  ����@WKeyIdx - ������Կ������
	* ��  ����@lpData  - mac����
	* ��  ����@lpMAC  - MACֵ��
    * ����ֵ���ɹ�����0,�����0
	*********************************************/
	int (WINAPI *SFKBVerifyMAC)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, TPCHR lpMAC, PCHR ErrDesc);

	/********************************************
    * ��  �ܣ���λ
    * ��  ����@hDrv - ���
    * ����ֵ����
	*********************************************/
	void (WINAPI *SFKBResetDev)(HDLL hDrv, VPair &inList,PCHR ErrDesc);

	/********************************************
    * ��  �ܣ���ȡӦ�ñ��
    * ��  ����@hDrv - ���
    * ����ֵ����
	*********************************************/
	int (*KPGetPinPadID)(HDLL hDrv, VPair &inList, PCHR result, PCHR ErrDesc);

	/********************************************
	���ܣ���������
    ������
		CommPort���ַ������Կ��ַ�������ʹ�ô��ڵĶ˿ںš�����"COM1"��
		ExtPort, �ַ������Կ��ַ�������ʹ�ô�����չ��ʱ�á���ʹ�ô�����չ��ʱΪNULL��
		VocFlag, �ַ�������1��:������ʾ�������Ա��η���������� 
						��0��:��������ʾһ���� 
		timeout		�ַ���������ĵȴ�ʱ�䣬��λΪ�롣���������Ҫ�����ó�ʱ���ܡ�
		���أ�
		0		�ͻ����������
		1		�ͻ�����һ���
		2		�ͻ����²������		
	*********************************************/
	int (*PinPadReadEvaluate)(HDLL hDrv, VPair &inList,PCHR ErrDesc);

	int (*driveKeyBd)(HDLL hDrv, VPair &inList, PCHR ErrDesc);
}KB_ADAPTER_INTERFACE;

/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
*********************************************/
BOOL WINAPI GetSFKBCOM(KB_ADAPTER_INTERFACE *pInterf);

typedef BOOL (CALLBACK * LPGetSFKBCOM)(KB_ADAPTER_INTERFACE *);