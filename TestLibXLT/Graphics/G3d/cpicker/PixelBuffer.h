#ifndef __PIXELBUFFER_H
#define __PIXELBUFFER_H

#include <windows.h>

struct PixelBuffer
	{
	public:
		PixelBuffer();
		void Create(int _w, int _h);
		void Destroy();
		void SetPixel(int x, int y, unsigned int color);
		void Display(HDC dc);

	private:
		HBITMAP hBmpBuffer;
		void *lpBits;
		int w,h;
	};

#endif