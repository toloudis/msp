/*****************************************************************************
**  orthoRemoteCommandMgr.hpp
**
**      Handles all the checking and processing of commands.
**
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef ORTHO_REMOTECOMMANDMGR_HPP
#error orthoRemoteCommandMgr.hpp multiply included
#endif
#define ORTHO_REMOTECOMMANDMGR_HPP

#ifndef APP_CHAREVENTHANDLER_HPP
#include "Core/App/appCharEventHandler.hpp"
#endif

#ifndef ORTHO_RENDERREQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRenderRequestTask.hpp"
#endif


#include <string>
#include <vector>
#include <fstream>

//============================================================================
// Forward reference
//============================================================================
class LightWaitResponse;
class mnmObject;

//============================================================================
// Event handler to respond to character key presses
//============================================================================
class LocalCharEventRequest : public appCharEventHandler
{
public:
	virtual void ReceiveCharEvent(appCharEvent& i_Event);
};

//============================================================================
//============================================================================
namespace orthoRemoteCommandMgr
{
	//
	//	Info about each animation driver
	//
	struct RemoteAnimFrameData
	{
		std::string m_AnimFile;
		float		m_fAnimStartTime;
		float		m_fAnimLength;
		float		m_OrientationY;
	};

	//
	//	Info about all the generated files (OUTPUT)
	//
	struct RemoteFilenameData
	{
		std::string m_Filename;
	};

	//	
	//	Info about remote render data
	//
	class RemoteRenderData
	{
	public:
		RemoteRenderData();
		~RemoteRenderData();

		std::string renderCamera;
		std::string renderName;
		std::string responseFormat;
		std::string imageFormat;
		std::string maskFormat;
		int renderWidth;
		int renderHeight;
		int renderStartWidth;
		int renderStartHeight;
		float renderTimePoint;
		float renderFPS;

		void SetRenderResponse( LightWaitResponse* response );		//will delete the existing response
		void CreateRenderResponse( const std::string& modelName );
		LightWaitResponse* GetRenderResponse(){ return m_CurrentRenderResponse; }
	private:
		LightWaitResponse *m_CurrentRenderResponse;
	};

	//	render statistics and animation frame list
	extern std::vector<RemoteRenderData>	m_RenderParameters;
	extern std::vector<RemoteAnimFrameData>	m_AnimFrameData;
	extern std::string m_RenderModelName;
	extern std::string l_CameraToRender;
	extern mnmObject* l_pSelectedObject;

	extern std::ifstream* l_pCommandFile;


	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void DeInitialize();

	//----------------------------------------------------------------------------
	//	Check the input
	//----------------------------------------------------------------------------
//	DWORD CheckInput( LPVOID param );

	void parse_input_xml(std::string& workingString);

	//----------------------------------------------------------------------------
	//	Compress the string into GZIP format
	//----------------------------------------------------------------------------
	bool CompressGZIPString( std::string& workingString );

	//----------------------------------------------------------------------------
	//	Decompress the string from GZIP format
	//----------------------------------------------------------------------------
	bool DecompressGZIPString( std::string& workingString );
};

