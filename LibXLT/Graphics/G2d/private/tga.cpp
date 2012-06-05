#include "tga.h"

#include "Core/it/itString.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

inline void writeRGBA (envType::UInt32 *p, envType::UInt8 r, envType::UInt8 g, envType::UInt8 b, envType::UInt8 a)
{
  envType::UInt8 *dp = (envType::UInt8 *)p;
  dp[0] = r;
  dp[1] = g;
  dp[2] = b;
  dp[3] = a;
}

/**
* flip a Uint32 image block vertically
* by allocating a new block, then copying
* the rows in reverse order
*/ 
envType::UInt32* flip(envType::UInt32* in, int w, int h)
{
	int adv, adv2, i;
	envType::UInt32* out;

	out = new envType::UInt32[w * h];
	if(!out)
	{
		return NULL;
	}

	adv = 0; adv2 = w * h;

	for(i = 0; i < h; i++)
	{
		adv2 -= w;
		::memmove(out + adv, in + adv2, w * sizeof(envType::UInt32));
		adv += w;
	}

	delete [] (in);
	return out;
}

/* read or write headers */
/* you may set rgbe_header_info to null if you want to */
int TGA_Read(const itString& filePath, FILE *fp, int *width, int *height, envType::UInt32** o_data)
{
	TgaHeader header; 
	TgaFooter footer; 

	// read the footer first 
	fseek (fp, 0L - TGA_FOOTER_LEN, SEEK_END); 
	if (fread (&footer, TGA_FOOTER_LEN, 1, fp) != 1) 
	{ 
		return 0; 
	} 

	// check the footer to see if we have a v2.0 TGA file 
	bool hasFooter = false;
	if (memcmp(footer.signature, TGA_SIGNATURE, sizeof (footer.signature)) == 0) 
		hasFooter = true; 

	// now read the header 
	if (fseek (fp, 0, SEEK_SET) || fread (&header, sizeof (header), 1, fp) != 1)  
	{ 
		return 0; 
	}

	// skip over alphanumeric ID field 
	if (header.idLength && fseek (fp, header.idLength, SEEK_CUR)) 
	{ 
		return 0; 
	} 

	// now parse the header 

	// endian-safe loading of 16-bit sizes 
	int im_w,im_h;
	im_w = (header.widthHi << 8) | header.widthLo; 
	im_h = (header.heightHi << 8) | header.heightLo;    

	if ((im_w > 32767) || (im_w < 1) || (im_h > 32767) || (im_h < 1))
	{
		im_w = 0;
		return 0;
	}

	*width = im_w;
	*height = im_h;
	*o_data = new envType::UInt32[im_w*im_h];

	// this flag indicated bottom-up pixel storage 
	int vinverted = header.descriptor ^ TGA_DESC_VERTICAL; 

	int rle = 0;
	switch (header.imageType) 
	{ 
	case TGA_TYPE_COLOR_RLE:
	case TGA_TYPE_GRAY_RLE:
		rle = 1; 
		break; 

	case TGA_TYPE_COLOR: 
	case TGA_TYPE_GRAY:
		rle = 0; 
		break; 

	default: 
		return 0; 
	} 

	// bits per pixel 
	int bpp = header.bpp; 

	if( ! ((bpp == 32) || (bpp == 24) || (bpp == 8)) ) 
	{ 
		return 0;
	} 

    // first we read the file data into a buffer for parsing 
    // then we decode from RAM 

    // find out how much data must be read from the file 
    // (this is NOT simply width*height*4, due to compression) 

	struct ::_stat64i32 ss;
	::_wstat(filePath.GetString(), &ss);
	int datasize = ss.st_size - sizeof(TgaHeader) - header.idLength -
		(hasFooter ? TGA_FOOTER_LEN : 0);
	
	unsigned char* buf = new unsigned char[datasize]; 
	if(!buf) 
	{
		im_w = 0;
		return 0;
	}
	
	// read in the pixel data 
	if( fread(buf, 1, datasize, fp) != datasize) 
	{
		delete [] buf;
		return 0;
	}

	// buffer is ready for parsing 

	// bufptr is the next byte to be read from the buffer 
	unsigned char* bufptr = buf; 

	// dataptr is the next 32-bit pixel to be filled in 
	envType::UInt32* dataptr = *o_data;
	
	// decode uncompressed BGRA data 
	if(!rle) 
	{ 
		for(int y = 0; y < im_h; y++) // for each row 
		{ 
			int x;

			// point dataptr at the beginning of the row 
			if(!vinverted)
				// some TGA's are stored upside-down! 
				dataptr = (*o_data) + (im_h - (y+1)) * im_w;
			else
				dataptr = (*o_data) + y * im_w;


			for(x = 0; x < im_w; x++) // for each pixel in the row 
			{
				switch(bpp)
				{

					// 32-bit BGRA pixels 
				case 32: 
					writeRGBA (dataptr, 
						*(bufptr + 2),  // R 
						*(bufptr + 1),  // G 
						*(bufptr + 0),  // B 
						*(bufptr + 3)   // A 
						);
					dataptr++;
					bufptr += 4;
					break;

					// 24-bit BGR pixels 
				case 24: 
					writeRGBA (dataptr,
						*(bufptr + 2),  // R 
						*(bufptr + 1),  // G 
						*(bufptr + 0),  // B 
						(char) 0xff     // A 
						);
					dataptr++;
					bufptr += 3;
					break;

					// 8-bit grayscale 
				case 8:
					writeRGBA (dataptr,
						*bufptr,      // grayscale 
						*bufptr,
						*bufptr,
						(char) 0xff
						);
					dataptr++;
					bufptr += 1;
					break;
				}  

			} // end for (each pixel) 

		} // end for (each row) 

	} // end if (RLE) 

	// decode RLE compressed data 
	else 
	{ 
		unsigned char curbyte, red, green, blue, alpha; 
		envType::UInt32 *final_pixel = dataptr + im_w * im_h; 

		// loop until we've got all the pixels 
		while(dataptr < final_pixel)	
		{ 
			int count; 

			curbyte = *bufptr++; 
			count = (curbyte & 0x7F) + 1; 

			if(curbyte & 0x80)  // RLE packet 
			{ 
				int i;

				switch(bpp)
				{
				case 32:
					blue = *bufptr++; green = *bufptr++; red = *bufptr++; 
					alpha = *bufptr++;
					for(i = 0; i < count; i++)
					{
						writeRGBA (dataptr, red, green, blue, alpha);
						dataptr++;
					}
					break;

				case 24:
					blue = *bufptr++; green = *bufptr++; red = *bufptr++; 
					for(i = 0; i < count; i++)
					{
						writeRGBA (dataptr, red, green, blue, (char) 0xff);
						dataptr++;
					}
					break;

				case 8:
					alpha = *bufptr++;
					for(i = 0; i < count; i++)
					{
						writeRGBA (dataptr, alpha, alpha, alpha, (char) 0xff);
						dataptr++;
					}
					break;
				}

			} // end if (RLE packet) 

			else  // raw packet 
			{ 
				int i;

				for(i = 0; i < count; i++) 
				{ 
					switch(bpp)
					{

						// 32-bit BGRA pixels 
					case 32: 
						writeRGBA (dataptr, 
							*(bufptr + 2),  // R 
							*(bufptr + 1),  // G 
							*(bufptr + 0),  // B 
							*(bufptr + 3)   // A 
							);
						dataptr++;
						bufptr += 4;                
						break;
						// 24-bit BGR pixels 
					case 24: 
						writeRGBA (dataptr,
							*(bufptr + 2),  // R 
							*(bufptr + 1),  // G 
							*(bufptr + 0),  // B 
							(char) 0xff     // A 
							);
						dataptr++;
						bufptr += 3;
						break;

						// 8-bit grayscale 
					case 8:
						writeRGBA (dataptr,
							*bufptr,      // pseudo-grayscale 
							*bufptr,
							*bufptr,
							(char) 0xff
							);
						dataptr++;
						bufptr += 1;
						break;
					}  
				} 
			} // end if (raw packet)  

		} // end for (each packet) 

		// must now flip a bottom-up image 

		/* This is the best of several ugly implementations
		* I considered. It's not very good since the image
		* will be upside-down throughout the loading process.
		* This could be done in-line with the de-RLE code
		* above, but that would be messy to code. There's
		* probably a better way... */

		if(!vinverted)
		{
			(*o_data) = flip((*o_data), im_w, im_h);

			if(!(*o_data))
			{
				delete [] (buf);
				return 0;
			}
		}

	  } // end if (image is RLE)  

	  delete [] buf; 
	  return 1;
}

void TGA_Write_RGBA(FILE *fp, int width, int height, envType::UInt32** data)
{
	TgaHeader tgah;
	memset(&tgah,0,sizeof(TgaHeader));

	tgah.idLength=0;
	tgah.colorMapType=0;
	tgah.imageType=2;
	/*tgah.colorMapIndexLo=0;
	tgah.colorMapIndexHi=0;
	tgah.colorMapLengthLo=0;
	tgah.colorMapLengthHi=0;
	tgah.colorMapSize=0;
	tgah.xOriginLo=0;
	tgah.xOriginHi=0;
	tgah.yOriginLo=0;
	tgah.yOriginHi=0;*/
	tgah.widthLo=(width & 0x00FF);
	tgah.widthHi=(width & 0xFF00) / 256;
	tgah.heightLo=(height & 0x00FF);
	tgah.heightHi=(height & 0xFF00) / 256;
	tgah.bpp=(unsigned char)32;
	tgah.descriptor=0;
	fwrite(&tgah,1,sizeof(TgaHeader),fp);

	long slsize = width * sizeof(envType::UInt32);
	unsigned char *scanline = new unsigned char[slsize];
	unsigned char r,g,b,a;
	int x, y;
	memset(scanline, 0, slsize);
	envType::UInt32* dataptr = *data;
	
	for( y = 0; y < height; y++)
	{
		memcpy(scanline, &dataptr[y * width], slsize);
		for (x = 0; x < width; x++)
		{
			r = scanline[x*4 + 0];
			g = scanline[x*4 + 1];
			b = scanline[x*4 + 2];
			a = scanline[x*4 + 3];
			scanline[x*4 + 0] = b;
			scanline[x*4 + 1] = g;
			scanline[x*4 + 2] = r;
			scanline[x*4 + 3] = a;
		}
		fwrite(scanline, 1, slsize, fp);
	}

	delete [] scanline;
}