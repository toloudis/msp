/*****************************************************************************
**	prtParticleGeneratorTemplate.hpp
**
**		A prtParticleGeneratorTemplate provides all the information needed
**	to create a particle generator.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_PARTICLEGENERATORTEMPLATE_HPP
#error prtParticleGeneratorTemplate.hpp multiply included
#endif
#define PRT_PARTICLEGENERATORTEMPLATE_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRT_SPRITEGROUPPARTICLEGENERATOR_HPP
#include "Graphics/prt/prtSpriteGroupParticleGenerator.hpp"
#endif
#ifndef SC_PARTICLEGENERATORTEMPLATE_HPP
#include "Graphics/sc/scParticleGeneratorTemplate.hpp"
#endif


//============================================================================
//============================================================================
class prtParticleGeneratorTemplate : public scParticleGeneratorTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		prtParticleGeneratorTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtParticleGeneratorTemplate();

		//--------------------------------------------------------------------
		//	Type refers to the type of the particle generator
		//--------------------------------------------------------------------
		enum Type
		{
			e_Static = 0,
			e_Cone,
			e_Spiral,
			e_Cone3D,
			e_Static3D,
			e_Spiral3D
		};

		Type GetType() const { return m_Type; }
		void SetType(Type i_Type) { m_Type = i_Type; }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		enum EmitterType
		{
			e_Point = 0,
			e_Block,
			e_Circle
		};

		EmitterType GetEmitterType() const { return m_EmitterType; }
		void SetEmitterType(EmitterType i_EmitterType) { m_EmitterType = i_EmitterType; }

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		const maVector3d& GetEmitterScale() const { return m_EmitterScale; }
		void SetEmitterScale(const maVector3d& i_Scale) { m_EmitterScale = i_Scale; }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtSpriteGroupParticleGenerator::RenderMode GetRenderMode() const { return m_RenderMode; }
		void SetRenderMode(prtSpriteGroupParticleGenerator::RenderMode i_RenderMode) { m_RenderMode = i_RenderMode; }
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtSpriteGroupParticleGenerator::UVAMode GetUVAMode() const { return m_UVAMode; }
		void SetUVAMode(prtSpriteGroupParticleGenerator::UVAMode i_UVAMode) { m_UVAMode = i_UVAMode; }
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtSpriteGroupParticleGenerator::ScaleMode GetScaleMode() const { return m_ScaleMode; }
		void SetScaleMode(prtSpriteGroupParticleGenerator::ScaleMode i_ScaleMode) { m_ScaleMode = i_ScaleMode; }

		//--------------------------------------------------------------------
		// Streak rendering renders a stretched polygon to represent the
		// motion of a particle over time.
		//--------------------------------------------------------------------
		bool GetRenderStreaks() const { return m_bRenderStreaks; }
		void SetRenderStreaks(bool i_bStreaks) { m_bRenderStreaks = i_bStreaks; }

		//--------------------------------------------------------------------
		//	These functions set the other "Parameters" in the particle
		//	generator.
		//--------------------------------------------------------------------
		void SetNumParameters(int i_Num);
		int GetNumParameters() const;
		float GetParameter(int i_Num) const;
		void SetParameter(int i_Num, float i_Val);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetAlphaAnimation(const anTypedAnimation<float>& i_Anim);
		const anTypedAnimation<float>& GetAlphaAnimation() const { return *m_pAlphaAnim; }

		//--------------------------------------------------------------------
		//	The template may store a locator referring to the texture used
		//	by the particle generator.  The locator could also contain
		//	a single name - the texture filename, which will have to be
		//	augmented by a directory before MakeTexture is called.
		//--------------------------------------------------------------------
		const fsLocator& GetTextureLocator() const { return m_TextureLocator; }
		void SetTextureLocator(const fsLocator& i_Locator) { m_TextureLocator = i_Locator; }

		//--------------------------------------------------------------------
		//	The template may store a locator referring to the geometry used
		//	by the 3D particle generator. 
		//--------------------------------------------------------------------
		const fsLocator& GetGeometryLocator() const { return m_GeometryLocator; }
		void SetGeometryLocator(const fsLocator& i_Locator) { m_GeometryLocator = i_Locator; }

		//--------------------------------------------------------------------
		//	GetTexture returns the texture used by the particle generator.
		//	This could be NULL, if MakeTexture has not been called to make
		//	a texture from the given locator.
		//--------------------------------------------------------------------
		matTexture* GetTexture() const;

		//--------------------------------------------------------------------
		//	MakeTexture loads a texture from the texture locator.
		//--------------------------------------------------------------------
		void MakeTexture();

		//--------------------------------------------------------------------
		//	MakeEmitter makes an emitter from parameters in the template.  
		//	The prtEmitter is allocated on the heap, and should be deleted
		//	later by the client.
		//--------------------------------------------------------------------
		prtEmitter* MakeEmitter() const;

	private:
		Type m_Type;
		EmitterType m_EmitterType;
		maVector3d m_EmitterScale;
		bool m_bRenderStreaks;

		prtSpriteGroupParticleGenerator::RenderMode m_RenderMode;
		prtSpriteGroupParticleGenerator::UVAMode m_UVAMode;
		prtSpriteGroupParticleGenerator::ScaleMode m_ScaleMode;

		std::vector<float> m_Parameters;
		anTypedAnimation<float>* m_pAlphaAnim;

		fsLocator m_TextureLocator;
		fsLocator m_GeometryLocator;

		matTexture* m_pTexture;
};
