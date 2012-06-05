/*****************************************************************************
**	api3dShape.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dShape.hpp"

#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"


//============================================================================
//============================================================================
namespace api3dShape
{
	maFloatRGBA l_LowColor( 0.0f, 0.0f, 0.0f, 1.0f );

	namespace
	{
		api3dObjectSimple* object_for_fragment(g3dFragment* i_Frag, 
											   const maFloatRGBA &i_Color)
		{
			matMaterial *mat = new matMaterial("Phong.fx");
			
			effPhongData* pData = dynamic_cast<effPhongData*>(mat->GetEffectData());
			DBG_ASSERT(pData != NULL, "object_for_fragment api3dobjectsimple not using effPhong");

			pData->m_ColorEmissive = (i_Color);
			pData->m_ColorAmbient = ( l_LowColor );
			pData->m_ColorDiffuse = ( l_LowColor );
			pData->m_ColorSpecular = ( l_LowColor );
			pData->m_ColorDiffuse.SetAlpha( i_Color.GetAlpha() );
			pData->m_Transparency = i_Color.GetAlpha();
			i_Frag->SetMaterial(mat);
			return new api3dObjectSimple(i_Frag, mat);
		}
		api3dObjectSimple* solid_object_for_fragment(g3dFragment* i_Frag, 
			const maFloatRGBA &i_Color)
		{
			matMaterial *mat = new matMaterial("Solid.fx");

			effSolidData* pData = dynamic_cast<effSolidData*>(mat->GetEffectData());
			DBG_ASSERT(pData != NULL, "object_for_fragment api3dobjectsimple not using effSolid");

			pData->m_Color = i_Color;
			i_Frag->SetMaterial(mat);

			return new api3dObjectSimple(i_Frag, mat);
		}


	}	// end of namespace

	//--------------------------------------------------------------------
	//	CreateCube makes a fragment which is a cube with sides of the
	//	given length.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCube( const maFloatRGBA &i_Color, 
									float i_fSide,
									bool i_bMorphable)
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateCube( i_fSide, i_bMorphable );
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateTexturedRectangle makes a Rectangle with the given width,	
	//	height, and texture.  It will be XY planar centered at 0,0,0.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateTexturedRectangle(	const fsLocator& i_TexturePath,
											const maFloatRGBA &i_Color, 
											float i_fWidth,
											float i_fHeight,
											int i_nWidthDivisions,
											int i_nHeightDivisions,
											bool i_bMorphable )
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle( i_fWidth, i_fHeight, i_nWidthDivisions, i_nHeightDivisions, i_bMorphable );

		api3dObjectSimple* pObj = object_for_fragment(fragment, i_Color);

		//std::string texture_name;
		//fsFileUtil::LocatorToANSIFilename(i_TexturePath, texture_name);
		//DBG_LOG( "textured rectangle (" << texture_name.c_str() << ")" );

		matTexture* pTex = matTextureMgr::LoadTexture(i_TexturePath);
		fragment->GetMaterial()->TypedData<effPhongData>()->m_TextureDiffuse = pTex;

		return pObj;
	}

	//--------------------------------------------------------------------
	// 	CreateRectangle makes a Rectangle with the given width and height.  
	//	It will be XY planar centered at 0,0,0.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateRectangle(	const maFloatRGBA &i_Color, 
									float i_fWidth,
									float i_fHeight,
									int i_nWidthDivisions,
									int i_nHeightDivisions,
									bool i_bMorphable )
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateRectangle( i_fWidth, i_fHeight, i_nWidthDivisions, i_nHeightDivisions, i_bMorphable );
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateCone makes a cone object based on the color, radius, height,
	//	and divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCone(const maFloatRGBA &i_Color, float i_fRadius,
							float i_fHeight, int i_nDivisions)
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateCone( i_fRadius, i_fHeight, i_nDivisions );
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateCylinder makes a cylinder object based on the color, radius, height,
	//	and divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCylinder(const maFloatRGBA &i_Color, float i_fRadius,
							float i_fBottomRadius, float i_fHeight, int i_nDivisions)
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateCylinder( i_fRadius, i_fBottomRadius, i_fHeight, i_nDivisions );
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateSphere makes a sphere-like object which has the
	// given radius, latitude divisions, and longitude divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateSphere(const maFloatRGBA &i_Color, float i_fRadius,
								int i_nLatDiv, int i_nLongDiv)
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateSphere(i_fRadius, i_nLatDiv, i_nLongDiv);
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateLineBlock makes a fragment which is a block with
	// sides of the given lengths, drawn with lines.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateLineBlock(const maFloatRGBA &i_Color, 
										float i_fXSide, float i_fYSide, float i_fZSide, 
										bool i_bMorphable /*= false*/)
	{
		g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateLineBlock(i_fXSide, i_fYSide, i_fZSide, i_bMorphable);
		
		return solid_object_for_fragment(fragment, i_Color);
	}

	//--------------------------------------------------------------------
	// 	CreateLineList makes a line that passes through the
	//	vertices in the array.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateLineList(const maFloatRGBA &i_Color,
				maPoint3d* i_pVertices, int i_NumVerts,
				bool i_bClosed /*= false*/, bool i_bMorphable /*= false*/)
	{
		std::vector<maPoint3d> line_normals(i_NumVerts, maPoint3d(0,1,0));

		int numIndices = 2*(i_NumVerts-1) + (i_bClosed ? 2 : 0);
		std::vector<unsigned short> indices(numIndices);

		int i = 0;
		for (i=0; i<i_NumVerts-1; i++)
		{
			indices[2*i] = i;
			indices[2*i+1] = i+1;
		}
		if (i_bClosed)
		{
			indices[2*i] = i;
			indices[2*i+1] = 0;
		}

		g3dFragment* fragment = g3dFragmentCreate::CreateLineList( i_pVertices,
						 			&(line_normals[0]), i_NumVerts,
									&(indices[0]), numIndices,
									&g3dPrimitiveFragmentUtil::DefaultMaterial(),
									i_bMorphable);

		return solid_object_for_fragment(fragment, i_Color);

	}

}	// end of namespace
