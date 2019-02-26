#ifndef BMPLIB_H
#define BMPLIB_H
#include <stdio.h>

#ifdef DLL_EXPORT
#define CALL_Dll extern "C" __declspec(dllexport)  // ��ʱΪ����dll��Ϊ��֤���ݣ�������C�ķ�ʽ
#else
#define CALL_Dll extern "C" __declspec(dllimport)  // ��ʱΪ����dll��Ϊ��֤���ݣ�������C�ķ�ʽ
#endif 

#define WORD  unsigned short
#define DWORD unsigned int 
#define LONG  unsigned int
#define BYTE  unsigned char

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType;         //2Bytes   λͼ�ļ������ͣ�����ΪBM 
	DWORD   bfSize;         //4Bytes   λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ   
	WORD    bfReserved1;    //2Bytes   λͼ�ļ������֣�����Ϊ0 
	WORD    bfReserved2;    //2Bytes   λͼ�ļ������֣�����Ϊ0 
	DWORD   bfOffBits;      //4Bytes   λͼ���ݵ���ʼλ�ã��������λͼ 
	//   �ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ 
}BITMAPFILEHEADER;

typedef struct _tagBMP_INFOHEADER
{
	DWORD  biSize;              //4Bytes��INFOHEADER�ṹ���С�����������汾I NFOHEADER����������
	LONG   biWidth;             //4Bytes��ͼ���ȣ�������Ϊ��λ��
	LONG   biHeight;            //4Bytes��ͼ��߶ȣ�+��ͼ��洢˳��ΪBottom2Top��-��Top2Bottom
	WORD   biPlanes;            //2Bytes��ͼ������ƽ�棬BMP�洢RGB���ݣ������Ϊ1
	WORD   biBitCount;          //2Bytes��ͼ������λ��
	DWORD  biCompression;       //4Bytes��0����ѹ����1��RLE8��2��RLE4
	DWORD  biSizeImage;         //4Bytes��4�ֽڶ����ͼ�����ݴ�С
	LONG   biXPelsPerMeter;     //4 Bytes��������/�ױ�ʾ��ˮƽ�ֱ���
	LONG   biYPelsPerMeter;     //4 Bytes��������/�ױ�ʾ�Ĵ�ֱ�ֱ���
	DWORD  biClrUsed;           //4 Bytes��ʵ��ʹ�õĵ�ɫ����������0��ʹ�����еĵ�ɫ������
	DWORD  biClrImportant;      //4 Bytes����Ҫ�ĵ�ɫ����������0�����еĵ�ɫ����������Ҫ
}BMP_INFOHEADER;

typedef struct _tagRGBQUAD
{
	BYTE  rgbBlue;       //ָ����ɫǿ��
	BYTE  rgbGreen;      //ָ����ɫǿ��
	BYTE  rgbRed;        //ָ����ɫǿ��
	BYTE  rgbReserved;  //����������Ϊ0
}RGBQUAD;

typedef struct _tagFileInfo
{
	BITMAPFILEHEADER FileHeader;
	BMP_INFOHEADER   InfoHeader;

	BYTE *pBuff = NULL;
	BYTE *pRGBBuff = NULL;
}FileInfo;

/*
���ܣ���ȡbmp�ļ���Ϣ
������fp�ļ�ָ��
      fi�ļ���Ϣ����λ�ã����û�����ָ�뼴�ɣ��ڲ��Զ������ڴ�
*/
CALL_Dll int Bmp_Get_FileInfo(FILE *fp, FileInfo *fi);

/*
���ܣ���װΪBMP�ļ�
������FileName �����ļ���
	  fi�ļ���Ϣ����Ҫ�п����Ϣ���������24λ����Ҫ�е��԰���Ϣ
˵������ʱ��֧���Զ���ȫ����
*/
CALL_Dll int Bmp_Write_File(const char *FileName, FileInfo *fi);

/*
���ܣ��ͷ�bmp�ļ���Ϣ�ڴ�
������fi �ļ���Ϣָ��
*/
CALL_Dll int Bmp_Release_FileInfo(FileInfo *fi);

#endif