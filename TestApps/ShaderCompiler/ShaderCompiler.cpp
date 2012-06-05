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
#include <string>
#include <d3d9.h> 
#include <d3dx9.h>
#include <D3DX9Effect.h>

#include "ShaderStrings.hpp"

#define PLACES	1

using namespace std;

//----------------------------------------------------
// Libs
//----------------------------------------------------
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//----------------------------------------------------
// Globals
//----------------------------------------------------
LPDIRECT3D9             g_pD3D			= NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB			= NULL;
LPD3DXBUFFER			g_pEffect		= NULL;
HANDLE					g_consoleHandle	= NULL;

//----------------------------------------------------
// d3d_init()
//----------------------------------------------------
void d3d_init( HWND hWnd )
{
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice );
}

//----------------------------------------------------
// MsgProc()
//----------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hWnd, msg, wParam, lParam );
}

//----------------------------------------------------
// Write()
//----------------------------------------------------
void Write(HANDLE g_consoleHandle, std::string t)
{
    WriteConsole(g_consoleHandle, t.c_str(), (DWORD)t.length(), NULL, NULL);
}

//----------------------------------------------------
// Cleanup()
//----------------------------------------------------
void Cleanup( WNDCLASSEX wc )
{
	Write(g_consoleHandle, "Press Enter to exit.\n");
	UnregisterClass( "A Basic Window", wc.hInstance );
	FreeConsole();
	if( g_pVB != NULL ) g_pVB->Release();	 
    if( g_pd3dDevice != NULL ) g_pd3dDevice->Release();	
    if( g_pD3D != NULL ) g_pD3D->Release();	
}

//----------------------------------------------------
// CompileStringEffect()
//----------------------------------------------------
HRESULT CompileStringEffect(const char * eff)
{
	HRESULT hr;
	LPD3DXEFFECTCOMPILER pEffectCompiler = NULL;
	LPD3DXBUFFER pParseErrors = NULL;
	hr = D3DXCreateEffectCompiler(eff, strlen(eff), NULL, NULL,
		0,
		&pEffectCompiler,
		&pParseErrors
	);

	if ( hr != S_OK ) return hr;

	LPD3DXBUFFER pErrorMsgs = NULL;
	hr = pEffectCompiler->CompileEffect(0, &g_pEffect, &pErrorMsgs);

	if ( hr != S_OK ) return hr;

	LPD3DXEFFECT pD3DXEffect = NULL;
	hr = D3DXCreateEffect(g_pd3dDevice, 
		g_pEffect->GetBufferPointer(), g_pEffect->GetBufferSize(), 
		NULL, NULL, 0, NULL,
		&pD3DXEffect, NULL);

	return hr;
}

//----------------------------------------------------
// ReadShader()
//----------------------------------------------------
HRESULT ReadShader( const char * in , string & out )
{
	ifstream inFile( in );

	if ( !inFile ) return 0;

	string temp;
	while( getline( inFile, temp ) )
	{
		out.append("\r\n").append(temp);
	}

	inFile.close();

	return 1;
}

//----------------------------------------------------
// WriteShaderTextDEBUG()
//----------------------------------------------------
HRESULT WriteShaderTextDEBUG( string in , const char * out )
{
	ofstream outFile(out, ios::out | ios::binary);

	if(!outFile) return 0;

	outFile.write(in.c_str() , in.length());
	outFile.close();

	return 1;
}

//----------------------------------------------------
// WriteShaderBin()
//----------------------------------------------------
HRESULT WriteShaderBin( const char * out )
{
	ofstream outFile(out, ios::out | ios::binary);

	if(!outFile) return 0;
	
	outFile.write((char*)g_pEffect->GetBufferPointer(), g_pEffect->GetBufferSize());
	outFile.close();

	return 1;
}

//----------------------------------------------------
// decrypt()
//----------------------------------------------------
char decrypt(char c, int numPlaces)
{
	if ( (c >= 31) && (c <= '~') )
	{
		return c + numPlaces;
	}

    return c;
}

//----------------------------------------------------
// WinMain()
//----------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{

	// Direct3D & windows d3d_init
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "A Basic Window", NULL };
    RegisterClassEx( &wc );
    HWND hWnd = CreateWindow( "A Basic Window", "A Basic Window", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, NULL, NULL, wc.hInstance, NULL );
	d3d_init( hWnd );
	HRESULT hr = 0;
	
	// Console init
	AttachConsole(ATTACH_PARENT_PROCESS); 
	g_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	Write(g_consoleHandle, "\n");
	
	// If invalid args
	if ( __argc != 3 )
	{
		Write(g_consoleHandle,"Usage: CompileCustomShader <in.h> <out.fx>\n");
		Cleanup(wc);
		return 0;
	}

	Write(g_consoleHandle, "Processing...\n");

	string header;
	string footer;
	string custom;
	string fullShader;

	int headerSize = sizeof(g_Header)/sizeof(std::string);
	for ( int i = 0 ; i < headerSize ; i++ )
	{
		// unscramble
		const char * encrypted = g_Header[i].c_str();
		string decrypted = "";
		for ( unsigned int j = 0 ; j < g_Header[i].size() ; j++ )
		{
			char encrypted_chr = encrypted[j];
			char decrypted_chr = decrypt( encrypted_chr , PLACES );
			decrypted.push_back(decrypted_chr);
		}
		header.append(decrypted);
		header.append("\r\n");
	}

	int footerSize = sizeof(g_Footer)/sizeof(std::string);
	for ( int i = 0 ; i < footerSize ; i++ )
	{
		// unscramble
		const char * encrypted = g_Footer[i].c_str();
		string decrypted = "";
		for ( unsigned int j = 0 ; j < g_Footer[i].size() ; j++ )
		{
			char encrypted_chr = encrypted[j];
			char decrypted_chr = decrypt( encrypted_chr , PLACES );
			decrypted.push_back(decrypted_chr);
		}
		footer.append(decrypted);
		footer.append("\r\n");
	}	

	hr = ReadShader( __argv[1] , custom );
	
	if ( hr != 1 )
	{
		Write(g_consoleHandle,"Shader reading failed.\n");
		Cleanup(wc); 
		return 0;
	}

	fullShader.append(header).append(custom).append(footer);
	
#ifdef _DEBUG
	hr = WriteShaderTextDEBUG( fullShader, "CustomInMemory.hlsl" );
#endif
	
	hr = CompileStringEffect( fullShader.c_str() );
	if ( hr == S_OK )
	{
		hr = WriteShaderBin( __argv[2] );
	} 
	else 
	{
		Write(g_consoleHandle,"Shader compile failed, please check your input file.\n");
		Cleanup(wc); 
		return 0;
	}

	if ( hr == 1 )
	{
		Write(g_consoleHandle, "Shader compile success!\n");		
	} 
	else 
	{
		Write(g_consoleHandle, "Shader writing failed.\n");
	}
	

	Cleanup(wc);
    return 1;
}
