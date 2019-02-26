#include "BmpLib.h"
#include <malloc.h>

int _Bmp_Read_Byte(void *Dest, FILE *pfp, int offset, int readNum)
{
	fseek(pfp, offset, SEEK_SET);
	fread(Dest, 1, readNum, pfp);
	return 0;
}

int _Bmp_Write_Byte(const void *pSrc, FILE *pfp, int offset, int writeNum)
{
	fseek(pfp, offset, SEEK_SET);
	fwrite(pSrc, 1, writeNum, pfp);
	return 0;
}

int _Bmp_Get_FileSize(FILE *fp)
{
	fseek(fp, 0, SEEK_END);   ///将文件指针移动文件结尾
	return ftell(fp);
}

int _Bmp_Get_RGBBlock(FILE *fp, FileInfo *fi)
{
	if ((fp == NULL) && (fi == NULL))
		return -1;

	if ((fi->InfoHeader.biBitCount == 24) || (fi->FileHeader.bfOffBits == (fi->InfoHeader.biSize + 14)))
	{
		fi->pRGBBuff = NULL;
		return 0;
	}

	BYTE *pBuf;
	int RGBNum = fi->FileHeader.bfOffBits - (fi->InfoHeader.biSize + 14);
	pBuf = (BYTE *)malloc(RGBNum);

	if (pBuf == NULL)
	{
		printf("调色板内存分配失败");
		fi->pRGBBuff = NULL;
		return -1;
	}

	fseek(fp, fi->InfoHeader.biSize + 14, SEEK_SET);

	fread(pBuf,1,RGBNum,fp);

	fi->pRGBBuff = pBuf;

	return 0;
}

int _Bmp_Write_RGBBlock(FILE *fp, FileInfo *fi)
{
	if ((fp == NULL) && (fi == NULL))
		return -1;

	if ((fi->InfoHeader.biBitCount == 24) || (fi->FileHeader.bfOffBits == (fi->InfoHeader.biSize + 14)))
	{
		return 0;
	}

	int RGBNum = fi->FileHeader.bfOffBits - (fi->InfoHeader.biSize + 14);

	fseek(fp, fi->InfoHeader.biSize + 14, SEEK_SET);
	fwrite(fi->pRGBBuff, 1, RGBNum, fp);

	return 0;
}

/*
功能：获取bmp裸数据（像素信息）
参数：fp文件指针
fi文件信息 ，可通过函数 Bmp_Get_FileInfo 获取
Buff 像素数据储存位置
*/
int _Bmp_Get_FileSrc(FILE *fp, FileInfo *fi)
{
	unsigned int index = 0;
	unsigned int readNum = 1000;
	BYTE *pBuff;

	if ((fp == NULL) && (fi == NULL))
		return -1;

	pBuff = (BYTE *)malloc(fi->InfoHeader.biSizeImage);

	fseek(fp, fi->FileHeader.bfOffBits, SEEK_SET);

	while (index != fi->InfoHeader.biSizeImage)
	{
		if ((index + readNum) > fi->InfoHeader.biSizeImage)
		{
			readNum = fi->InfoHeader.biSizeImage - index;
		}

		readNum = fread(&pBuff[index], 1, readNum, fp);
		index += readNum;

		if (readNum == 0)
		{
			if (index != fi->InfoHeader.biSizeImage)
			{
				printf("图片格式错误，或者确认文件的打开方式是否为'rb' !\n");
				return -1;
			}
		}
	}

	fi->pBuff = pBuff;

	return 0;
}

int _Bmp_Write_FileSrc(FILE *fp, FileInfo *fi)
{
	unsigned int index = 0;
	unsigned int writeNum = 1000;
	BYTE *pBuff;

	pBuff = fi->pBuff;

	if ((fp == NULL) || (fi == NULL) || (pBuff == NULL))
		return -1;

	fseek(fp, fi->FileHeader.bfOffBits, SEEK_SET);

	while (index != fi->InfoHeader.biSizeImage)
	{
		if ((index + writeNum) > fi->InfoHeader.biSizeImage)
		{
			writeNum = fi->InfoHeader.biSizeImage - index;
		}

		fwrite(&pBuff[index], 1, writeNum, fp);
		index += writeNum;
	}

	return 0;
}

int Bmp_Get_FileInfo(FILE *fp, FileInfo *fi)
{
	DWORD fileSize;
	if (fp == NULL)
		return -1;

	if (fi == NULL)
		fi = (FileInfo *)malloc(sizeof(FileInfo));

	_Bmp_Read_Byte(&fi->FileHeader.bfType, fp, 0,2);
	_Bmp_Read_Byte(&fi->FileHeader.bfSize, fp, 2, 4);
	_Bmp_Read_Byte(&fi->FileHeader.bfReserved1, fp, 6,2);
	_Bmp_Read_Byte(&fi->FileHeader.bfReserved2, fp, 8,2);
	_Bmp_Read_Byte(&fi->FileHeader.bfOffBits, fp, 10, 4);

	_Bmp_Read_Byte(&fi->InfoHeader.biSize, fp, 14, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biWidth, fp, 18, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biHeight, fp, 22, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biPlanes, fp, 26,2);
	_Bmp_Read_Byte(&fi->InfoHeader.biBitCount, fp, 28,2);
	_Bmp_Read_Byte(&fi->InfoHeader.biCompression, fp, 30, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biSizeImage, fp, 34, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biXPelsPerMeter, fp, 38, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biYPelsPerMeter, fp, 42, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biClrUsed, fp, 46, 4);
	_Bmp_Read_Byte(&fi->InfoHeader.biClrImportant, fp, 50, 4);

	fileSize = (DWORD)_Bmp_Get_FileSize(fp);

	if (fileSize != fi->FileHeader.bfSize)
	{
		printf("file size error %d != %d\n", fileSize, fi->FileHeader.bfSize);
		return -1;
	}

	_Bmp_Get_FileSrc(fp, fi);
	_Bmp_Get_RGBBlock(fp, fi);

	return 0;
}

/*
功能：确认输入信息，暂时不支持自动补全
*/
int _Bmp_Check_WriteInfo(FileInfo *fi)
{
	if (fi == NULL)
		return -1;

	// 基本头数据确认
	if ((fi->FileHeader.bfType != 0x4d42) ||
		(fi->FileHeader.bfReserved1 != 0) ||
		(fi->FileHeader.bfReserved2 != 0))
		return -1;

	//只支持8位 16位 24位
	if (!((fi->InfoHeader.biBitCount == 24) ||
		(fi->InfoHeader.biBitCount == 16) ||
		(fi->InfoHeader.biBitCount == 8))
		)
		return -1;

	// 24位图确认
	if ((fi->InfoHeader.biBitCount == 24) && (fi->FileHeader.bfOffBits != 54))
		return -1;

	//数据确认
	if (fi->pBuff == NULL)
		return -1;

	//调色板数据确认
	if ((fi->pRGBBuff == NULL))
	{
		// 24位没有调试板
		if ((fi->InfoHeader.biBitCount == 16) ||
			(fi->InfoHeader.biBitCount == 8)
			)
			return -1;
	}

	return 0;
}

/*
功能：封装为BMP文件
参数：FileName 生成文件名
fi文件信息，需要有宽高信息，如果不是24位，需要有调试板信息
*/
int Bmp_Write_File(const char *FileName, FileInfo *fi)
{
	FILE *fp;
	if ((FileName == NULL) || (fi == NULL))
		return -1;

	fp = fopen((const char *)FileName,"wb");
	if (fp == NULL)
		return -2;

	if (_Bmp_Check_WriteInfo(fi) != 0)
		return -1;

	_Bmp_Write_Byte((const void *)&fi->FileHeader.bfType, fp, 0, 2);
	_Bmp_Write_Byte((const void *)&fi->FileHeader.bfSize, fp, 2, 4);
	_Bmp_Write_Byte((const void *)&fi->FileHeader.bfReserved1, fp, 6, 2);
	_Bmp_Write_Byte((const void *)&fi->FileHeader.bfReserved2, fp, 8, 2);
	_Bmp_Write_Byte((const void *)&fi->FileHeader.bfOffBits, fp, 10, 4);

	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biSize, fp, 14,4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biWidth, fp, 18,4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biHeight, fp, 22,4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biPlanes, fp, 26,2);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biBitCount, fp, 28,2);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biCompression, fp, 30, 4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biSizeImage, fp, 34, 4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biXPelsPerMeter, fp, 38, 4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biYPelsPerMeter, fp, 42, 4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biClrUsed, fp, 46, 4);
	_Bmp_Write_Byte((const void *)&fi->InfoHeader.biClrImportant, fp, 50, 4);

	_Bmp_Write_RGBBlock(fp, fi);
	_Bmp_Write_FileSrc(fp, fi);

	fclose(fp);
	return 0;
}

int Bmp_Release_FileInfo(FileInfo *fi)
{
	if (fi == NULL)
		return 0;

	if (fi->pBuff != NULL)
		free(fi->pBuff);

	if (fi->pRGBBuff != NULL)
		free(fi->pRGBBuff);

	return 0;
}
