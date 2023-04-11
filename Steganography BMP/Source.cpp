#include "iostream"
#include "stdio.h"
#include "Windows.h"
#include "conio.h"

//This program accepts BMP format pictures with 32/24 bits per pixel.
//The size of message that can be hidden in BMP is restricted with the size of BMP file itself.
//If the message is extracted from BMP file where it wasn't hidden the output message will consist of junk.


#define OneThing 1//Read only one structure
#define Byte 8//Bits per 1 byte

typedef struct
{
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER InfoHeader;
	CHAR PathIn[256];
	FILE *PicIn;
	BYTE *Buffer;
}Picture;




int main(int argc, char *argv[])
{
	//Error happened
	int Dummy = -1;
	
	int MemoryForOneCh;
	//Iterator variables
	int MessageOffset = 0;
	int BufferOffset = 0;
	int j = 0;
	int LinesCounter = 0;
	//Flags
	int HideOrGet = 0;
	bool JustEndedLine = true;

	Picture PIC;
	LONG SizeOfMessage;
	WORD SizeOfBuff;

	WORD BM = 0x4D42;

	CHAR Mask = 0xb00000001;
	CHAR Symbol;
	CHAR *Message;
	CHAR PathOut[256];
	FILE *PicOut;



////////////////////BEGIN
	printf("Enter a path to the file you want to hide the message/get the message:\r\n");
	fflush(stdin);
	fgets(PIC.PathIn, 255, stdin);
	PIC.PathIn[strlen(PIC.PathIn)-1] = '\0';
	
	//Whether file exists or not
	if ((PIC.PicIn = fopen(PIC.PathIn, "rb")) == NULL)
	{
		printf("Cannot open file.\r\n");
		system("pause");
		return Dummy;
	}
	//Read first header
	fread(&PIC.FileHeader, sizeof(BITMAPFILEHEADER), OneThing, PIC.PicIn);
	
	//Whether this file a bitmap or not
	if (PIC.FileHeader.bfType != BM)
	{
		printf("This file is not appropriate to hide the message.\r\n");
		system("pause");
		return Dummy;
       
	}
	//Read second header
	fread(&PIC.InfoHeader, sizeof(BITMAPINFOHEADER), OneThing, PIC.PicIn);
	//|bfHeight| as it can be negative number
	PIC.InfoHeader.biHeight = abs(PIC.InfoHeader.biHeight);

	//Programm accepts only 32 and 24 bit bitmaps
	if ((PIC.InfoHeader.biBitCount != 24) && (PIC.InfoHeader.biBitCount != 32))
	{
		printf("Picture is not compatible with this program.\r\n");
		system("pause");
        return Dummy;

	}

	// biPlanes > 1 was used in old format Beat-Maps
	if (PIC.InfoHeader.biPlanes != 1)
	{
		printf("Picture has uncompatible format.\r\n");
		system("pause");
		return Dummy;
	}

	//After all tests, this picture is appropriate to hide a message
	printf("This picture is appropriate to hide/get a message.\r\n");

	//Count the sizes of Message and Buffer according to the picture accepted
	MemoryForOneCh = Byte;
	SizeOfMessage = PIC.FileHeader.bfSize - PIC.FileHeader.bfOffBits;
	SizeOfMessage = SizeOfMessage / MemoryForOneCh - 2;
	SizeOfBuff = (PIC.FileHeader.bfSize - PIC.FileHeader.bfOffBits) / PIC.InfoHeader.biHeight;// Size of Buff equals to one row of pixels (in bytes)

	Message = new CHAR[SizeOfMessage];
	memset(Message, 0x00, SizeOfMessage);
	
	//Whether the picture already has the Message or the message must be hidden in the picture
	printf("You want to hide or get the message?(1 - Hide, 0 - Get)\r\n");
	scanf("%d", &HideOrGet);

	//If none then close the program
	if (HideOrGet != 1 && HideOrGet != 0)
	{
		printf("You chose none.\r\n");
		system("pause");
		return Dummy;
	}

	if (HideOrGet == 1)
	{
		//Set the name of the file where the picture with message will be saved
		printf("Enter the name of file you want to save the picture with hidden message in:\r\n");
		fflush(stdin);
		fgets(PathOut, 255, stdin);
		PathOut[strlen(PathOut)-1] = '\0';

		//If names of both opened files are the same, end the program
		if (strcmp(PathOut, PIC.PathIn) == 0)
		{
			printf("File names of old and new picture are the same.\r\n");
			system("pause");
			return(Dummy);
		}
		//Else open the new file for writing
		PicOut = fopen(PathOut, "wb");
	
		//Set the message that will be hidden
		printf("You can hide message size of %i\r\nEnter your message\r\n!!!DO NOT use ' ' to divide words!!!\r\n", SizeOfMessage);
		fflush(stdin);
		fgets(Message, SizeOfMessage-1, stdin);
		Message[strlen(Message)-1] = '\0';

		if (Message == '\0')
		{
			printf("There is nothing to hide\r\n");
			system("pause");
			return (Dummy);
		}
		//Writing into new file everything that is prior to the bitmap
		fseek(PIC.PicIn, 0, SEEK_SET);//To the beginning of a file
		PIC.Buffer = new BYTE[PIC.FileHeader.bfOffBits];
		fread(PIC.Buffer, PIC.FileHeader.bfOffBits, OneThing, PIC.PicIn);
		fwrite(PIC.Buffer, PIC.FileHeader.bfOffBits, OneThing, PicOut);
		delete PIC.Buffer;
		
		PIC.Buffer = new BYTE[SizeOfBuff];
		
		//Writing BMP to the new file, and hiding the Message in the process;
		while (LinesCounter != PIC.InfoHeader.biHeight)
		{
			fread(PIC.Buffer, SizeOfBuff, OneThing, PIC.PicIn);
			while (BufferOffset < SizeOfBuff)
			{
				if (Message[MessageOffset] != '\0')
				{
					if (j == (MemoryForOneCh - 1))
					{
						MessageOffset++;
						j = 0;
					}
					else
					{
						Symbol = Message[MessageOffset];
						Symbol = Symbol & (Mask << j);
						Symbol = Symbol >> j;
						PIC.Buffer[BufferOffset] = PIC.Buffer[BufferOffset] & (~Mask);
						PIC.Buffer[BufferOffset] = PIC.Buffer[BufferOffset] | Symbol;
						j++;
						BufferOffset++;
					}
				}
				else
				{
					//Writing '\0' in the end of message
					if (JustEndedLine == true)
					{
						if (j == (MemoryForOneCh))
						{
							JustEndedLine = false;
						}
						else
						{
							Symbol = Message[MessageOffset];
							Symbol = Symbol & (Mask << j);
							Symbol = Symbol >> j;
							PIC.Buffer[BufferOffset] = PIC.Buffer[BufferOffset] & (~Mask);
							PIC.Buffer[BufferOffset] = PIC.Buffer[BufferOffset] | Symbol;
							j++;
							BufferOffset++;
						}
					}
					else break;
				}
			}
			LinesCounter++;
			BufferOffset = 0;
			fwrite(PIC.Buffer, SizeOfBuff, OneThing, PicOut);
		}
		
	}
	if (HideOrGet == 0)
	{
		PIC.Buffer = new BYTE[SizeOfBuff];
		fseek(PIC.PicIn, PIC.FileHeader.bfOffBits, SEEK_SET);
		Message[MessageOffset] = Message[MessageOffset] & 0;
		while (LinesCounter != PIC.InfoHeader.biHeight)
		{
			fread(PIC.Buffer, SizeOfBuff, OneThing, PIC.PicIn);
			while (BufferOffset < SizeOfBuff)
			{
				if (j == (MemoryForOneCh - 1))
				{
					if (Message[MessageOffset] == '\0')
						goto TheEndOfCycle;
					MessageOffset++;
					Message[MessageOffset] = Message[MessageOffset] & 0;
					j = 0;
				}
				else
				{
					Symbol = PIC.Buffer[BufferOffset] & Mask;
					Symbol = Symbol << j;
					Message[MessageOffset] = Message[MessageOffset] | Symbol;
					j++;
					BufferOffset++;
				}
			}
			LinesCounter++;
			BufferOffset = 0;
		}
	TheEndOfCycle:
		printf("%s\r\n", Message);
	}
	system("pause");
	return 1;
}
