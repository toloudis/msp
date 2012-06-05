/****************************************************************************\
**	mdlAnimImport.hpp
**
**		mdlAnimImport.hpp supplies functions used to import 
**	animation files.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_ANIMIMPORT_HPP
#error mdlAnimImport.hpp multiply included
#endif
#define MDL_ANIMIMPORT_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif 
#ifndef SMDL_KEYROOTMAP_HPP
#include "Graphics/smdl/smdlKeyRootMap.hpp"
#endif 
#ifndef SMDL_MORPHANIMKEYS_HPP
#include "Graphics/smdl/smdlMorphAnimKeys.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class chReader;
class fsLocator;


//============================================================================
//	Any of these mdlAnimImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlAnimImport
{
	//------------------------------------------------------------------------
	//	LoadAnimation loads an animation from a file.  The smdlTree should be
	//	empty when initially passed in.
	//------------------------------------------------------------------------
	void LoadAnimation(	const fsLocator& i_Locator,
						smdlKeyRootMap& o_KeyRoots,
						std::vector<anKeyData<maPoint3d>*>& o_TranslationChannels,
						std::vector<anKeyData<maRotation>*>& o_RotationChannels,
						std::vector<anKeyData<maVector3d>*>& o_ScaleChannels,
						std::vector<anKeyDataBase<bool>*>& o_VisibleChannels,
						float &o_FramesPerSecond);
		
	//------------------------------------------------------------------------
	//	LoadCharacterAnimation loads a combination of joint and blend shape
	//	animation from the given file.
	//------------------------------------------------------------------------
	void LoadCharacterAnimation( const fsLocator& i_Locator,
						smdlKeyRootMap& o_KeyRoots,
						std::vector<anKeyData<maPoint3d>*>& o_TranslationChannels,
						std::vector<anKeyData<maRotation>*>& o_RotationChannels,
						std::vector<anKeyData<maVector3d>*>& o_ScaleChannels,
						std::vector<anKeyDataBase<bool>*>& o_VisibleChannels,
						bool &o_DeltaAnimation,
						float &o_FramesPerSecond,
						float &o_BeginFrame,
						std::map<std::string, smdlMorphAnimKeys>& o_BlendShapeKeys,
						std::vector<anKeyData<float>*>& o_MorphChannels,
						std::map<std::string, smdlVertexAnimKeys>& o_VertexKeys,
						smdlVertexFrames& o_VertexFrames,
						std::map<std::string, smdlGeoAnimKeys>& o_SkinKeys);

	//------------------------------------------------------------------------
	//	LoadVertexAnimation loads the baked vertex and normal 
	//	animation from the given file.
	//------------------------------------------------------------------------
	void LoadVertexAnimation( const fsLocator& i_Locator,
						std::vector<smdlVertexAnimKeys>& o_VertexKeys,
						smdlVertexFrames& o_VertexFrames,
						float &o_FramesPerSecond);
}
