// SimpleSceneBuilderSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "eonTestScene.hpp"
#include "eonConverter.hpp"

#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Dbg/dbgPackage.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlReader.hpp"


#include <Dali/Libraries/SceneBuilder/ISceneBuilder.h>
#include <Dali/Libraries/SceneBuilder/win32/EONCOMSceneBuilderFactory.h>
#include <Dali/Libraries/Meshes/TriMesh.h>
#include <Dali/Libraries/SceneBuilder/SceneLight.h>
#include <Dali/Libraries/SceneBuilder/SceneTexture2D.h>
#include <Dali/Libraries/SceneBuilder/SceneMaterialAdvanced.h>
#include <Dali/Libraries/SceneBuilder/SceneMultiMaterial.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CComModule _Module;

// The one and only application object

CWinApp theApp;

using namespace std;
using namespace EON;

namespace
{
	// Version string of the tool to write in chunk at end of file
	const char* c_ConvertVersionStr = "1.0.1";
	const char* c_ExeName = "EON Converter";

	void read_mx_file(const fsLocator& i_GeoFile)
	{
		std::vector< shared_ptr<mdlFragInfo> > geoData;
		//mdlMatInfoTable material_table;
		mdlReader::ReadWorldFragments(i_GeoFile, geoData); //, material_table);

		DBG_WARNING1("Number of fragments loaded : %d", geoData.size());
		//DBG_WARNING1("Number of materials loaded : %d", material_table.size());
	}
}

//int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
int main(int argc, char* argv[])
{
	int nRetCode = 0;

	dbgPackage::Init();

	if (argc != 2)
	{
		std::cout << argv[0] << " version " << c_ConvertVersionStr << std::endl;
		std::cout << "Syntax: " << c_ExeName << " filename.ecv [output_filename.chx]" << std::endl;
		std::cout << "   If used with a single argument, compresses the file in place" << std::endl;
		std::cout << "   and puts a backup file, anim.bak, in the exe directory." << std::endl;
	}
	else
	{
		fsLocator conv_file;
		fsFileUtil::UnicodeStringToLocator(itString(argv[1]), conv_file);
		std::cout << "Running " << argv[0] << " on file " << argv[1] << std::endl;
		DBG_WARNING2("Running %s on file: %s", argv[0], argv[1]);


		// initialize MFC and print and error on failure
		if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			::CoInitialize(NULL);

			try
			{	
				eonConverter::DoConversion(conv_file);

				////const char* c_Filename = "Dinning_Room_Chandelier.mx";
				//const char* c_Filename = "Furniture.mx";
				////const char* c_Filename = "Prims.mx";
				//DBG_WARNING1("Running EONConverter on file: %s", c_Filename);
				//fsLocator geo_file;
				//geo_file.Push("Data");
				//geo_file.Push(c_Filename);

				//std::vector< shared_ptr<mdlFragInfo> > geoData;
				//mdlReader::ReadWorldFragments(geo_file, geoData);
				//DBG_WARNING1("Number of fragments loaded : %d", geoData.size());

				//if (!geoData.empty())
				//{
				//	//eonTestScene::DoFragTest(*geoData[0]);
				//	eonTestScene::DoFragTest(geoData);
				//}

				//eonTestScene::DoEONTest();
			}
			catch (fsReadOnlyX& i_Ex)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::cerr << "File is read only: " << filename << std::endl;
				DBG_ERROR1("File is read only: %s", filename.c_str());
			}
			catch (fsFileDoesntExistX& i_Ex)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::cerr << "File doesn't exist: " << filename << std::endl;
				DBG_ERROR1("File doesn't exist: %s", filename.c_str());
			}
			catch (fsInvalidLocatorX& i_Ex)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::cerr << "File doesn't exist: " << filename << std::endl;
				DBG_ERROR1("File doesn't exist: %s", filename.c_str());
			}
			catch (gfInvalidFileBinX& i_Ex)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::cerr << "File is not character animation file: " << filename << std::endl;
				DBG_ERROR1("File is not character animation file: %s", filename.c_str());
			}
			catch (chInvalidChunkX& )
			{
				std::cerr << "File format error in chunk parsing." << std::endl;
				DBG_ERROR0("File format error in chunk parsing.");
			}
			catch (mdlInvalidModelFileX& i_Ex)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::cerr << "File format error in file: " << filename << std::endl;
				DBG_ERROR1("File format error in file: %s", filename.c_str());
			}
			catch (EON::Exception *e)
			{
				TRACE((const char*)e->toString());
			}
			catch (_com_error &e)
			{
				 _bstr_t bstrSource(e.Source());
				 _bstr_t bstrDescription(e.Description());
				 TRACE("Exception thrown for classes generated by #import" );
				 TRACE("\tCode = %08lx\n",      e.Error());
				 TRACE("\tCode meaning = %s\n", e.ErrorMessage());
				 TRACE("\tSource = %s\n",       (LPCTSTR) bstrSource);
				 TRACE("\tDescription = %s\n",  (LPCTSTR) bstrDescription);
			}
			catch (...)
			{
				std::cerr << "Unknown error while processing." << std::endl;
				DBG_ERROR0("Unknown error while processing.");
			}

			::CoUninitialize();
		}
	}

	dbgPackage::CleanUp();

	return nRetCode;
}

