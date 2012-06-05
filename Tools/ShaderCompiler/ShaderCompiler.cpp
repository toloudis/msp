/*****************************************************************************
**  ShaderCompiler.cpp
**
**  A program to compile custom pixel shaders.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include <fstream>
#include <sstream>
#include <iostream>

#include "ShaderStrings.hpp"

#include "Core/env/envPlatform.hpp"
#include "Graphics/eff/effShaderSDK.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"

#define PLACES	1

//----------------------------------------------------
// MsgProc()
//----------------------------------------------------
LRESULT WINAPI MsgProc( HWND i_hWnd, UINT i_msg, WPARAM i_wParam, LPARAM i_lParam )
{
    return DefWindowProc( i_hWnd, i_msg, i_wParam, i_lParam );
}

//----------------------------------------------------
// Cleanup()
//----------------------------------------------------
void Cleanup( WNDCLASSEX i_wc )
{
	UnregisterClass( L"A Basic Window", i_wc.hInstance );
	FreeConsole();
	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	GraphicsDX9Layer::CleanUp();
	GraphicsLayer::CleanUp();
}

#define SETUP_GLOBALS
//----------------------------------------------------
// ReadCustomShader()
//----------------------------------------------------
bool ReadCustomShader( const char * i_in , std::string& o_out )
{
	std::ifstream inFile( i_in );

	if ( !inFile ) return false;

	std::string str;
	bool wroteStub = false;
	bool setGlobals = false;
	while( std::getline( inFile, str ) )
	{
		o_out.append("\r\n").append(str);

#ifdef SETUP_GLOBALS
		int stubIdx = o_out.find("float4 SurfaceShader()");
		if ( stubIdx != std::string::npos )
		{
			std::string stub = "float4 SurfaceShader(float3 Ng, float3 N, float3 L, float3 I, float3 E, float3 P, float3 Cld, float3 Cls, float4 Csd, float4 Css, float Os, float g_bumpMapScale)";
			o_out.replace(stubIdx,stub.length(),"float4 SurfaceShader(float3 Ng, float3 N, float3 L, float3 I, float3 E, float3 P, float3 Cld, float3 Cls, float4 Csd, float4 Css, float Os, float g_bumpMapScale)");
			//wroteStub = true;
		}
		/*
		if ( wroteStub )
		{
			if ( str.find("{") != std::string::npos )
			{
				wroteStub = false;
				setGlobals = true;
			}
		}

		if ( setGlobals )
		{
			o_out.append("\r\n").append("float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );");
			o_out.append("\r\n").append("float3 L = normalize(light.L);");
			o_out.append("\r\n").append("float3 E = g_eyePos.xyz;");
			o_out.append("\r\n").append("float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);");
			o_out.append("\r\n").append("float3 P = IN.V.WorldPos;");
			o_out.append("\r\n").append("float3 Cld = light.Cld;");
			o_out.append("\r\n").append("float3 Cls = light.Cls;");
			o_out.append("\r\n").append("float4 SurfaceDiff = g_diffuse;");
			o_out.append("\r\n").append("float4 SurfaceSpec = g_specular;");
			setGlobals = false;
		}
		*/
#endif
	}

	inFile.close();

	return true;
}

//----------------------------------------------------
// WriteShaderTextDEBUG()
//----------------------------------------------------
bool WriteShaderTextDEBUG( std::string i_in , const char * i_out )
{
	std::ofstream outFile(i_out, std::ios::out | std::ios::binary);

	if(!outFile) return false;

	outFile.write(i_in.c_str() , i_in.length());
	outFile.close();

	return true;
}

//----------------------------------------------------
// WriteShaderBin()
//----------------------------------------------------
bool WriteShaderBin( const char * i_out , BYTE * i_data , int i_size )
{
	std::ofstream outFile(i_out, std::ios::out | std::ios::binary);

	if(!outFile) return 0;
	
	outFile.write( (char*)i_data , i_size );
	outFile.close();

	return 1;
}

//----------------------------------------------------
// decrypt()
//----------------------------------------------------
char decrypt(char i_c, int i_numPlaces)
{
	if ( (i_c >= 31) && (i_c <= '~') )
	{
		return i_c + i_numPlaces;
	}
    return i_c;
}

//----------------------------------------------------
// decryptArray()
//----------------------------------------------------
void decryptArray( std::string* i_encrypted , int i_size, std::string& o_decrypted )
{
	for ( int i = 0 ; i < i_size ; i++ )
	{
		const char * encrypted = i_encrypted[i].c_str();
		std::string decrypted = "";
		for ( unsigned int j = 0 ; j < i_encrypted[i].size() ; j++ )
		{
			char encrypted_chr = encrypted[j];
			char decrypted_chr = decrypt( encrypted_chr , PLACES );
			decrypted.push_back(decrypted_chr);
		}
		o_decrypted.append(decrypted);
		o_decrypted.append("\r\n");
	}
}

//----------------------------------------------------
// WinMain()
//----------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	// Init console		
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen("CONOUT$","wb",stdout);

	// If invalid args
	if ( __argc != 3 )
	{
		std::cout << "Usage: ShaderCompiler.exe <in.hlsl> <out.fx>" << std::endl;		
		return 0;
	}
	
	// Flow control flag
	bool success = false;

	// Get hWnd
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"A Basic Window", NULL };
    RegisterClassEx( &wc );
    HWND hWnd = CreateWindow( L"A Basic Window", L"A Basic Window", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, NULL, NULL, wc.hInstance, NULL );

	// Init graphics 	
	std::cout << "Initializing graphics subsystem ... ";
	GraphicsLayer::Init();
	GraphicsDX9Layer::Init( hWnd );
	GraphicsLayer::InitGraphics(GraphicsDX9Layer::GetSystem2D(), GraphicsDX9Layer::GetSystem3D());
	GraphicsDX9Layer::InitGraphics();
	std::cout << "OK!" << std::endl;
		
	// Delcare external source files
	std::string header;
	std::string footer;
	std::string custom;
	std::string fullShader;

	// Decrypt header & footer
	decryptArray( g_Header, sizeof(g_Header)/sizeof(std::string) , header );
	decryptArray( g_Footer, sizeof(g_Footer)/sizeof(std::string) , footer );

	// Read custom shader
	std::string processing = "Processing \"";
	processing.append( __argv[1] ).append("\" ... ");
	std::cout << processing;

	success = ReadCustomShader( __argv[1] , custom );	
	if ( !success )
	{
		std::cout << "FAILED." << std::endl;
		std::cout << "Shader reading failed." << std::endl;
		Cleanup(wc); 
		return 0;
	}

	// Stitch it all together
	fullShader.append(header).append(custom).append(footer);
	
#ifdef _DEBUG
	// Write out decrypted file for debugging
	success = WriteShaderTextDEBUG( fullShader, "CustomInMemory.hlsl" );
#endif

	int shaderSize = 0;
	int * pShaderSize = &shaderSize;

	BYTE * pCompiledShader = NULL;
	BYTE ** ppCompiledShader = &pCompiledShader;

	std::vector<std::string> errors;

	// Compile stitched shader in memory
	success = effShaderSDK::CompileCustomShader( fullShader.c_str(), ppCompiledShader, pShaderSize, 
												 errors, sizeof(g_Header)/sizeof(std::string), __argv[1] );

	// Write it to file if it compiled fine
	if ( success )
	{
		success = WriteShaderBin( __argv[2], pCompiledShader, shaderSize );
	}  

	// Failed to compile
	else 
	{
		std::cout << "FAILED." << std::endl;
		std::cout << "Error list:" << std::endl;		
		for ( int i = 0 ; i < errors.size() ; i++ )
		{
			std::cout << errors[i];
		}

		Cleanup(wc); 
		return 0;
	}

	// Successfully wrote bin file
	if ( success )
	{
		std::cout << "OK!" << std::endl;
		std::string finished = "Wrote binary shader \"";
		finished.append( __argv[2] ).append("\" successfully.");
		std::cout << finished << std::endl;
	}

	// Failed to write bin file
	else 
	{
		std::cout << "FAILED." << std::endl;
		std::cout << "Compile succeeded but writing failed." << std::endl;
	}
	
	// Cleanup
	Cleanup(wc);

    return 1;
}
