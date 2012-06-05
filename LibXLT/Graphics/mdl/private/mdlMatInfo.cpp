/****************************************************************************\
**	mdlMatInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlMatInfo.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maSTLHelpers.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrTextureFinder.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void load_effects(	const mdlMaterialInfo& i_MatInfo,
						matMaterial* o_pMaterial,
						const fsResourceFinder& i_TextureFinder)
	{
		matMaterial* mat = o_pMaterial;
		if (i_MatInfo.GetShaderParams())
		{
			mat->SetShaderParams(i_MatInfo.GetShaderParams());
			// Load shader and set up bindings here.
			// It is important to load the shader here because the material needs to know
			// which shader params are our standardized diffuse and alpha parameters.
			if (!mat->GetShaderParams()->m_pShaderBindings)
			{
				matShaderEffect* eff = mat->GetShaderParams()->GetShader();
				if (eff)
				{
					eff->BuildPrtyObject(mat->GetShaderParams().get());
				}
				else
				{
					DBG_WARNING("Shader failed to load.");
				}
			}
		}
		else
		{
			DBG_WARNING("No Shader parameters.");
		}

		//mat->SetShaderEffect(i_MatInfo.GetShader(), i_MatInfo.GetShaderData());

		// copy glow parameters here too 
		if (i_MatInfo.GetHasGlow()) // bga - this check was commented out in old mdlMatInfo code
		{
			mat->SetHasGlow(i_MatInfo.GetHasGlow());
			mat->GlowData() = i_MatInfo.GetGlowParams();
			//mat->GlowData().m_bConstantGlow = i_MatInfo.m_GlowData.m_bConstantGlow;
			//mat->GlowData().m_GlowAmount = i_MatInfo.m_GlowData.m_GlowAmount;
			//mat->GlowData().m_GlowScale = i_MatInfo.m_GlowData.m_GlowScale;
			//mat->GlowData().m_GlowSize = i_MatInfo.m_GlowData.m_GlowSize;
			//mat->GlowData().m_NameGlowMask = i_MatInfo.m_GlowData.m_NameGlowMask;
			// glow mask texture
			mat->GlowData().ReloadTextures(i_TextureFinder);
		}
		// copy outline parameters here too 
		if (i_MatInfo.GetHasOutline())
		{
			mat->SetHasOutline(i_MatInfo.GetHasOutline());
			mat->OutlineData() = i_MatInfo.GetOutlineParams();
		}

		if (i_MatInfo.GetHasReflection())
		{
			mat->SetHasReflection(i_MatInfo.GetHasReflection());
			mat->ReflectionData() = i_MatInfo.GetReflectionParams();
		}

		// copy displacement parameters here too 
		if (i_MatInfo.GetHasDisplacement())
		{
			mat->SetHasDisplacement(i_MatInfo.GetHasDisplacement());
			mat->DisplacementData() = i_MatInfo.GetDisplacementParams();

			// displacement texture
			mat->DisplacementData().ReloadTextures(i_TextureFinder);
		}

		// copy displacement parameters here too 
		if (i_MatInfo.GetHasRendermanOverride())
		{
			mat->SetHasRendermanOverride(i_MatInfo.GetHasRendermanOverride());
			mat->RendermanOverrideData() = i_MatInfo.GetRendermanOverrideParams();

			// displacement texture
			mat->RendermanOverrideData().ReloadTextures(i_TextureFinder);
		}

		// Normal Maps
		mat->NormalsData() = i_MatInfo.GetNormalsParams();		
		mat->NormalsData().ReloadTextures(i_TextureFinder);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlMatInfo::mdlMatInfo()
:	m_pMaterial(NULL)
{
}

//------------------------------------------------------------------------
// Create material pointer and set name, but don't load effects 
//	or textures.
//------------------------------------------------------------------------
void mdlMatInfo::CreateMaterial()
{
	// Create material at this point
	if (!this->m_pMaterial)
	{
		this->m_pMaterial = new matMaterial();
		this->m_pMaterial->SetName( this->m_Info.GetMaterialName() );
	}
}

//------------------------------------------------------------------------
// Load textures from effect data into material.
// This function creates the m_pMaterial pointer if needed.
//------------------------------------------------------------------------
void mdlMatInfo::LoadTextures(const fsResourceFinder& i_TextureFinder,
				   std::vector<matTexture*>& o_Textures)
{	
	// Create material at this point
	if (!m_pMaterial)
	{
		m_pMaterial = new matMaterial();
	}
	this->LoadTextures(this->m_pMaterial, i_TextureFinder, o_Textures);
}

//------------------------------------------------------------------------
// Loads textures from this material info into any material, 
// not just the m_pMaterial pointer.
//------------------------------------------------------------------------
void mdlMatInfo::LoadTextures(matMaterial *o_pMaterial,
							  const fsResourceFinder& i_TextureFinder,
							   std::vector<matTexture*>& o_Textures)
{
	// Only read one material at a time so that
	// we don't confuse the effect and shader code.
	//envScopedLock material_lock(mdlReader::GetReaderMutex());

	o_pMaterial->SetName( this->m_Info.GetMaterialName() );

	matMaterial* mat = o_pMaterial;

	// Use resource finder that searches through material library also
	mtrTextureFinder texture_finder(i_TextureFinder);

	// putting this here because it is similar code
	mat->UVTransform() = this->m_Info.GetUVTransform();
	load_effects(this->m_Info, mat, texture_finder);
	mat->GlowData().GetTextures(o_Textures);
	mat->DisplacementData().GetTextures(o_Textures);
	mat->NormalsData().GetTextures(o_Textures);
	mat->RendermanOverrideData().GetTextures(o_Textures);

	if (mat->GetEffectData())
	{
		mat->GetEffectData()->ReloadTextures(texture_finder);
		mat->GetEffectData()->GetTextures(o_Textures);
	}
	if (mat->GetShaderParams())
	{
		mat->GetShaderParams()->ReloadTextures(texture_finder);
		mat->GetShaderParams()->GetTextures(o_Textures);
	}
}

