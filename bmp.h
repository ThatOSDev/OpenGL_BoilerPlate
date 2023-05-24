
/*!
@author ThatOSDev
@NOTE
#define BMP_IMPLEMENTATION
#include "bmp.h"
*/

#ifndef BMP_H
#define BMP_H

/*! @breif
    This allows you to load a BMP file. It can be 24 or 32 Bits.
	@param[in] This is the path and name of the file to load.
	@param[out] The Width of the image. This is pulled from the image header.
	@param[out] The Height of the image. This is pulled from the image header.
	@param[out] The Bit Depth of the image. This is pulled from the image header.
	@return This will return the data of the image.
*/
unsigned char* LoadBMP(const char* fileName, int* width, int* height, unsigned short* bd);

/*! @breif
    This allows you to generate a BMP file. It can be 24 or 32 Bits.
	@param[in] The user specifies the Width of the image.
	@param[in] The user specifies the Height of the image.
	@param[in] The user specifies the Bit Depth of the image.
	@param[in] Red.
    @param[in] Green.
	@param[in] Blue.
	@return This will return the data of the image.
*/
unsigned char* GenerateBMP(int width, int height, int bits, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

/*! @breif
    This allows you to save a BMP file. It can be 24 or 32 Bits.
	@param[in] This is the path and name of the file to save.
	@param[in] The is the data you want saved with the image.
	@param[in] The user specifies the Width of the image.
	@param[in] The user specifies the Height of the image.
	@param[in] The user specifies the Bit Depth of the image.
*/
void SaveBMP(const char* fileName, unsigned char* data, int width, int height, unsigned short bitDepth);

#endif // BMP_H

#ifdef BMP_IMPLEMENTATION

#include <stdio.h>   // FILE  fclose()  fopen()
#include <stdlib.h>  // malloc

unsigned char* LoadBMP(const char* fileName, int* width, int* height, unsigned short* bd)
{
    FILE* pFile = fopen(fileName, "rb");
    if(pFile)
    {
        unsigned char* data = NULL;
        if(fgetc(pFile) == 'B' && fgetc(pFile) == 'M')
        {
            unsigned char* tempData = NULL;
            unsigned int image_data_address;
            int w, h, bitDepth;

            fseek(pFile, 8, SEEK_CUR);
            fread(&image_data_address, 4, 1, pFile);
            fseek(pFile, 4, SEEK_CUR);
            fread(width, 4, 1, pFile);
            fread(height, 4, 1, pFile);
            fseek(pFile, 2, SEEK_CUR);
            fread(bd, 2, 1, pFile);

            w = *width;
            h = *height;
            bitDepth = *bd;

            int totalBytes = ((w * h) * (bitDepth / 8)) * sizeof(unsigned char);
            tempData = malloc(totalBytes * sizeof(unsigned char));
            fseek(pFile, image_data_address, SEEK_SET);
            int bytes_read = fread(tempData, sizeof(unsigned char), totalBytes, pFile);
            if(tempData != NULL && bytes_read > 0)
            {
                data = calloc(totalBytes, sizeof(unsigned char));
                int c = 0;
                int LoopBytes = 3;
                if(bitDepth == 32){LoopBytes = 4;}
                for(int t = 0; t < totalBytes; t+=LoopBytes) // ARGB  |  Opengl == RGBA
                {
                    data[c] = tempData[t + 2];
                    c++;
                    data[c] = tempData[t + 1];
                    c++;
                    data[c] = tempData[t];
                    c++;
                    if(LoopBytes == 4)
                    {
                        data[c] = tempData[t + 3];
                        c++;
                    }
                }
                free(tempData);
            }
        }
        fclose(pFile);
        return data;
    }

    return 0;
}

unsigned char* GenerateBMP(int width, int height, int bits, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
    unsigned char* data = NULL;
    int loopBits = (bits / 8);
    int totalBytes = ((width * height) * loopBits) * sizeof(unsigned char);
    data = calloc(totalBytes, sizeof(unsigned char));
    for(int t = 0; t < totalBytes;)
    {
        data[t] = red;
        t++;
        data[t] = green;
        t++;
        data[t] = blue;
        t++;
        if(loopBits == 4)
        {
            data[t] = alpha;
            t++;
        }
    }
    return data;
}

void SaveBMP(const char* fileName, unsigned char* data, int width, int height, unsigned short bitDepth)
{
    int HeaderSize = 54;
    int bmpInfoSize = 40;
    if(bitDepth == 32){HeaderSize = 138; bmpInfoSize = 124;}
    int dSize = ((width * height) * (bitDepth / 8));
    int tSize = HeaderSize + dSize;
    int ppm = 2835;
    unsigned char bmpHeader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, HeaderSize, 0, 0, 0};
    unsigned char bmpInfo[124] = {bmpInfoSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, bitDepth, 0};

    bmpHeader[2] = (unsigned char)(tSize);
    bmpHeader[3] = (unsigned char)(tSize >> 8);
    bmpHeader[4] = (unsigned char)(tSize >> 16);
    bmpHeader[5] = (unsigned char)(tSize >> 24);

    bmpInfo[4]  = (unsigned char)(width);
    bmpInfo[5]  = (unsigned char)(width >> 8);
    bmpInfo[6]  = (unsigned char)(width >> 16);
    bmpInfo[7]  = (unsigned char)(width >> 24);

    bmpInfo[8]  = (unsigned char)(height);
    bmpInfo[9]  = (unsigned char)(height >> 8);
    bmpInfo[10] = (unsigned char)(height >> 16);
    bmpInfo[11] = (unsigned char)(height >> 24);

    bmpInfo[20] = (unsigned char)(dSize);
    bmpInfo[21] = (unsigned char)(dSize >> 8);
    bmpInfo[22] = (unsigned char)(dSize >> 16);
    bmpInfo[23] = (unsigned char)(dSize >> 24);

    int LoopBytes = 3;
    if(bitDepth == 32){LoopBytes = 4;}
    if(LoopBytes == 4)
    {
        bmpInfo[16] = 0x3;

        bmpInfo[24] = (unsigned char)(ppm);
        bmpInfo[25] = (unsigned char)(ppm >> 8);
        bmpInfo[26] = (unsigned char)(ppm >> 16);
        bmpInfo[27] = (unsigned char)(ppm >> 24);

        bmpInfo[28] = (unsigned char)(ppm);
        bmpInfo[29] = (unsigned char)(ppm >> 8);
        bmpInfo[30] = (unsigned char)(ppm >> 16);
        bmpInfo[31] = (unsigned char)(ppm >> 24);

        bmpInfo[40] = 0x0;
        bmpInfo[41] = 0x0;
        bmpInfo[42] = 0xff;
        bmpInfo[43] = 0x0;

        bmpInfo[44] = 0x0;
        bmpInfo[45] = 0xff;
        bmpInfo[46] = 0x0;
        bmpInfo[47] = 0x0;

        bmpInfo[48] = 0xff;
        bmpInfo[49] = 0x0;
        bmpInfo[50] = 0x0;
        bmpInfo[51] = 0x0;

        bmpInfo[52] = 0x0;
        bmpInfo[53] = 0x0;
        bmpInfo[54] = 0x0;
        bmpInfo[55] = 0xff;

        bmpInfo[56] = 0x42;
        bmpInfo[57] = 0x47;
        bmpInfo[58] = 0x52;
        bmpInfo[59] = 0x73;
    }

    unsigned char* tempChar = malloc(dSize * sizeof(unsigned char));
    int c = 0;
    for(int t = 0; t < dSize; t+=LoopBytes)
    {
        tempChar[c] = data[t + 2];
        c++;
        tempChar[c] = data[t + 1];
        c++;
        tempChar[c] = data[t];
        c++;
        if(LoopBytes == 4)
        {
            tempChar[c] = data[t + 3];
            c++;
        }
    }

    FILE* pFile;
    pFile = fopen(fileName, "wb");
    fwrite(bmpHeader, 1, 14, pFile);
    if(bitDepth == 24){bmpInfoSize = 40;}
    fwrite(bmpInfo, 1, bmpInfoSize, pFile);
    fwrite(tempChar, 1, dSize, pFile);
    fclose(pFile);
    free(tempChar);
}

#endif // BMP_IMPLEMENTATION

/*
BMP HEADER -- 14 Bytes Total
-----------------------------
2 bytes - BM
4 bytes - The size of the BMP file in bytes
2 bytes - if created manually can be 0
2 bytes - if created manually can be 0
4 bytes - The offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found.

40 Byte -- BMP INFO
--------------------
4 	the size of this header, in bytes (40)
4 	the bitmap width in pixels (signed integer)
4 	the bitmap height in pixels (signed integer)
2 	the number of color planes (must be 1)
2 	the number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
4 	the compression method being used. See the next table for a list of possible values
4 	the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
4 	the horizontal resolution of the image. (pixel per metre, signed integer)
4 	the vertical resolution of the image. (pixel per metre, signed integer)
4 	the number of colors in the color palette, or 0 to default to 2n
4 	the number of important colors used, or 0 when every color is important; generally ignored

124 Byte -- BMP INFO
--------------------
4 	28 00 00 00 	40 bytes 	Number of bytes in the DIB header (from this point)
4 	02 00 00 00 	2 pixels (left to right order) 	Width of the bitmap in pixels
4 	02 00 00 00 	2 pixels (bottom to top order) 	Height of the bitmap in pixels. Positive for bottom to top pixel order.
2 	01 00 	1 plane 	Number of color planes being used
2 	18 00 	24 bits 	Number of bits per pixel
4 	00 00 00 00 	0 	BI_RGB, no pixel array compression used
4 	10 00 00 00 	16 bytes 	Size of the raw bitmap data (including padding)
4 	13 0B 00 00 	2835 pixels/metre horizontal 	Print resolution of the image,
4 	13 0B 00 00 	2835 pixels/metre vertical      72 * 39.375
4 	00 00 00 00 	0 colors 	Number of colors in the palette
4 	00 00 00 00 	0 important colors 	0 means all colors are important
4 	00 00 00 00 	0 colors 	Number of colors in the palette
4 	00 00 00 00 	0 important colors 	0 means all colors are important
4 	00 00 FF 00 	00FF0000 in big-endian 	Red channel bit mask (valid because BI_BITFIELDS is specified)
4 	00 FF 00 00 	0000FF00 in big-endian 	Green channel bit mask (valid because BI_BITFIELDS is specified)
4 	FF 00 00 00 	000000FF in big-endian 	Blue channel bit mask (valid because BI_BITFIELDS is specified)
4 	00 00 00 FF 	FF000000 in big-endian 	Alpha channel bit mask
4 	20 6E 69 57 	little-endian "Win " 	LCS_WINDOWS_COLOR_SPACE
*/


