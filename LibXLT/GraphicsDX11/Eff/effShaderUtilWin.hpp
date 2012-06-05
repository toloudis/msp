/****************************************************************************\
**  effShaderUtilWin.hpp
**
**      effShaderUtilWin sets up effect shaders from files
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_SHADERUTILWIN_HPP
#error effShaderUtilWin.hpp multiply included
#endif
#define EFF_SHADERUTILWIN_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#include c_g2dD3DX11Effect_H
#include <string>

class fsLocator;
class matShaderEffect;

namespace effShaderUtilWin
{

	//------------------------------------------------------------------------
	// Initialize system
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	// Clean up system
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	// Create effect from file
	//------------------------------------------------------------------------
	matShaderEffect* CompileEffect(const fsLocator &i_Locator);
	matShaderEffect* CompileGenericEffect(const fsLocator &i_Locator);

	ID3DX11Effect* CompileStringEffect(const char* eff);


	//------------------------------------------------------------------------
	// Create effect from compiled binary data array
	//------------------------------------------------------------------------
	ID3DX11Effect* LoadEffectData(void* i_Eff, int i_nBytes, std::string i_Name);

	ID3DX11Effect* LoadEffectDX11(const fsLocator& i_Locator);

	void LoadShaderDX11(const fsLocator& i_Locator, 
		const std::string& i_Entrypoint,
		const std::string& i_Profile,
		ID3DBlob** o_Shader);


	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	HRESULT CompileShaderFromFile( WCHAR* szFileName, 
		LPCSTR szEntryPoint, 
		LPCSTR szShaderModel, 
		ID3DBlob** ppBlobOut );
	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	HRESULT LoadShaderFromFile( const WCHAR* szFileName, SIZE_T* pSizeOut, void** ppBlobOut );
}
