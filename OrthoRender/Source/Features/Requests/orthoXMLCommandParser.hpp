/*****************************************************************************
**  orthoXMLCommandParser.hpp
**
**      Recursively process the XML Block commands via Function Dictionary lookup.
**      All XML elements are case insensitive.
**      The parser operates by mapping a Function Object (Functor) to the Begin and End
**      XML elements.
**      --Pre Functors are used to create task objects and setup parameters for successive
**        elements to add to in a hierarchy.
**      --Value Functors (similar to post functors) are almost always use to handle a End element where
**        the previous acquired Value is set to the bound Functor (and sometimes converted)
**      --Post Functors operate on the End element and commit the task, most use the common PostParse Functor
**
**	How to add a new XML value:
**		An XML Value is bound by a pre and post element that needs to be handled,
**		so 3 XML elements total are parsed.  Only the Post requires a functor since the
**		Value acquired during parsing is passed to it.  This means the pre Functor
**		get set to default.  To add a new Value called "country" to the "scene" block do the following.
**
**	1. Insert AddXMLPreFunctor( "job-scene-country", &orthoXMLCommandParser::ParseDefault ) in orthoXMLCommandParser
**	   constructor.  Note that the placement is only for readability and isn't a requirement. Also use the default
**	   functor "ParseDefault". Last make sure that the string is lower case.
**	2. Add MAKE_VALUE_TASK_FUNCTION_STRING( Country ) to the "XML Value Functors" class definitions.
**		Since the new value is a string we use the helper macro MAKE_VALUE_TASK_FUNCTION_STRING to create the functor.
**	3. Add std::string m_Country to the orthoSceneRequestLoadTask class. This is where the value will be stored when parsed.
**	4. Add virtual void SetCountry( const std::string& i_Country ){ m_Country = i_Country; } to orthoSceneRequestLoadTask class.
**	   This is the function that will be called by the post functor to convert/store the value.
**	5. Add virtual void SetCountry( const std::string& ){ DBG_ERROR( "Country value has invalid placement." ); } to orthoRequestTask class.
**	   Since our "Set" functions are virtual we need a placeholder in the base class.  This is also a new function, otherwise you can reuse existing ones.
**	   6. Finally insert AddXMLPostFunctor( "job-scene-country", &orthoXMLCommandParser::ParseCountry ) in orthoXMLCommandParser
**	   constructor. Note everything from (1).  This time we use the Functor that we created.
**
**	How to add a new XML Block:
**		A Block holds one or more values and is nested within a category (i.e. Scene, Modifications, etc...)
**		To handle this we need to create a new task. To add a new Block called "bind" to the Modification Category do the following.
**	1. Create a new class orthoModificationRequestBindTask in OrthoModificationRequestTask.hpp.
**	2. Set the base class constructor name to "bind" (make sure it's lower case) and set the ID to BIND_ID
**	3. Create and Place the ID in the enumerated list in OrthoRequestTask class where its sort priority lies.
**	4. Add valiables to the new orthoModificationRequestBindTask, see previous "how to" to implement these.
**	5. Implement the ExecuteTask function.
**	6. Add MAKE_PRE_TASK_FUNCTION_CREATE( Bind, new orthoModificationRequestBindTask() ); to the parser class.
**	   This creates the class when the begin XML element is parsed.  We use this for the pre-functor. 
**	   Note: make sure that the label is unique.
**	7. Insert AddXMLPreFunctor( "job-modifications-bind", &orthoXMLCommandParser::PreParseBind ) in orthoXMLCommandParser
**	   constructor.  We use the functor created in (6)
**	8. Finally insert AddXMLPostFunctor( "job-modifications-bind", &orthoXMLCommandParser::PostParse ) in orthoXMLCommandParser
**	   constructor.  Here we use the common PostParse functor to insert the task into the queue for later execution (by means of ExecuteTask).
**
**  Author - John Schwab
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_XMLCOMMANDPARSER_HPP
#error orthoXMLCommandParser.hpp multiply included
#endif
#define ORTHO_XMLCOMMANDPARSER_HPP

#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#ifndef ORTHO_XMLSTRINGUTIL_HPP
#include "Features/Requests/orthoXMLStringUtil.hpp"
#endif

//---------Request Tasks----------------------
#include "Features/Requests/Tasks/orthoJobInfoRequestTask.hpp"
#include "Features/Requests/Tasks/orthoSceneRequestTask.hpp"
#include "Features/Requests/Tasks/orthoObjectsRequestTask.hpp"
#include "Features/Requests/Tasks/orthoCMSRequestTask.hpp"
#include "Features/Requests/Tasks/orthoRenderRequestTask.hpp"
#include "Features/Requests/Tasks/orthoModificationRequestTask.hpp"

#include <hash_map>

//helper function building templates (how to make these into actual templates?)

//Used to declare a (Pre) Functor that creates a new Task inherited from orthoRequestTask
// This macro makes functors with the name bool PreParseXXXXXX() with parameters of std::string& io_XMLString
#define MAKE_PRE_TASK_FUNCTION_CREATE( label, className )\
bool orthoXMLCommandParser::PreParse##label( std::string& io_XMLString ){\
	m_TaskStack.push_back( ##className );\
	return true;}\

//Used to declare a (Post) Functor that calls a Set function for String Parameters of base class orthoRequestTask
// This macro makes functors with the name bool ParseXXXXXX() with parameters of std::string& i_Value
#define MAKE_VALUE_TASK_FUNCTION_STRING( label )\
bool Parse##label( std::string& i_Value ){\
	if( !m_TaskStack.empty() ){	m_TaskStack.back()->Set##label( i_Value );	return true; }\
	return false;}

//Used to declare a (Post) Functor that calls a Set function for Int Parameters of base class orthoRequestTask
// This macro makes functors with the name bool ParseXXXXXX() with parameters of std::string& i_Value
#define MAKE_VALUE_TASK_FUNCTION_INT( label )\
	bool Parse##label( std::string& i_Value ){\
	if( !m_TaskStack.empty() ){	m_TaskStack.back()->Set##label( atoi(i_Value.c_str()) );	return true; }\
	return false;}

//Used to declare a (Post) Functor that calls a Set function for Float Parameters of base class orthoRequestTask
// This macro makes functors with the name bool ParseXXXXXX() with parameters of std::string& i_Value
#define MAKE_VALUE_TASK_FUNCTION_FLOAT( label )\
	bool Parse##label( std::string& i_Value ){\
	if( !m_TaskStack.empty() ){	m_TaskStack.back()->Set##label( (float)atof(i_Value.c_str()) );	return true; }\
	return false;}

//============================================================================
//  Class that parses a command XML File and creates Tasks using a Functor Dictionary lookup
//============================================================================
class orthoXMLCommandParser
{
public:
	typedef bool(orthoXMLCommandParser::*CallbackPtr)(std::string&);		//member function callback signature ("this" pointer implied on call)
	typedef stdext::hash_map<std::string, CallbackPtr > StringFunctorHashMap;	//Dictionary to Functor Hash

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	orthoXMLCommandParser();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~orthoXMLCommandParser();

	//use to validate an XML element that requires no handling
	bool ParseDefault( std::string& io_XMLString ){ return true; } //always returns true

	//--------------------------------------------------------------------
	//	ParseBlock - actually parse through the blocks, changing
	//	the XMLString.  It returns true if the string was parsed successfully.
	//--------------------------------------------------------------------
	bool ParseBlock( std::string& io_XMLString );

	//These functions add new elements to the command parser, make sure to imply the hierarchy with (-) seperators
	//make sure strings are lower case
	void AddXMLPreFunctor( const std::string& name, CallbackPtr fptr );		//Operate on and XML Begin element
	void AddXMLPostFunctor( const std::string& name, CallbackPtr fptr );	//Operate on and XML End element

	void Clear();		//clears the parser of any internal data

protected:
	//-------------XML Pre Functors-----
	bool PreParseJob( std::string& io_XMLString );	//job has started (required)

	MAKE_PRE_TASK_FUNCTION_CREATE( JobInfo, new orthoJobInfoRequestTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Scene, new orthoSceneRequestLoadTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Models, new orthoObjectsRequestModelsTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( SaveXMLCharacter, new orthoCMSRequestSaveXMLCharacterTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( SaveScene, new orthoCMSRequestSaveSceneTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( SaveXMLScene, new orthoCMSRequestSaveXMLSceneTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Frame, new orthoRenderRequestFrameTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Sequence, new orthoRenderRequestSequenceTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Texture, new orthoModificationRequestTextureTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Material, new orthoModificationRequestMaterialTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Color, new orthoModificationRequestColorTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Animation, new orthoModificationRequestAnimationAddTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Scale, new orthoModificationRequestScaleCharacterTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( DeleteCharacter, new orthoModificationRequestDeleteCharacterTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( ChangeCamera, new orthoModificationRequestChangeCameraTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Expression, new orthoModificationRequestExpressionTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Attachment, new orthoModificationRequestAttachmentTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( Detachment, new orthoModificationRequestDetachmentTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( ShowCharacter, new orthoModificationRequestShowCharacterTask() );
	MAKE_PRE_TASK_FUNCTION_CREATE( HideCharacter, new orthoModificationRequestHideCharacterTask() );

	bool PreParseBaseModel( std::string& io_XMLString );
	bool PreParseModel( std::string& io_XMLString );

	//-------------XML Value Functors---------
	MAKE_VALUE_TASK_FUNCTION_STRING( Name );
	MAKE_VALUE_TASK_FUNCTION_STRING( JobName );
	MAKE_VALUE_TASK_FUNCTION_STRING( FileName );
	MAKE_VALUE_TASK_FUNCTION_STRING( ImageFormat );
	MAKE_VALUE_TASK_FUNCTION_STRING( MaskFormat );
	MAKE_VALUE_TASK_FUNCTION_STRING( CameraName );
	MAKE_VALUE_TASK_FUNCTION_STRING( JointName );
	MAKE_VALUE_TASK_FUNCTION_STRING( ShaderName );
	MAKE_VALUE_TASK_FUNCTION_STRING( LayerName );
	MAKE_VALUE_TASK_FUNCTION_STRING( AnimationName );
	MAKE_VALUE_TASK_FUNCTION_STRING( ObjectName );
	MAKE_VALUE_TASK_FUNCTION_INT( Height );
	MAKE_VALUE_TASK_FUNCTION_INT( Width );
	MAKE_VALUE_TASK_FUNCTION_INT( StartHeight );
	MAKE_VALUE_TASK_FUNCTION_INT( StartWidth );
	MAKE_VALUE_TASK_FUNCTION_INT( Directions );
	MAKE_VALUE_TASK_FUNCTION_FLOAT( Time );
	MAKE_VALUE_TASK_FUNCTION_FLOAT( FrameRate );
	MAKE_VALUE_TASK_FUNCTION_FLOAT( CameraFOV );
	MAKE_VALUE_TASK_FUNCTION_FLOAT( Weight );

	bool ValueParseModelName( std::string& i_Value );
	bool ValueParseModelFileName( std::string& i_Value );

	bool ValueParseColorValue( std::string& i_Value );
	bool ValueParseXValue( std::string& i_Value );
	bool ValueParseYValue( std::string& i_Value );
	bool ValueParseZValue( std::string& i_Value );
	bool ValueParseCameraOrbit( std::string& i_Value );

	//-------------XML Post Functors
	bool PostParse( std::string& i_Value );			//commit the task for this end block
	bool PostParseRotate( std::string& i_Value );
	bool PostParseDeleteAnimation( std::string& i_Value );

	bool PostParseJob( std::string& i_Value );		//job has ended (required)

private:
	bool							m_bJobStarted;	//valid if a job has been started, tasks fail to commit if not true

	std::string						m_Token;		//currently parsed XML token
	std::string						m_Value;		//last parsed XML value string

	StringFunctorHashMap			m_PreParseMap;	//hashed map of parse function pointers from names (When XML Enters)
	StringFunctorHashMap			m_PostParseMap;	//hashed map of parse function pointers from names (When XML Exits)

	StringStack						m_Nesting;		//A Stack of strings that handles the hierarchy and catenation of the strings
	std::vector<orthoRequestTask*>	m_TaskStack;	//A stack that holds tasks before they are committed. 
};
