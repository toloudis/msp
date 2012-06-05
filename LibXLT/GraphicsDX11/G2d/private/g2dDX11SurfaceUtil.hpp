/****************************************************************************\
**  g2dDX11SurfaceUtil.hpp
**
**      g2dDX11SurfaceUtil.hpp is a collection of routines to extract data
**		from D3D surfaces
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_D3D11SURFACEUTIL_HPP
#error g2dDX11SurfaceUtil.hpp multiply included
#endif
#define G2D_D3D11SURFACEUTIL_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

#include <string>

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

		void GetValue(	envType::UInt8 &o_Red,
						envType::UInt8 &o_Green,
						envType::UInt8 &o_Blue,
						envType::UInt8 &o_Alpha) const;
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
		envType::UInt8* m_pBase;
	protected:
		envType::UInt8* m_pData;
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
		SurfaceLocker(g2dD3D11TexturePtr i_pSurface, bool i_NeedWriteAccess = false);
		~SurfaceLocker();

		bool IsLocked() { return (m_pSurface != NULL); }

		SurfaceIterator* GetIterator() {return m_pIterator;}

	private:

		g2dD3D11TexturePtr m_pSurface;

		SurfaceIterator* m_pIterator;
		SurfaceIterator* CreateIterator(D3D11_MAPPED_SUBRESOURCE& i_LockInfo);
};

namespace g2dDX11SurfaceUtil
{
	// allocate buffer and copy pixels into it. caller must delete [] buffer.
	void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
		envType::UInt8 **o_pBuffer,
		int *o_bufSize,
		int *o_width,
		int *o_height,
		int *o_bpp,
		int i_sampling = 1,
		bool i_bGreyscale = false,
		RECT* i_pRect = NULL);

	void GetSurfacePixelsTGA(g2dD3D11TexturePtr i_pSurface,
		envType::UInt8 **o_pBuffer,
		int *o_bufSize,
		int *o_width,
		int *o_height,
		int *o_bpp,
		int	i_Sampling = 1,
		const std::string& i_CompressionCode = std::string("None"),
		const RECT* i_pRect = NULL);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
		g2dPixelR16G16B16A16** o_pBuffer,
		int* o_width,
		int* o_height);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
		g2dPixelR8G8B8A8** o_pBuffer,
		int* o_width,
		int* o_height);

	// straight copy, no downsampling. caller must delete.
	void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
		g2dPixelRGBA32F** o_pBuffer,
		int* o_width,
		int* o_height);

	// just fill the passed in buffer.
	void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
		g2dPixelRGBA32F* o_pBuffer,
		int i_Width,
		int i_Height);

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the surface
	//----------------------------------------------------------------------------
	void GetPixelColor(g2dD3D11TexturePtr i_pSurface,
							int i_X,
							int i_Y,
							envType::UInt8 &o_Red,
							envType::UInt8 &o_Green,
							envType::UInt8 &o_Blue,
							envType::UInt8 &o_Alpha);

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the surface
	//----------------------------------------------------------------------------
	void GetPixelColor(g2dD3D11TexturePtr i_pSurface,
							int i_X,
							int i_Y,
							envType::Float32 &o_Red,
							envType::Float32 &o_Green,
							envType::Float32 &o_Blue,
							envType::Float32 &o_Alpha);
} // namespace g2dDX11SurfaceUtil

