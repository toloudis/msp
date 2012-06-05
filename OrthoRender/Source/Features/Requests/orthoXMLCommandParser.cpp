/****************************************************************************\
**	orthoXMLCommandParser.cpp
**
**		see .hpp for an explanation of the parsing and task system.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Requests/orthoXMLCommandParser.hpp"
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"

#include "Core/Ma/maConstants.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"

//================================================================================
//-------orthoXMLCommandParser class-------------------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
orthoXMLCommandParser::orthoXMLCommandParser()
{
	m_bJobStarted = false;

	AddXMLPreFunctor( "job", &orthoXMLCommandParser::PreParseJob );

	//JobInfo Block
	AddXMLPreFunctor( "job-jobinfo", &orthoXMLCommandParser::PreParseJobInfo );
	AddXMLPreFunctor( "job-jobinfo-jobname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-jobinfo-jobname", &orthoXMLCommandParser::ParseJobName );
	AddXMLPostFunctor( "job-jobinfo", &orthoXMLCommandParser::PostParse );

	//Scene Block
	AddXMLPreFunctor( "job-scene", &orthoXMLCommandParser::PreParseScene );
	AddXMLPreFunctor( "job-scene-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-scene-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-scene", &orthoXMLCommandParser::PostParse );

	//Objects Block
	AddXMLPreFunctor( "job-objects", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-objects-models", &orthoXMLCommandParser::PreParseModels );
	AddXMLPreFunctor( "job-objects-models-basemodel", &orthoXMLCommandParser::PreParseBaseModel );
	AddXMLPreFunctor( "job-objects-models-basemodel-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-objects-models-basemodel-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-objects-models-model", &orthoXMLCommandParser::PreParseModel );
	AddXMLPreFunctor( "job-objects-models-model-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-objects-models-model-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-objects-models-model-filename", &orthoXMLCommandParser::ValueParseModelFileName );
	AddXMLPostFunctor( "job-objects-models-model-name", &orthoXMLCommandParser::ValueParseModelName );
	AddXMLPostFunctor( "job-objects-models-model", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-objects-models-basemodel-filename", &orthoXMLCommandParser::ValueParseModelFileName );
	AddXMLPostFunctor( "job-objects-models-basemodel-name", &orthoXMLCommandParser::ValueParseModelName );
	AddXMLPostFunctor( "job-objects-models-basemodel", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-objects-models", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-objects", &orthoXMLCommandParser::ParseDefault );

	//Modifications Block
	AddXMLPreFunctor( "job-modifications", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-attachments", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-attachments-attachment", &orthoXMLCommandParser::PreParseAttachment );
	AddXMLPreFunctor( "job-modifications-attachments-attachment-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-attachments-attachment-joint", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-attachments-attachment-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-detachments", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-detachments-detachment", &orthoXMLCommandParser::PreParseDetachment );
	AddXMLPreFunctor( "job-modifications-detachments-detachment-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-detachments-detachment-joint", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-detachments-detachment-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-showcharacters", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-showcharacters-character", &orthoXMLCommandParser::PreParseShowCharacter );
	AddXMLPreFunctor( "job-modifications-showcharacters-character-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-showcharacters-character-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-hidecharacters", &orthoXMLCommandParser::ParseDefault ) ;
	AddXMLPreFunctor( "job-modifications-hidecharacters-character", &orthoXMLCommandParser::PreParseHideCharacter ) ;
	AddXMLPreFunctor( "job-modifications-hidecharacters-character-name", &orthoXMLCommandParser::ParseDefault ) ;
	AddXMLPreFunctor( "job-modifications-hidecharacters-character-filename", &orthoXMLCommandParser::ParseDefault ) ;
	AddXMLPreFunctor( "job-modifications-textures", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-textures-texture", &orthoXMLCommandParser::PreParseTexture );
	AddXMLPreFunctor( "job-modifications-textures-texture-shader", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-textures-texture-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-textures-texture-layer", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-materials", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-materials-material", &orthoXMLCommandParser::PreParseMaterial );
	AddXMLPreFunctor( "job-modifications-materials-material-shader", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-materials-material-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-colors", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-colors-color", &orthoXMLCommandParser::PreParseColor );
	AddXMLPreFunctor( "job-modifications-colors-color-shader", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-colors-color-value", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-animations", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-animations-animation", &orthoXMLCommandParser::PreParseAnimation );
	AddXMLPreFunctor( "job-modifications-animations-animation-animationname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-animations-animation-cameraname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-animations-animation-anglecount", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-animations-animation-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-deleteallanimation", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-scales", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-scales-scale", &orthoXMLCommandParser::PreParseScale );
	AddXMLPreFunctor( "job-modifications-scales-scale-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-scales-scale-xvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-scales-scale-yvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-scales-scale-zvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-deletecharacters", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-deletecharacters-character", &orthoXMLCommandParser::PreParseDeleteCharacter );
	AddXMLPreFunctor( "job-modifications-deletecharacters-character-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-rotates", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-rotates-rotate", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera", &orthoXMLCommandParser::PreParseChangeCamera );
	AddXMLPreFunctor( "job-modifications-cameras-camera-cameraname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-move", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-move-xvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-move-yvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-move-zvalue", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-orbit", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-fov", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-targetjoint", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-cameras-camera-targetobject", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-expressions", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-expressions-expression", &orthoXMLCommandParser::PreParseExpression );
	AddXMLPreFunctor( "job-modifications-expressions-expression-name", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-expressions-expression-filename", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-modifications-expressions-expression-weight", &orthoXMLCommandParser::ParseDefault );

	AddXMLPostFunctor( "job-modifications", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-attachments", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-attachments-attachment", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-attachments-attachment-name", &orthoXMLCommandParser::ParseName );
	AddXMLPostFunctor( "job-modifications-attachments-attachment-joint", &orthoXMLCommandParser::ParseJointName );
	AddXMLPostFunctor( "job-modifications-attachments-attachment-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-detachments", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-detachments-detachment", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-detachments-detachment-name", &orthoXMLCommandParser::ParseName );
	AddXMLPostFunctor( "job-modifications-detachments-detachment-joint", &orthoXMLCommandParser::ParseJointName );
	AddXMLPostFunctor( "job-modifications-detachments-detachment-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-showcharacters", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-showcharacters-character", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-showcharacters-character-name", &orthoXMLCommandParser::ParseName );
	AddXMLPostFunctor( "job-modifications-showcharacters-character-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-hidecharacters", &orthoXMLCommandParser::ParseDefault ) ;
	AddXMLPostFunctor( "job-modifications-hidecharacters-character", &orthoXMLCommandParser::PostParse ) ;
	AddXMLPostFunctor( "job-modifications-hidecharacters-character-name", &orthoXMLCommandParser::ParseName ) ;
	AddXMLPostFunctor( "job-modifications-hidecharacters-character-filename", &orthoXMLCommandParser::ParseFileName ) ;
	AddXMLPostFunctor( "job-modifications-textures", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-textures-texture", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-textures-texture-shader", &orthoXMLCommandParser::ParseShaderName );
	AddXMLPostFunctor( "job-modifications-textures-texture-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-textures-texture-layer", &orthoXMLCommandParser::ParseLayerName );
	AddXMLPostFunctor( "job-modifications-materials", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-materials-material", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-materials-material-shader", &orthoXMLCommandParser::ParseShaderName );
	AddXMLPostFunctor( "job-modifications-materials-material-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-colors", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-colors-color", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-colors-color-shader", &orthoXMLCommandParser::ParseShaderName );
	AddXMLPostFunctor( "job-modifications-colors-color-value", &orthoXMLCommandParser::ValueParseColorValue );
	AddXMLPostFunctor( "job-modifications-animations", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-animations-animation", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-animations-animation-animationname", &orthoXMLCommandParser::ParseAnimationName );
	AddXMLPostFunctor( "job-modifications-animations-animation-cameraname", &orthoXMLCommandParser::ParseCameraName );
	AddXMLPostFunctor( "job-modifications-animations-animation-anglecount", &orthoXMLCommandParser::ParseDirections );
	AddXMLPostFunctor( "job-modifications-animations-animation-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-deleteallanimation", &orthoXMLCommandParser::PostParseDeleteAnimation );
	AddXMLPostFunctor( "job-modifications-scales", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-scales-scale", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-scales-scale-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-scales-scale-xvalue", &orthoXMLCommandParser::ValueParseXValue );
	AddXMLPostFunctor( "job-modifications-scales-scale-yvalue", &orthoXMLCommandParser::ValueParseYValue );
	AddXMLPostFunctor( "job-modifications-scales-scale-zvalue", &orthoXMLCommandParser::ValueParseZValue );
	AddXMLPostFunctor( "job-modifications-deletecharacters", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-deletecharacters-character", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-deletecharacters-character-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-rotates", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-rotates-rotate", &orthoXMLCommandParser::PostParseRotate );
	AddXMLPostFunctor( "job-modifications-cameras", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-cameras-camera", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-cameras-camera-cameraname", &orthoXMLCommandParser::ParseCameraName );
	AddXMLPostFunctor( "job-modifications-cameras-camera-move", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-cameras-camera-move-xvalue", &orthoXMLCommandParser::ValueParseXValue );
	AddXMLPostFunctor( "job-modifications-cameras-camera-move-yvalue", &orthoXMLCommandParser::ValueParseYValue );
	AddXMLPostFunctor( "job-modifications-cameras-camera-move-zvalue", &orthoXMLCommandParser::ValueParseZValue );
	AddXMLPostFunctor( "job-modifications-cameras-camera-orbit", &orthoXMLCommandParser::ValueParseCameraOrbit );
	AddXMLPostFunctor( "job-modifications-cameras-camera-fov", &orthoXMLCommandParser::ParseCameraFOV );
	AddXMLPostFunctor( "job-modifications-cameras-camera-targetjoint", &orthoXMLCommandParser::ParseJointName );
	AddXMLPostFunctor( "job-modifications-cameras-camera-targetobject", &orthoXMLCommandParser::ParseObjectName );
	AddXMLPostFunctor( "job-modifications-expressions", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-modifications-expressions-expression", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-modifications-expressions-expression-name", &orthoXMLCommandParser::ParseName );
	AddXMLPostFunctor( "job-modifications-expressions-expression-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-modifications-expressions-expression-weight", &orthoXMLCommandParser::ParseWeight );

	//RenderRequests Block
	AddXMLPreFunctor( "job-renderrequest", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame", &orthoXMLCommandParser::PreParseFrame );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-imageformat", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-maskformat", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-cameraname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-height", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-width", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-startheight", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-startwidth", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-frames-frame-time", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence", &orthoXMLCommandParser::PreParseSequence );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-imageformat", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-maskformat", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-cameraname", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-height", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-width", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-renderrequest-sequences-sequence-framerate", &orthoXMLCommandParser::ParseDefault );

	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-framerate", &orthoXMLCommandParser::ParseFrameRate );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-width", &orthoXMLCommandParser::ParseWidth );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-height", &orthoXMLCommandParser::ParseHeight );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-cameraname", &orthoXMLCommandParser::ParseCameraName );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-maskformat", &orthoXMLCommandParser::ParseMaskFormat );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence-imageformat", &orthoXMLCommandParser::ParseImageFormat );
	AddXMLPostFunctor( "job-renderrequest-sequences-sequence", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-renderrequest-sequences", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-time", &orthoXMLCommandParser::ParseTime );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-startwidth", &orthoXMLCommandParser::ParseStartWidth );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-startheight", &orthoXMLCommandParser::ParseStartHeight );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-width", &orthoXMLCommandParser::ParseWidth );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-height", &orthoXMLCommandParser::ParseHeight );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-cameraname", &orthoXMLCommandParser::ParseCameraName );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-maskformat", &orthoXMLCommandParser::ParseMaskFormat );
	AddXMLPostFunctor( "job-renderrequest-frames-frame-imageformat", &orthoXMLCommandParser::ParseImageFormat );
	AddXMLPostFunctor( "job-renderrequest-frames-frame", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-renderrequest-frames", &orthoXMLCommandParser::ParseDefault );
	AddXMLPostFunctor( "job-renderrequest", &orthoXMLCommandParser::ParseDefault );

	//CMS Block
	AddXMLPreFunctor( "job-cms", &orthoXMLCommandParser::ParseDefault );
	AddXMLPreFunctor( "job-cms-savexmlscene", &orthoXMLCommandParser::PreParseSaveXMLScene );
	AddXMLPreFunctor( "job-cms-savescene", &orthoXMLCommandParser::PreParseSaveScene );
	AddXMLPreFunctor( "job-cms-savexmlcharacter", &orthoXMLCommandParser::PreParseSaveXMLCharacter );
	AddXMLPreFunctor( "job-cms-savexmlcharacter-filename", &orthoXMLCommandParser::ParseDefault );

	AddXMLPostFunctor( "job-cms-savexmlcharacter-filename", &orthoXMLCommandParser::ParseFileName );
	AddXMLPostFunctor( "job-cms-savexmlcharacter", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-cms-savescene", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-cms-savexmlscene", &orthoXMLCommandParser::PostParse );
	AddXMLPostFunctor( "job-cms", &orthoXMLCommandParser::ParseDefault );

	AddXMLPostFunctor( "job", &orthoXMLCommandParser::PostParseJob );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
orthoXMLCommandParser::~orthoXMLCommandParser()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoXMLCommandParser::AddXMLPreFunctor( const std::string& name, CallbackPtr fptr )
{
	m_PreParseMap[ name ] = fptr;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoXMLCommandParser::AddXMLPostFunctor( const std::string& name, CallbackPtr fptr )
{
	m_PostParseMap[ name ] = fptr;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoXMLCommandParser::Clear()
{
	m_Nesting.Clear();
	if( !m_TaskStack.empty())
	{
		DBG_ERROR0( "Left over tasks in stack, previous parsing must have failed!" );
		envSTLHelpers::DeleteContainer( m_TaskStack );
	}
}

//--------------------------------------------------------------------
//	ParseBlock - actually parse through the block, changing
//	the XMLString.  It returns true if block was parsed successfully.
//--------------------------------------------------------------------
bool orthoXMLCommandParser::ParseBlock( std::string& io_XMLString )
{
	bool rval = true;
	//
	//	Parse out the commands
	//
	while (!io_XMLString.empty())
	{
		switch( orthoXMLStringUtil::returnTokenXML( io_XMLString, m_Token ) )
		{
			case orthoXMLStringUtil::e_XMLElement:
			{
				//convert element to lower case
				int len = m_Token.size() + 1; //include NULL
				if( len > 1 ) _strlwr_s( const_cast<char*>(m_Token.c_str()), len );	//modify string in place, overriding const

				m_Nesting.Push( m_Token );
				StringFunctorHashMap::const_iterator citr = m_PreParseMap.find( m_Nesting.String() );
				if( citr != m_PreParseMap.end() )
				{
					if( !(this->*citr->second)( io_XMLString ))
					{
						DBG_WARNING2("Unhandled PreFunctor of block (%s) in (%s)", m_Token.c_str(), m_Nesting.String().c_str());
						rval = false;
					}
				}
				else
				{
					DBG_WARNING2("XML: Unknown Begin block (%s) in (%s)", m_Token.c_str(), m_Nesting.String().c_str() );
					rval = false;
				}
				rval &= ParseBlock( io_XMLString );	//process nested block, clear return flag to propagate failures
				break;
			}
			case orthoXMLStringUtil::e_XMLValue:
			{
				m_Value = m_Token;
				break;
			}
			case orthoXMLStringUtil::e_XMLEndElement:
			{
				//convert element to lower case
				int len = m_Token.size() + 1; //include NULL
				if( len > 1 ) _strlwr_s( const_cast<char*>(m_Token.c_str()), len );	//modify string in place, overriding const

				if( m_Token == m_Nesting.Top() )
				{
					StringFunctorHashMap::const_iterator citr = m_PostParseMap.find( m_Nesting.String() );
					if( citr != m_PostParseMap.end() )
					{
						if( !(this->*citr->second)( m_Value ))
						{
							DBG_WARNING2("Unhandled postFunctor of block (%s) in (%s)", m_Token.c_str(), m_Nesting.String().c_str());
							rval = false;
						}
					}
					else
					{
						DBG_WARNING2("XML: Unknown End block (%s) in (%s)", m_Token.c_str(), m_Nesting.String().c_str() );
						rval = false;
					}
					m_Nesting.Pop();
					return rval;
				}
				else
				{
					DBG_ERROR2("Mismatched End Block (%s) for Block (%s)", m_Token.c_str(), m_Nesting.Top().c_str());
				}
				break;
			}
			case orthoXMLStringUtil::e_XMLError:
			{
				DBG_ERROR2("Invalid syntax parsing XML token (%s) from (%s)", m_Token.c_str(), io_XMLString.c_str());
				break;
			}
		}
	}
	return rval;
}

//=========================================================================
//------XML Pre Functors------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PreParseJob( std::string& io_XMLString )
{
	m_bJobStarted = true;
	orthoRequestTaskUtil::StartNewJob();
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PreParseBaseModel( std::string& io_XMLString )
{
	orthoObjectsRequestModelsTask* pTask = dynamic_cast<orthoObjectsRequestModelsTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		int count = pTask->m_Tasks.size();
		pTask->m_Tasks.resize( count+1 );
		pTask->m_Tasks.back().m_bIsBaseModel = true;
	}
	else
	{
		DBG_ERROR0("ModelsTask is NULL -- This shouldn't happen!");
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PreParseModel( std::string& io_XMLString )
{
	orthoObjectsRequestModelsTask* pTask = dynamic_cast<orthoObjectsRequestModelsTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		int count = pTask->m_Tasks.size();
		pTask->m_Tasks.resize( count+1 );
		pTask->m_Tasks.back().m_bIsBaseModel = false;
	}
	else
	{
		DBG_ERROR0("ModelsTask is NULL -- This should happen!");
		return false;
	}
	return true;
}

//=========================================================================
//------XML Value Functors------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseModelName( std::string& i_Value )
{
	orthoObjectsRequestModelsTask* pTask = dynamic_cast<orthoObjectsRequestModelsTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->m_Tasks.back().SetName( i_Value );
		return true;
	}
	DBG_ERROR1( "Name block (%s) found outside of Models!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseModelFileName( std::string& i_Value )
{
	orthoObjectsRequestModelsTask* pTask = dynamic_cast<orthoObjectsRequestModelsTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->m_Tasks.back().SetFileName( i_Value );
		return true;
	}
	DBG_ERROR1( "FileName block (%s) found outside of Models!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseColorValue( std::string& i_Value )
{
	int r,g,b;
	orthoModificationRequestTask* pTask = dynamic_cast<orthoModificationRequestTask*>(m_TaskStack.back());
	if (pTask && (sscanf(i_Value.c_str(),"0x%2x%2x%2x", &r, &g, &b) == 3))
	{
		maFloatRGBA color(((float) r)/(float) 256.0,
			((float) g)/(float) 256.0,
			((float) b)/(float) 256.0,
			1.0f);
		pTask->SetColor( color );
		return true;
	}
	DBG_ERROR1( "Invalid Color Value block (%s)!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseXValue( std::string& i_Value )
{
	orthoModificationRequestTask* pTask = dynamic_cast<orthoModificationRequestTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->GetVector().SetX( (float)atof(i_Value.c_str()) );
		return true;
	}
	DBG_ERROR1( "XValue block (%s) found outside of Modifications!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseYValue( std::string& i_Value )
{
	orthoModificationRequestTask* pTask = dynamic_cast<orthoModificationRequestTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->GetVector().SetY( (float)atof(i_Value.c_str()) );
		return true;
	}
	DBG_ERROR1( "YValue block (%s) found outside of Modifications!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseZValue( std::string& i_Value )
{
	orthoModificationRequestTask* pTask = dynamic_cast<orthoModificationRequestTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->GetVector().SetZ( (float)atof(i_Value.c_str()) );
		return true;
	}
	DBG_ERROR1( "ZValue block (%s) found outside of Modifications!", i_Value.c_str() );
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::ValueParseCameraOrbit( std::string& i_Value )
{
	orthoModificationRequestTask* pTask = dynamic_cast<orthoModificationRequestTask*>(m_TaskStack.back());
	if (pTask != NULL)
	{
		pTask->SetCameraOrbit( (float)(atof(i_Value.c_str()) * maConstants::c_dAngleToRad) );
		return true;
	}
	DBG_ERROR1( "CameraOrbit block (%s) found outside of Modifications!", i_Value.c_str() );
	return false;
}

//=========================================================================
//------XML Post Functors------------------------------
//#define SUPRESS_TASKS		//supress tasks for parse testing

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PostParseJob( std::string& i_Value )
{
	if( m_bJobStarted )
	{
		m_bJobStarted = false;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PostParseRotate( std::string& i_Value )
{
	if( !m_bJobStarted )
	{
		DBG_ERROR0( "Job Not started!" );
		return false;
	}
#ifndef SUPRESS_TASKS
	orthoModificationRequestRotateCharacterTask* pTask = new orthoModificationRequestRotateCharacterTask();
	pTask->SetAngle( (float)(atof(i_Value.c_str()) * maConstants::c_dAngleToRad) );
	orthoRequestTaskUtil::AddTaskToCurrentJob( pTask );
#endif
	return true;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PostParseDeleteAnimation( std::string& i_Value )
{
	if( !m_bJobStarted )
	{
		DBG_ERROR0( "Job Not started!" );
		return false;
	}

#ifndef SUPRESS_TASKS
	orthoRequestTaskUtil::AddTaskToCurrentJob( new orthoModificationRequestAnimationDeleteAllTask() );
#endif
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoXMLCommandParser::PostParse( std::string& i_Value )
{
	if( !m_bJobStarted )
	{
		DBG_ERROR0( "Job Not started!" );
		return false;
	}
	if( !m_TaskStack.empty() )
	{
		if( m_TaskStack.back()->GetTaskName() == m_Nesting.Top() )
		{
#ifndef SUPRESS_TASKS
			orthoRequestTaskUtil::AddTaskToCurrentJob( m_TaskStack.back() );	
#else
			delete m_TaskStack.back();
#endif
			m_TaskStack.pop_back();
			return true;
		}
		DBG_ERROR0( "Post Block name mismatch!" );
	}
	return false;
}
