#ifndef _H_TGA
#define _H_TGA

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <stdio.h>
#include <iosfwd>
#include <string>



// TGA pixel formats
#define TGA_TYPE_MAPPED      1
#define TGA_TYPE_COLOR       2 
#define TGA_TYPE_GRAY        3 
#define TGA_TYPE_MAPPED_RLE  9 
#define TGA_TYPE_COLOR_RLE  10 
#define TGA_TYPE_GRAY_RLE   11

// TGA header flags
#define TGA_DESC_ABITS      0x0f 
#define TGA_DESC_HORIZONTAL 0x10 
#define TGA_DESC_VERTICAL   0x20 

#define TGA_SIGNATURE "TRUEVISION-XFILE" 
#define TGA_FOOTER_LEN 26

class TgaHeader
{
  public:

    unsigned char idLength; 
    unsigned char colorMapType; 
    unsigned char imageType; 
    unsigned char colorMapIndexLo, colorMapIndexHi; 
    unsigned char colorMapLengthLo, colorMapLengthHi; 
    unsigned char colorMapSize; 
    unsigned char xOriginLo, xOriginHi; 
    unsigned char yOriginLo, yOriginHi; 
    unsigned char widthLo, widthHi; 
    unsigned char heightLo, heightHi; 
    unsigned char bpp; 
    unsigned char descriptor; 
};

class TgaFooter
{
  public:

    unsigned int extensionAreaOffset;
    unsigned int developerDirectoryOffset;
    char signature[16]; 
    char dot; 
    char null; 
};

/* read or write pixels */
/* can read or write pixels in chunks of any size including single pixels*/
int TGA_Read(const std::string& filePath, FILE *fp, int *width, int *height, uint32_t** data);

/* write 32bits rgba uncompressed tga file */
/* input data is a 32bits rgba array */
void TGA_Write_RGBA(std::ostream &os, int width, int height, uint32_t** data);

#endif /* _H_TGA */



