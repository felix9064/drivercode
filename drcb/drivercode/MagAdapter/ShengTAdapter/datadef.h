#ifndef DATADEF_IDSHENGT_20120905_H
#define DATADEF_IDSHENGT_20120905_H

#pragma once


//����ź���
#define SUCESS 0          //�ɹ�
#define ERR_OPENPORT -1    //�˿�δ��
#define ERR_TRANSPORT -2   //dataBuf���Ȳ���
#define ERR_ERRORDATA -3    //��ʱ



#define CAMERELOG_DIR              "log"


/*
����˵�������ſ�
������
	nPort	����������豸���Ӷ˿ڡ�����ȡֵCOM1��COM2������ ������ȡֵΪ��COM10ʱΪ����ͨ��
									����ΪSDT, HID , PCSC
	dwBaud	������������ڲ����ʣ���Ϊ����ʱ��Ч��
	chBpNo	���������BP�ڣ�ȡֵΪ"A"�� " B"'�� "C"�� "K"�ֱ�ΪA��B��C��K��, ȡֵΪNULL��""��ʾ������BP�ڡ�
	nCharset	����������ŵ���ʽ��ȡֵΪ0,1,2,3��Ӧ��ʾΪ0-ISO��ʽ��1-IBM��ʽ, 2-ISO��ʽ(�¿�)��3-IBM��ʽ(�¿�)
	nTimeout	�����������ʾѰ����ʱʱ�䣬��λΪ�롣
	szTrack1	�����������szTrack1ȡֵΪNULLʱ���򲻶�1�ŵ�����NULLʱ�򱣴�1�ŵ����ݡ�
	nTrack1Len	���������szTrack1�ĳ��ȡ�
	szTrack2	�����������szTrack2ȡֵΪNULLʱ���򲻶�2�ŵ�����NULLʱ�򱣴�2�ŵ����ݡ�
	nTrack2Len	���������szTrack2�ĳ��ȡ�
	szTrack3	�����������szTrack3ȡֵΪNULLʱ���򲻶�3�ŵ�����NULLʱ�򱣴�3�ŵ����ݡ�
	nTrack3Len	���������szTrack3�ĳ��ȡ�
���ز�����
	0	 �ɹ�
	-1	 ���豸ʧ��
	-2	 ����ָ��ʧ��----���豸ͨѶʧ��
	-3	 ��������ʧ��----���豸ͨѶʧ��
	-4   ���յ��ı��ĸ�ʽ����
	-5	 ��������ʧ��
	-6	 д������ʧ��
	-7	 �û�ȡ������
	-8	 ��ʱ
	-100 ��������
*/
typedef long (CALLBACK *lpCT_ReadMsgCard)(const char* nPort, const int dwBaud, const char* chBpNo, const int nCharset, const int nTimeout,
										 char* szTrack1, const int nTrack1Len, char* szTrack2, const int nTrack2Len, char* szTrack3, const int nTrack3Len);


/*
����˵����д�ſ�
	nPort	����������豸���Ӷ˿ڡ�����ȡֵCOM1��COM2������ ������ȡֵΪ��COM10ʱΪ����ͨ��
									����ΪSDT, HID , PCSC
	dwBaud	������������ڲ����ʣ���Ϊ����ʱ��Ч��
	chBpNo	���������BP�ڣ�ȡֵΪ"A"�� " B"'�� "C"�� "K" �ֱ�ΪA��B��C��K�ڣ�ȡֵΪNULL��""��ʾ������BP�ڡ�
	nCharset	����������ŵ���ʽ��ȡֵΪ0,1,2,3��Ӧ��ʾΪ0-ISO��ʽ��1-IBM��ʽ, 2-ISO��ʽ(�¿�)��3-IBM��ʽ(�¿�)
	nTimeout	�����������ʾѰ����ʱʱ�䣬��λΪ�롣
	szTrack1	�����������szTrack1ȡֵΪNULLʱ����д1�ŵ�����NULLʱ������д��1�ŵ���
	szTrack2	�����������szTrack2ȡֵΪNULLʱ����д2�ŵ�����NULLʱ������д��2�ŵ���
	szTrack3	�����������szTrack3ȡֵΪNULLʱ����д3�ŵ�����NULLʱ������д��3�ŵ���

 ���ز�����
	0	 �ɹ�
	-1	 ���豸ʧ��
	-2	 ����ָ��ʧ��----���豸ͨѶʧ��
	-3	 ��������ʧ��----���豸ͨѶʧ��
	-4   ���յ��ı��ĸ�ʽ����
	-5	 ��������ʧ��
	-6	 д������ʧ��
	-7	 �û�ȡ������
	-8	 ��ʱ
	-100 ��������
*/
typedef long (CALLBACK *lpCT_WriteMsgCard)(const char* nPort, const int dwBaud, const char* chBpNo, const int nCharset,
										  const int nTimeout, const char* szTrack1, const char* szTrack2, const char* szTrack3);


typedef int (CALLBACK *lpCT_CancelReadMsgCard)();



#endif