/*****************************************************************************
**  prtParticleGeneratorTemplate.hpp
**
**      A prtParticleGeneratorTemplate provides all the information needed
**	to create a particle generator.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtParticleGeneratorTemplate.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/prt/prtBlockEmitter.hpp"
#include "Graphics/prt/prtCircleEmitter.hpp"
#include "Graphics/prt/prtPointEmitter.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
prtParticleGeneratorTemplate::prtParticleGeneratorTemplate()
:	m_Type(e_Static),
	m_EmitterType(e_Point),
	m_EmitterScale(1, 1, 1),
	m_RenderMode(prtSpriteGroupParticleGenerator::e_Multiplicative),
	m_UVAMode(prtSpriteGroupParticleGenerator::e_ScaleToLifetime),
	m_ScaleMode(prtSpriteGroupParticleGenerator::e_Exponential),
	m_pAlphaAnim(NULL),
	m_pTexture(NULL),
	m_bRenderStreaks(false)
{
	m_TextureLocator.Clear();
	m_GeometryLocator.Clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtParticleGeneratorTemplate::~prtParticleGeneratorTemplate()
{
	delete m_pAlphaAnim;

	if( m_pTexture )
	{
		matUVATexture* pOrigUVATexture = dynamic_cast<matUVATexture*>(m_pTexture);
		if (pOrigUVATexture)
		{
			int n = pOrigUVATexture->GetNumPages();
			for (int i = 0; i < n; i++)
			{
				matTextureMgr::ReleaseTexture(pOrigUVATexture->GetPage(i));
			}
			pOrigUVATexture->RemoveAllTexturePages();
		}
		matTextureMgr::ReleaseTexture(m_pTexture);
	}
}

//--------------------------------------------------------------------
//	These functions set the other "Parameters" in the particle
//	generator.
//--------------------------------------------------------------------
void prtParticleGeneratorTemplate::SetNumParameters(int i_Num)
{
	m_Parameters.resize(i_Num);
}

int prtParticleGeneratorTemplate::GetNumParameters() const
{
	return m_Parameters.size();
}

float prtParticleGeneratorTemplate::GetParameter(int i_Num) const
{
	DBG_ASSERT( (i_Num >= 0) && (i_Num < m_Parameters.size()), "Invalid index " << i_Num << " < " << m_Parameters.size() );
	if (i_Num < 0 || i_Num >= m_Parameters.size())
		return 0.0f;
	return m_Parameters[i_Num];
}

void prtParticleGeneratorTemplate::SetParameter(int i_Num, float i_Val)
{
	DBG_ASSERT( (i_Num >= 0) && (i_Num < m_Parameters.size()), "Invalid index " << i_Num << " < " << m_Parameters.size() );
	if (i_Num < 0 || i_Num >= m_Parameters.size())
		return;
	m_Parameters[i_Num] = i_Val;
}

void prtParticleGeneratorTemplate::SetAlphaAnimation(const anTypedAnimation<float>& i_Anim)
{
	delete m_pAlphaAnim;
	m_pAlphaAnim = dynamic_cast<anTypedAnimation<float>*>(i_Anim.Clone());
}

//--------------------------------------------------------------------
//	GetTexture returns the texture used by the particle generator.
//	This could be NULL, if MakeTexture has not been called to make
//	a texture from the given locator.
//--------------------------------------------------------------------
matTexture* prtParticleGeneratorTemplate::GetTexture() const
{
	return m_pTexture;
}

//--------------------------------------------------------------------
//	MakeTexture loads a texture from the texture locator.
//--------------------------------------------------------------------
void prtParticleGeneratorTemplate::MakeTexture()
{
	if ( m_Type == e_Static3D || m_Type == e_Cone3D	|| m_Type == e_Spiral3D )
		return;

	matTexture* pNewTexture = matTextureMgr::LoadTexture(m_TextureLocator);

	matUVATexture* pNewUVATexture = dynamic_cast<matUVATexture*>(pNewTexture);
	if( pNewUVATexture )
	{
		// if the new texture is a uva, then let it replace the old uva.
		if( m_pTexture )
			matTextureMgr::ReleaseTexture(m_pTexture);
		m_pTexture = pNewTexture;
	}
	else
	{
		// if the new texture is not a uva, then upgrade it to a uva with
		//		the former uva's anim data by inserting it as page 0.
		matTexture* pOrigTexture = m_pTexture;
		matUVATexture* pOrigUVATexture = dynamic_cast<matUVATexture*>(m_pTexture);

		pNewUVATexture = matTextureMgr::CreateUVATexture();
		if (pNewTexture)
			pNewUVATexture->AddTexturePage(pNewTexture);
		m_pTexture = pNewUVATexture;
		if (pOrigUVATexture)
		{
			pNewUVATexture->CopyData(pOrigUVATexture);
		}
		if (pOrigTexture)
			matTextureMgr::ReleaseTexture(pOrigTexture);
	}

//	m_pTexture = matTextureMgr::LoadTexture(m_TextureLocator);
}

//--------------------------------------------------------------------
//	MakeEmitter makes an emitter from parameters in the template.  
//	The prtEmitter is allocated on the heap, and should be deleted
//	later by the client.
//--------------------------------------------------------------------
prtEmitter* prtParticleGeneratorTemplate::MakeEmitter() const
{
	switch( m_EmitterType )
	{
		case e_Point:
		{
			return new prtPointEmitter;
		}
		break;

		case e_Circle:
		{
			return new prtCircleEmitter(m_EmitterScale.m_X);
		}
		break;
		
		case e_Block:
		{
			return new prtBlockEmitter(m_EmitterScale.m_X, m_EmitterScale.m_Y, m_EmitterScale.m_Z);
		}
		break;
	}

	return NULL;
}
