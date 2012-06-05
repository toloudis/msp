/****************************************************************************\
**  matUVATexture.hpp
**
**      matUVATexture is a matTexture which has mip-map levels.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_UVATEXTURE_HPP
#error matUVATexture.hpp multiply included
#endif
#define MAT_UVATEXTURE_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif
#ifndef AN_2STATEANIMATION_HPP
#include "Graphics/an/an2StateAnimation.hpp"
#endif

#include <memory>
#include <vector>

class matUVATexture : public matTexture
{
	public:

		//--------------------------------------------------------------------
		//	This constructor will not typically be used by the mat client,
		//	since textures are loaded and managed by the matTextureMgr.
		//--------------------------------------------------------------------
		matUVATexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matUVATexture();

		//--------------------------------------------------------------------
		// Replace all data members except for the texture pages. This is sort
		// of like a copy constructor. If new members are added, this should
		// account for it.
		//--------------------------------------------------------------------
		void CopyData(const matUVATexture* i_pSrcUVATexture);

		//--------------------------------------------------------------------
		//	AddTexturePage causes the given matTexture to be added to the
		//	matUVATexture.  The matUVATexture will not delete the texture
		//	page.
		//--------------------------------------------------------------------
		void AddTexturePage(matTexture* i_Texture);

		//--------------------------------------------------------------------
		//	Remove (not delete) all the texture pages for this UVA
		//--------------------------------------------------------------------
		void RemoveAllTexturePages();

		//--------------------------------------------------------------------
		//	GetNumPages returns the number of texture pages.
		//--------------------------------------------------------------------
		int GetNumPages() const;

		//--------------------------------------------------------------------
		//	GetTexturePage returns the requested texture page.
		//--------------------------------------------------------------------
		matTexture* GetPage(int i_Num);
		const matTexture* GetPage(int i_Num) const;

		//--------------------------------------------------------------------
		//	SetWidthFrames and SetHeightFrames changes the number of
		//	divisions of the pages into frames.  The maximum number of frames
		//	per page is the number of width frames multiplied by the number
		//	of height frames.  The default is one for each.
		//--------------------------------------------------------------------
		void SetNumWidthFrames(int i_Num);
		void SetNumHeightFrames(int i_Num);

		//--------------------------------------------------------------------
		//	GetNumWidthFrames and GetNumHeightFrames return the number of
		//	frames across and down, respectively, in a texture page.
		//--------------------------------------------------------------------
		int GetNumWidthFrames() const;
		int GetNumHeightFrames() const;

		//--------------------------------------------------------------------
		//	SetNumFrames sets the number of frames in the animation.  This
		//	is included in case someone doesn't want to use the entire texture
		//	page for a UVA.  The UVA will only cycle through the given number
		//	of frames.
		//--------------------------------------------------------------------
		void SetNumFrames(int i_Num);

		//--------------------------------------------------------------------
		//	GetNumFrames returns the number of frames in the UVA.  Contrast
		//	with GetNumTexturePages.
		//--------------------------------------------------------------------
		int GetNumFrames() const;

		//--------------------------------------------------------------------
		//	Set/GetLooping changes the looping characteristic of the UVA.
		//	The default is true.
		//--------------------------------------------------------------------
		void SetLooping(bool i_bLooping);
		bool GetLooping() const;

		//--------------------------------------------------------------------
		//	Set/GetReversing changes the reversing characteristic of the UVA.
		//	The default is false.
		//--------------------------------------------------------------------
		void SetReversing(bool i_bReversing);
		bool GetReversing() const;

		//--------------------------------------------------------------------
		//	SetStartTime sets the time at which the UVA starts its frame
		//	cycling animation.  The default is zero.
		//--------------------------------------------------------------------
		void SetStartTime(float i_Time);

		//--------------------------------------------------------------------
		//	SetFrameRate sets the rate at which the UVA changes frames.  The
		//	default is g3dConstants::c_fDefaultFrameRate
		//--------------------------------------------------------------------
		void SetFrameRate(float i_FPS);

		//--------------------------------------------------------------------
		//	GetFrameRate returns the frame rate of the UVA.
		//--------------------------------------------------------------------
		float GetFrameRate() const;

		//--------------------------------------------------------------------
		//	GetLength returns the time length of the UVA anim.  This depends
		//	on the frame rate and the number of frames.
		//--------------------------------------------------------------------
		float GetLength() const;

		//--------------------------------------------------------------------
		//	GetFrameAtTime returns the frame number that should be played
		//	at the given time.
		//--------------------------------------------------------------------
		int GetFrameAtTime(float i_Time) const;

		//--------------------------------------------------------------------
		//	GetPage(Number)ForFrame returns the texture page that
		//	contains the given frame.
		//--------------------------------------------------------------------
		matTexture* GetPageForFrame(int i_Frame) const;
		int GetPageNumberForFrame(int i_Frame) const;

		//--------------------------------------------------------------------
		//	GetUVForFrame returns the UV coordinate at the upper left of the
		//	requested frame.
		//--------------------------------------------------------------------
		void GetUVForFrame(int i_Frame, float& o_U, float& o_V) const;

		//--------------------------------------------------------------------
		//	GetFrameWidth and GetFrameHeight return the size of a frame
		//	in texture coordinates ([0..1]).
		//--------------------------------------------------------------------
		float GetFrameWidth() const;
		float GetFrameHeight() const;

	protected:
		//--------------------------------------------------------------------
		//	MakeTransparencyMap will build a transparency map for this texture
		//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
		//--------------------------------------------------------------------
		char * MakeTransparencyMap(float i_Bias,
									const g2dPFD& i_PFD,
									int i_Width,
									int i_Height);
	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void remake_anim();

	private:
		//--------------------------------------------------------------------
		//	data
		//--------------------------------------------------------------------
		std::vector<matTexture*> m_Pages;
		an2StateAnimation<float>* m_Anim;
		int		m_WidthFrames;
		int		m_HeightFrames;
		int		m_NumFrames;
		float	m_StartTime;
		float	m_Rate;
		bool	m_bLooping;
		bool	m_bReversing;
};


