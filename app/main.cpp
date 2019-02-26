#include <stdio.h>
#include <malloc.h>
#include "BmpLib.h"


void main()
{
	FILE *fp;
	FileInfo fi;

	fp = fopen("123.bmp","rb");
	if (fp == NULL)
	{
		printf("file error \n");
		return;
	}

	if (Bmp_Get_FileInfo(fp, &fi) != 0)
	{
		printf("error \n");
		return;
	}

	printf("标志：0x%4x \n",fi.FileHeader.bfType);
	printf("大小：%d kb \n", fi.FileHeader.bfSize / 1024);
	printf("保留位1：0x%04x \n", fi.FileHeader.bfReserved1);
	printf("保留位2：0x%04x \n", fi.FileHeader.bfReserved2);
	printf("数据偏移：0x%04x \n", fi.FileHeader.bfOffBits);

	Bmp_Write_File("aaaa.bmp", &fi);

	Bmp_Release_FileInfo(&fi);

	fclose(fp);
}