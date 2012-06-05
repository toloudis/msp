/****************************************************************************\
**  matUVATexture.cpp
**
**      matUVATexture is a matTexture which has a series of frames that
**	cycle over time.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//	This constructor will not typically be used by the mat client,
//	since textures are loaded and managed by the matTextureMgr.
//--------------------------------------------------------------------
matUVATexture::matUVATexture()
:	m_WidthFrames(1),
	m_HeightFrames(1),
	m_StartTime(0.0f),
	m_Rate(g3dConstants::c_fDefaultFrameRate),
	m_NumFrames(1),
	m_bLooping(true),
	m_bReversing(false),
	m_Anim(NULL)
{
	//	Normally, we might set a PAC here but the UVA texture has no
	//	PAC, so it will just be NULL.
	this->remake_anim();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matUVATexture::~matUVATexture()
{
	delete m_Anim;
}

//--------------------------------------------------------------------
//	AddTexturePage causes the given matTexture to be added to the
//	matUVATexture.  The matUVATexture will not delete the texture
//	page.
//--------------------------------------------------------------------
void matUVATexture::AddTexturePage(matTexture* i_Texture)
{
	if( m_Pages.size() == 0 )
	{
		this->SetHeight(i_Texture->GetHeight());
		this->SetWidth(i_Texture->GetWidth());
		this->SetPixelFormat(i_Texture->GetPixelFormat());
	}

	m_Pages.push_back(i_Texture);
}

//--------------------------------------------------------------------
//	Remove (not delete) all the texture pages for this UVA
//--------------------------------------------------------------------
void matUVATexture::RemoveAllTexturePages()
{
	m_Pages.clear();
}

//--------------------------------------------------------------------
//	GetNumPages returns the number of texture pages.
//--------------------------------------------------------------------
int matUVATexture::GetNumPages() const
{
	return m_Pages.size();
}

//--------------------------------------------------------------------
//	GetTexturePage returns the requested texture page.
//--------------------------------------------------------------------
matTexture* matUVATexture::GetPage(int i_Num)
{
	DBG_ASSERT( (i_Num >= 0) && (i_Num < m_Pages.size()), "Index out of range");
	if (i_Num < 0 || i_Num >= m_Pages.size())
		return NULL;
	return m_Pages[i_Num];
}

const matTexture* matUVATexture::GetPage(int i_Num) const
{
	DBG_ASSERT( (i_Num >= 0) && (i_Num < m_Pages.size()), "Index out of range");
	if (i_Num < 0 || i_Num >= m_Pages.size())
		return NULL;
	return m_Pages[i_Num];
}

//--------------------------------------------------------------------
//	SetWidthFrames and SetHeightFrames changes the number of
//	divisions of the pages into frames.  The maximum number of frames
//	per page is the number of width frames multiplied by the number
//	of height frames.  The default is one for each.
//--------------------------------------------------------------------
void matUVATexture::SetNumWidthFrames(int i_Num)
{
	DBG_ASSERT( i_Num > 0, "Negative NumWidthFrames");
	if (i_Num <= 0)
		return;
	m_WidthFrames = i_Num;
}

void matUVATexture::SetNumHeightFrames(int i_Num)
{
	DBG_ASSERT( i_Num > 0, "Negative NumHeightFrames");
	if (i_Num <= 0)
		return;
	m_HeightFrames = i_Num;
}

//--------------------------------------------------------------------
//	GetNumWidthFrames and GetNumHeightFrames return the number of
//	frames across and down, respectively, in a texture page.
//--------------------------------------------------------------------
int matUVATexture::GetNumWidthFrames() const
{
	return m_WidthFrames;
}

int matUVATexture::GetNumHeightFrames() const
{
	return m_HeightFrames;
}

//--------------------------------------------------------------------
//	SetNumFrames sets the number of frames in the animation.  This
//	is included in case someone doesn't want to use the entire texture
//	page for a UVA.  The UVA will only cycle through the given number
//	of frames.
//--------------------------------------------------------------------
void matUVATexture::SetNumFrames(int i_Num)
{
	DBG_ASSERT( i_Num > 0, "Negative or zero number of frames");
	if (i_Num <= 0)
		return;
	m_NumFrames = i_Num;
	this->remake_anim();
}

//--------------------------------------------------------------------
//	GetNumFrames returns the number of frames in the UVA.  Contrast
//	with GetNumTexturePages.
//--------------------------------------------------------------------
int matUVATexture::GetNumFrames() const
{
	return m_NumFrames;
}

//--------------------------------------------------------------------
//	Set/GetLooping changes the looping characteristic of the UVA.
//	The default is true.
//--------------------------------------------------------------------
void matUVATexture::SetLooping(bool i_bLooping)
{
	if( this->GetLooping() != i_bLooping )
	{
		m_bLooping = i_bLooping;
		m_Anim->SetLooping(i_bLooping);
	}
}

bool matUVATexture::GetLooping() const
{
	return m_bLooping;
}

//--------------------------------------------------------------------
//	Set/GetReversing changes the reversing characteristic of the UVA.
//	The default is false.
//--------------------------------------------------------------------
void matUVATexture::SetReversing(bool i_bReversing)
{
	if( this->GetReversing() != i_bReversing )
	{
		m_bReversing = i_bReversing;
		m_Anim->SetReversing(i_bReversing);
	}
}

bool matUVATexture::GetReversing() const
{
	return m_bReversing;
}

//--------------------------------------------------------------------
//	SetStartTime sets the time at which the UVA starts its frame
//	cycling animation.  The default is zero.
//--------------------------------------------------------------------
void matUVATexture::SetStartTime(float i_Time)
{
	m_StartTime = i_Time;
}

//--------------------------------------------------------------------
//	SetFrameRate sets the rate at which the UVA changes frames.  The
//	default is g3dConstants::c_fDefaultFrameRate
//--------------------------------------------------------------------
void matUVATexture::SetFrameRate(float i_FPS)
{
	m_Rate = i_FPS;
	this->remake_anim();
}

//--------------------------------------------------------------------
//	GetFrameRate returns the frame rate of the UVA.
//--------------------------------------------------------------------
float matUVATexture::GetFrameRate() const
{
	return m_Rate;
}

//--------------------------------------------------------------------
//	GetLength returns the time length of the UVA anim.  This depends
//	on the frame rate and the number of frames.
//--------------------------------------------------------------------
float matUVATexture::GetLength() const
{
	return float(m_NumFrames) / m_Rate;
}

//--------------------------------------------------------------------
//	GetFrameAtTime returns the frame number that should be played
//	at the given time.
//--------------------------------------------------------------------
int matUVATexture::GetFrameAtTime(float i_Time) const
{
	float local_time = i_Time - m_StartTime;
	float cont_frame_num = m_Anim->GetValue(local_time);
	int frame_num = int(cont_frame_num);

	if( frame_num >= m_NumFrames )
		frame_num = m_NumFrames - 1;
	else if( frame_num < 0 )
		frame_num = 0;

	return frame_num;
}

//--------------------------------------------------------------------
//	GetPage(Number)ForFrame returns the texture page that contains
//	the given frame.
//--------------------------------------------------------------------
matTexture* matUVATexture::GetPageForFrame(int i_Frame) const
{
	return m_Pages[this->GetPageNumberForFrame(i_Frame)];
}

int matUVATexture::GetPageNumberForFrame(int i_Frame) const
{
	DBG_ASSERT(i_Frame >= 0, "Negative Frame in matUVATexture::GetPageForFrame");
	if (i_Frame < 0)
		i_Frame = 0;
	DBG_ASSERT(i_Frame < m_NumFrames, "Frame too large in matUVATexture::GetPageForFrame");
	if (i_Frame >= m_NumFrames)
		i_Frame = m_NumFrames;
	int frames_per_page = m_WidthFrames * m_HeightFrames;
	int page_num = i_Frame / frames_per_page;
	DBG_ASSERT(page_num < m_Pages.size(), "Not enough pages for frame");
	if (page_num >= m_Pages.size())
		page_num = 0;

	return page_num;
}

//--------------------------------------------------------------------
//	GetUVForFrame returns the UV coordinate at the upper left of the
//	requested frame.
//--------------------------------------------------------------------
void matUVATexture::GetUVForFrame(int i_Frame, float& o_U, float& o_V) const
{
	int page_frame = i_Frame % (m_WidthFrames * m_HeightFrames);
	int page_frame_y = page_frame / m_WidthFrames;
	int page_frame_x = page_frame - (page_frame_y * m_WidthFrames);
	o_U = float(page_frame_x) / float(m_WidthFrames);
	o_V = float(page_frame_y) / float(m_HeightFrames);
}

//--------------------------------------------------------------------
//	GetFrameWidth and GetFrameHeight return the size of a frame
//	in texture coordinates ([0..1]).
//--------------------------------------------------------------------
float matUVATexture::GetFrameWidth() const
{
	return 1.0f / float( m_WidthFrames );
}

float matUVATexture::GetFrameHeight() const
{
	return 1.0f / float( m_HeightFrames );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matUVATexture::remake_anim()
{
	delete m_Anim;
	m_Anim = new an2StateAnimation<float>(0.0f, float(m_NumFrames), float(m_NumFrames) / m_Rate);
	m_Anim->SetLooping(this->GetLooping());
	m_Anim->SetReversing(this->GetReversing());
}

//--------------------------------------------------------------------
//	MakeTransparencyMap will build a transparency map for this texture
//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
//--------------------------------------------------------------------
char * matUVATexture::MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height)
{
	// can't so it for this type because the textures change.
	// maybe this should be based on first texture?
	return NULL;
}

//--------------------------------------------------------------------
// Replace all data members except for the texture pages. This is sort
// of like a copy constructor. If new members are added, this should
// account for it.
//--------------------------------------------------------------------
void matUVATexture::CopyData(const matUVATexture* i_pSrcUVATexture)
{
	m_WidthFrames	= i_pSrcUVATexture->m_WidthFrames ;
	m_HeightFrames	= i_pSrcUVATexture->m_HeightFrames ;
	m_NumFrames		= i_pSrcUVATexture->m_NumFrames ;
	m_StartTime		= i_pSrcUVATexture->m_StartTime ;
	m_Rate			= i_pSrcUVATexture->m_Rate ;
	m_bLooping		= i_pSrcUVATexture->m_bLooping ;
	m_bReversing	= i_pSrcUVATexture->m_bReversing ;
	remake_anim();
}
