#ifndef BMPLIB_H
#define BMPLIB_H
#include <stdio.h>

#ifdef DLL_EXPORT
#define CALL_Dll extern "C" __declspec(dllexport)  // 此时为生成dll，为保证兼容，限制以C的方式
#else
#define CALL_Dll extern "C" __declspec(dllimport)  // 此时为导入dll，为保证兼容，限制以C的方式
#endif 

#define WORD  unsigned short
#define DWORD unsigned int 
#define LONG  unsigned int
#define BYTE  unsigned char

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType;         //2Bytes   位图文件的类型，必须为BM 
	DWORD   bfSize;         //4Bytes   位图文件的大小，以字节为单位   
	WORD    bfReserved1;    //2Bytes   位图文件保留字，必须为0 
	WORD    bfReserved2;    //2Bytes   位图文件保留字，必须为0 
	DWORD   bfOffBits;      //4Bytes   位图数据的起始位置，以相对于位图 
	//   文件头的偏移量表示，以字节为单位 
}BITMAPFILEHEADER;

typedef struct _tagBMP_INFOHEADER
{
	DWORD  biSize;              //4Bytes，INFOHEADER结构体大小，存在其他版本I NFOHEADER，用作区分
	LONG   biWidth;             //4Bytes，图像宽度（以像素为单位）
	LONG   biHeight;            //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
	WORD   biPlanes;            //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
	WORD   biBitCount;          //2Bytes，图像像素位数
	DWORD  biCompression;       //4Bytes，0：不压缩，1：RLE8，2：RLE4
	DWORD  biSizeImage;         //4Bytes，4字节对齐的图像数据大小
	LONG   biXPelsPerMeter;     //4 Bytes，用象素/米表示的水平分辨率
	LONG   biYPelsPerMeter;     //4 Bytes，用象素/米表示的垂直分辨率
	DWORD  biClrUsed;           //4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
	DWORD  biClrImportant;      //4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
}BMP_INFOHEADER;

typedef struct _tagRGBQUAD
{
	BYTE  rgbBlue;       //指定蓝色强度
	BYTE  rgbGreen;      //指定绿色强度
	BYTE  rgbRed;        //指定红色强度
	BYTE  rgbReserved;  //保留，设置为0
}RGBQUAD;

typedef struct _tagFileInfo
{
	BITMAPFILEHEADER FileHeader;
	BMP_INFOHEADER   InfoHeader;

	BYTE *pBuff = NULL;
	BYTE *pRGBBuff = NULL;
}FileInfo;

/*
功能：获取bmp文件信息
参数：fp文件指针
      fi文件信息储存位置，由用户定义指针即可，内部自动分配内存
*/
CALL_Dll int Bmp_Get_FileInfo(FILE *fp, FileInfo *fi);

/*
功能：封装为BMP文件
参数：FileName 生成文件名
	  fi文件信息，需要有宽高信息，如果不是24位，需要有调试板信息
说明：暂时不支持自动补全属性
*/
CALL_Dll int Bmp_Write_File(const char *FileName, FileInfo *fi);

/*
功能：释放bmp文件信息内存
参数：fi 文件信息指针
*/
CALL_Dll int Bmp_Release_FileInfo(FileInfo *fi);

#endif