/****************************************************************************\
**  g2dSurfaceUtilVk.hpp
**
**      GraphicsDX11's g2dDX11SurfaceUtil on g2dImageVk's CPU pixels: the
**	surface iterators (a pixel as bytes or floats, by the image's DXGI
**	format, as DX11 reads a mapped staging texture) and the routines that
**	extract an image's pixels. Only the lock and the format lookup differ.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DSURFACEUTILVK_HPP
#error g2dSurfaceUtilVk.hpp multiply included
#endif
#define GVK_G2DSURFACEUTILVK_HPP

#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif

#include <cstdint>
#include <string>

class maIntRect;

namespace gvk
{
class g2dImageVk;


//
//	SurfaceIterator
//

//----------------------------------------------------------------------------
//	SurfaceIterator - pointer to data within surface, gets 24-bit color
//    from different pixel formats
//----------------------------------------------------------------------------
class SurfaceIterator
{
	public:

		// After surface is locked, pass in surface descriptor
		// to iterator's constructor. If non-NULL, RECT
		// pointer defines sub region within surface.
		SurfaceIterator(	int i_Width,
							int i_Height,
							int i_Pitch,
							void* i_pData,
							const g2dPFD& i_PFD);
							//RECT *i_pRect = NULL );

		void GetValue(	uint8_t &o_Red,
						uint8_t &o_Green,
						uint8_t &o_Blue,
						uint8_t &o_Alpha) const;
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void GetValue(	float &o_Red,
			float &o_Green,
			float &o_Blue,
			float &o_Alpha) const;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void SetValue(	float i_Red,
			float i_Green,
			float i_Blue,
			float i_Alpha) const;

		void SetPosition( int i_Row, int i_Col );

		void IncRow()	{ m_pData += m_Pitch; }
		void DecRow()	{ m_pData -= m_Pitch; }
		void IncCol()	{ m_pData += m_RGBA_bytes; }

		int GetWidth() const
		{
			//return (m_pRect) ? (m_pRect->right - m_pRect->left) : m_Width;
			return m_Width;
		}
		int GetHeight() const
		{
			//return (m_pRect) ? (m_pRect->bottom - m_pRect->top) : m_Height;
			return m_Height;
		}
		int GetBytesPerPixel() const
		{
			return m_RGBA_bytes;
		}

	private:

		int m_Width;
		int m_Height;
		int m_Pitch;
		int m_RGBA_bytes;		// number of bytes for one pixel
		const g2dPFD m_PFD;
		//RECT *m_pRect;
		uint8_t* m_pBase;
	protected:
		uint8_t* m_pData;
};

class SurfaceIteratorRGBA32F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorRGBA32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorRG32F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorRG32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorR32F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorR32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorRGBA16F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorRGBA16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorRG16F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorRG16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorR16F : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorR16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};
class SurfaceIteratorRGBA8U : public SurfaceIterator
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	SurfaceIteratorRGBA8U(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void GetValue(float &o_Red, float &o_Green, float &o_Blue, float &o_Alpha) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void SetValue(float i_Red, float i_Green, float i_Blue, float i_Alpha) const;
};

//----------------------------------------------------------------------------
//	SurfaceLocker - locks surface on constructor, unlocks on destructor
//----------------------------------------------------------------------------
class SurfaceLocker
{
	public:
		SurfaceLocker(const g2dImageVk* i_pSurface, bool i_NeedWriteAccess = false);
		~SurfaceLocker();

		bool IsLocked() { return (m_pSurface != NULL); }

		SurfaceIterator* GetIterator() {return m_pIterator;}

	private:

		const g2dImageVk* m_pSurface;

		SurfaceIterator* m_pIterator;
		SurfaceIterator* CreateIterator();
};

namespace g2dSurfaceUtilVk
{
	// allocate buffer and copy pixels into it. caller must delete [] buffer.
	void GetSurfacePixels(const g2dImageVk* i_pSurface,
		uint8_t **o_pBuffer,
		int *o_bufSize,
		int *o_width,
		int *o_height,
		int *o_bpp,
		int i_sampling = 1,
		bool i_bGreyscale = false,
		const maIntRect* i_pRect = NULL);

	void GetSurfacePixelsTGA(const g2dImageVk* i_pSurface,
		uint8_t **o_pBuffer,
		int *o_bufSize,
		int *o_width,
		int *o_height,
		int *o_bpp,
		int	i_Sampling = 1,
		const std::string& i_CompressionCode = std::string("None"),
		const maIntRect* i_pRect = NULL);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(const g2dImageVk* i_pSurface,
		g2dPixelR16G16B16A16** o_pBuffer,
		int* o_width,
		int* o_height);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(const g2dImageVk* i_pSurface,
		g2dPixelR8G8B8A8** o_pBuffer,
		int* o_width,
		int* o_height);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(const g2dImageVk* i_pSurface,
		g2dPixelRGBA32F** o_pBuffer,
		int* o_width,
		int* o_height);

	// just fill the passed in buffer.
	void GetSurfacePixels(const g2dImageVk* i_pSurface,
		g2dPixelRGBA32F* o_pBuffer,
		int i_Width,
		int i_Height);

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the surface
	//----------------------------------------------------------------------------
	void GetPixelColor(const g2dImageVk* i_pSurface,
							int i_X,
							int i_Y,
							uint8_t &o_Red,
							uint8_t &o_Green,
							uint8_t &o_Blue,
							uint8_t &o_Alpha);

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the surface
	//----------------------------------------------------------------------------
	void GetPixelColor(const g2dImageVk* i_pSurface,
							int i_X,
							int i_Y,
							float &o_Red,
							float &o_Green,
							float &o_Blue,
							float &o_Alpha);
} // namespace g2dSurfaceUtilVk

} // namespace gvk
