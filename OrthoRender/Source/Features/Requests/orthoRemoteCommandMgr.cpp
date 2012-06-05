/*****************************************************************************
**  orthoRemoteCommandMgr.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include <windows.h>

#include "Features/Requests/orthoRemoteCommandMgr.hpp"

#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Capture/orthoPackage.hpp"
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"
#include "Features/Requests/orthoXMLStringUtil.hpp"
#include "MainApp/LightWaitMessage.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Core/App/appCharEvent.hpp"

//	library
#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Ma/maConstants.hpp"
#include "InputDI/In/inDeviceMgr.hpp"
#include "../3rdParty/zlib/zlib.h"

#include <sstream>

DWORD CheckInput( LPVOID param );

void LocalCharEventRequest::ReceiveCharEvent(appCharEvent& i_Event)
{
	if( !g_bRemoteRenderingCommand )	//only work in local mode
	{
		std::string workingString;

		itString::CharType Char = i_Event.GetChar();

		switch( Char )
		{
			//	tests:
			//		1, 2 - this will load empty scene and then the models (in new objects chunk)
			//	Scenes
			case 'a':{ workingString = "<Job><Scene><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\WS_M01_MAT_CURRENT.mab</FileName></Scene></Job>"; break;}
			case 's':{ workingString = "<Job><Scene><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\WS_F01_MAT_CURRENT.mab</FileName></Scene></Job>"; break;}
			case 'n':{ workingString = "<Job><Scene><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\Test.mab</FileName></Scene></Job>"; break;}
			case '1':{ workingString = "<Job><Scene><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\WS_MainScene_000.mab</FileName></Scene></Job>"; break;} // empty scene - no chars

			//	Base Models
			case '2':{ workingString = "<Job><Objects><Models><BaseModel><Name>WS_M01_Body_MAT_Current.chx</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Body_MAT_Current.chx</FileName></BaseModel></Models></Objects></Job>"; break;}
			case '3':{ workingString = "<Job><Objects><Models><Model><Name>Blah</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Bottom_DressSlacks01_MAT_current.chx</FileName></Model><Model><Name>Blah2</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></Model></Models></Objects></Job>"; break;}

			// Modifications
	//		case '3':{ workingString = "<Job><Modifications><Attachments><Attachment><Name>WS_M01_Body_MAT_Current.chx</Name><Joint>Ortho_brow_L_1_jnt</Joint><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Glasses_Aviator_MAT_Current.chx</FileName></Attachment></Attachments></Modifications></Job>"; break;}
			case '4':{ workingString = "<Job><Modifications><Detachments><Detachment><Name>WS_M01_Body_MAT_Current.chx</Name><Joint>Ortho_brow_L_1_jnt</Joint><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Glasses_Aviator_MAT_Current.chx</FileName></Detachment></Detachments></Modifications></Job>"; break;}
			case '5':{ workingString = "<Job><Modifications><Textures><Texture><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader><Layer>Alpha</Layer><FileName>WS_M01_Tie_Normal.dds</FileName></Texture></Textures></Modifications></Job>"; break;}
			case '6':{ workingString = "<Job><Modifications><Materials><Material><Shader>WS_M01_Hair01_Shader</Shader><FileName>WS_M01\\WS_M01_Hairs\\WS_M01_Hair01_Red.mtl</FileName></Material><Material><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader><FileName>WS_M01\\WS_M01_Tops\\WS_M01_Top_RelaxShirtSuspend\\WS_M01_Top_RlxShirt_Blue.mtl</FileName></Material></Materials></Modifications></Job>"; break;}
			case '7':{ workingString = "<Job><Modifications><Colors><Color><Shader>WS_M01_Hair01_Shader</Shader><Value>0x00FF00</Value></Color></Colors></Modifications></Job>"; break;}
			case '8':{ workingString = "<Job><Modifications><ShowCharacters><Character><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></Character></ShowCharacters></Modifications></Job>"; break;}
			case '9':{ workingString = "<Job><Modifications><HideCharacters><Character><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></Character></HideCharacters></Modifications></Job>"; break;}
			case '0':{ workingString = "<Job><Modifications><Animations><Animation><AnimationName>SuperIdle</AnimationName><FileName>WS_M01_ANM_thumbs_up.cha</FileName><AngleCount>8</AngleCount><CameraName>FRONTAL</CameraName></Animation></Animations></Modifications></Job>"; break;}
			case appCharEvent::e_BACKSPACE:{ workingString = "<Job><Modifications><DeleteAllAnimation></DeleteAllAnimation></Modifications></Job>"; break;}
			case 'q':{ workingString = "<Job><Modifications><Textures><Texture><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader><Layer>Diffuse</Layer><FileName>WS_M01_Tie_Normal.dds</FileName></Texture></Textures></Modifications></Job>"; break;}
			case 'w':{ workingString = "<Job><Modifications><Textures><Texture><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader><Layer>Alpha</Layer><FileName>WS_M01_Tie_Normal.dds</FileName></Texture></Textures></Modifications></Job>"; break;}
			case 'e':{ workingString = "<Job><Modifications><Textures><Texture><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader><Layer>Specular</Layer><FileName>WS_M01_Tie_Normal.dds</FileName></Texture></Textures></Modifications></Job>"; break;}
			case 'f':{ workingString = "<Job><Modifications><Expressions><Expression><FileName>WS_M01_Tie_Normal.dds</FileName></Expression></Expressions></Modifications></Job>"; break;}
			case 'd':{ workingString = "<Job><Modifications><DeleteCharacters><Character><FileName>WS_M01_Hair01_MAT_Current.chx</FileName></Character></DeleteCharacters></Modifications></Job>"; break;}
			case 'g':{ workingString = "<Job><Modifications><Scales><Scale><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\test.mab</FileName><XValue>0.5</XValue><YValue>0.5</YValue><ZValue>0.5</ZValue></Scale></Scales></Modifications></Job>"; break;}
			case 'b':{ workingString = "<Job><Modifications><Rotates><Rotate>45</Rotate></Rotates></Modifications></Job>"; break;}

			//camera modifications
			case appCharEvent::e_END:	//camera move
			{ workingString = "<Job><Modifications><Cameras><Camera><CameraName>FRONTAL</CameraName><Move><XValue>20.0</XValue><YValue>133.0</YValue><ZValue>450.0</ZValue></Move></Camera></Cameras></Modifications></Job>"; break;}
			case appCharEvent::e_DOWN:	//camera Orbit
			{ workingString = "<Job><Modifications><Cameras><Camera><CameraName>FRONTAL</CameraName><Orbit>45</Orbit></Camera></Cameras></Modifications></Job>"; break;}
			case appCharEvent::e_PAGEDOWN:	//camera FOV
			{ workingString = "<Job><Modifications><Cameras><Camera><CameraName>FRONTAL</CameraName><FOV>120</FOV></Camera></Cameras></Modifications></Job>"; break;}
			case appCharEvent::e_LEFT:	//camera Target Joint
			{ workingString = "<Job><Modifications><Cameras><Camera><CameraName>FRONTAL</CameraName><TargetJoint>Ortho_brow_L_1_jnt</TargetJoint></Camera></Cameras></Modifications></Job>"; break;}
			case appCharEvent::e_RIGHT:	//camera Target Object
			{ workingString = "<Job><Modifications><Cameras><Camera><CameraName>FRONTAL</CameraName><TargetObject>WS_M01_Glasses_Aviator_MAT_Current.chx</TargetObject></Camera></Cameras></Modifications></Job>"; break;}

			// Render Requests (don't change these)
			case '-':{ workingString = "<Job><RenderRequest><Sequences><Sequence><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><FrameRate>12.0</FrameRate><CameraName>FRONTAL</CameraName><Height>512</Height><Width>512</Width></Sequence></Sequences></RenderRequest></Job>"; break;}
			case '=':{ workingString = "<Job><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>FRONTAL</CameraName><Height>256</Height><Width>256</Width><StartWidth>256</StartWidth><StartHeight>256</StartHeight><Time>0.0</Time></Frame></Frames></RenderRequest></Job>"; break;}
										//progressive {workingString = "<Job><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>PNG</MaskFormat><CameraName>FACE</CameraName><Height>256</Height><Width>256</Width><StartWidth>64</StartWidth><StartHeight>64</StartHeight></Frame></Frames></RenderRequest></Job>"; break;}

			// Content Management Systems (CMS)  (don't change these)
			case ';':{ workingString = "<Job><CMS><SaveXMLScene></SaveXMLScene></CMS></Job>"; break;}
			case '\'':{ workingString = "<Job><CMS><SaveScene></SaveScene></CMS></Job>"; break;}
			case 'l':{ workingString = "<Job><CMS><SaveXMLCharacter><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></SaveXMLCharacter></CMS></Job>"; break;}

			//	Misc tests
			//
			case 'z':{ workingString = "<Job><Scene><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\WS_MainScene_000.mab</FileName></Scene><Objects><Models><BaseModel><Name>WS_M01_Body_MAT_Current.chx</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Body_MAT_Current.chx</FileName></BaseModel><Model><Name>Blah</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Bottom_DressSlacks01_MAT_current.chx</FileName></Model><Model><Name>Blah2</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><Animations><Animation><AnimationName>Walk</AnimationName><FileName>WS_M01_ANM_walk.cha</FileName><AngleCount>8</AngleCount><CameraName>FRONTAL</CameraName></Animation></Animations></Modifications></Job>"; break;}
			case 'p':{ workingString = "<Job><JobInfo><JobName>job-9e2405d0-d313-4800-8bd5-bb145423938c</JobName></JobInfo><Scene><FileName>c:\\projects\\WallStreet\\Shots\\Stock\\WS_F01_RTR.mab</FileName><Cameras><Camera><CameraName>shirt</CameraName></Camera></Cameras></Scene><BaseModel></BaseModel><Modifications><Animations><Animation><FileName>WS_F01_ANM_idle.cha</FileName><AngleCount>8</AngleCount><CameraName>shirt</CameraName></Animation></Animations></Modifications><RenderRequest><Frames></Frames><Sequences><Sequence><CameraName>shirt</CameraName><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><FrameRate>6.0</FrameRate><Height>190</Height><Width>190</Width></Sequence></Sequences></RenderRequest></Job>"; break;}
			case 'v':{ workingString = "<Job><Scene><FileName>c:\\projects\\WallStreet\\Shots\\Stock\\WS_F01_RTR.mab</FileName></Scene></Job>"; break;}
						//{workingString = "<Job><Modifications><Scales><Scale><FileName>C:\\Projects\\WallStreet\\Shots\\Stock\\test.mab</FileName><XValue>0.5</XValue><YValue>0.5</YValue><ZValue>0.5</ZValue></Scale></Scales></Modifications></Job>"; break;}
			case 'y':{ workingString = "<Job><JobInfo><JobName>job-0ae95d0b-5e49-414f-b128-fc0bde30e001</JobName></JobInfo><Scene><FileName>C:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>ISOMETRIC</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>body</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_lg_MAT_Current.chx</FileName></BaseModel><Model><Name>head</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>tail</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Body_Krawk_Tail_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>8</AngleCount><CameraName>ISOMETRIC</CameraName></Animation></Animations><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Yellow.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Eye_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Body_Krawk_Tail_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>FRONTAL</CameraName><StartWidth>500</StartWidth><StartHeight>500</StartHeight><Height>500</Height><Width>500</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
						//{workingString = "<Job><JobInfo><JobName>job-0ae95d0b-5e49-414f-b128-fc0bde30e001</JobName></JobInfo><Scene><FileName>C:\\projects\\Neopia\\Shots\\Stock\\WN_BASE_RTR.mab</FileName><Cameras><Camera><CameraName>ISOMETRIC</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>body</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_lg_MAT_Current.chx</FileName></BaseModel><Model><Name>head</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>tail</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Body_Krawk_Tail_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>8</AngleCount><CameraName>ISOMETRIC</CameraName></Animation></Animations><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Yellow.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Eye_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Body_Krawk_Tail_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>FRONTAL</CameraName><StartWidth>500</StartWidth><StartHeight>500</StartHeight><Height>500</Height><Width>500</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}

			// test strings
			case 'j':{ workingString = "<Job><JobInfo><JobName>job-d66faca8-6643-4c64-be54-0f547317d27d</JobName></JobInfo><Scene><FileName>c:\\projects\\wallstreet\\Shots\\Stock\\WS_MainScene_000.mab</FileName></Scene><Models><BaseModel><Name>WS_F01_Body_MAT_Current.chx</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_F01\\Models\\WS_F01_Body_MAT_Current.chx</FileName></BaseModel><Model><Name>Blah</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_F01\\Models\\WS_F01_Bottom_DressSlacks01_MAT_current.chx</FileName></Model><Model><Name>Blah2</Name><FileName>C:\\Projects\\WallStreet\\Data\\Stock\\Characters\\WS_F01\\Models\\WS_F01_Hair01_MAT_Current.chx</FileName></Model></Models><Modifications><Animations><Animation><AnimationName>SuperIdle</AnimationName><FileName>WS_F01_ANM_thumbs_up.cha</FileName><AngleCount>8</AngleCount><CameraName>FRONTAL</CameraName></Animation></Animations></Modifications><RenderRequest><Sequences><Sequence><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><FrameRate>6.0</FrameRate><CameraName>FRONTAL</CameraName><Height>512</Height><Width>512</Width></Sequence></Sequences></RenderRequest></Job>"; break;}
			case 'm':{ workingString = "<Job><JobInfo><JobName>job-a7d66f5f-70c0-4330-a139-5b29310efbdd</JobName></JobInfo><Scene><FileName>c:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>head</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>WN_Krawk_small</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_sm_MAT_Current.chx</FileName></BaseModel><Model><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Model><Model><Name>WN_Head_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Model><Model><Name>WN_Tail_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Tail_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><DeleteAllAnimation></DeleteAllAnimation><Rotates><Rotate>0</Rotate></Rotates><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>1</AngleCount><CameraName>head</CameraName></Animation></Animations><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Blue.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Bottom\\WN_Bottom_PantsArmor01.mtl</FileName><Shader>WN_Bottom_PantsArmor01_Shader</Shader></Material><Material><FileName>Feet\\WN_Feet_BootsArmor01.mtl</FileName><Shader>WN_Feet_BootsArmor01_Shader</Shader></Material><Material><FileName>Hands\\WN_Hands_GlovesArmor01.mtl</FileName><Shader>WN_Hands_GlovesArmor01_Shader</Shader></Material><Material><FileName>Hat\\WN_Hat_Helmet01.mtl</FileName><Shader>WN_Hat_Helmet01_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eyes.mtl</FileName><Shader>WN_Head_Krawk_Eye_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_HairBlue.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_SkinBlue.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Orb.mtl</FileName><Shader>WN_Prop_MagicWand_Orb_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Staff.mtl</FileName><Shader>WN_Prop_MagicWand_Staff_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shield_Metal.mtl</FileName><Shader>WN_Prop_Shield_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Handle.mtl</FileName><Shader>WN_Prop_Shovel_Handle_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Shovel.mtl</FileName><Shader>WN_Prop_Shovel_Shovel_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Leaf.mtl</FileName><Shader>WN_Prop_Spear_Leaf_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Metal.mtl</FileName><Shader>WN_Prop_Spear_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Stick.mtl</FileName><Shader>WN_Prop_Spear_Stick_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Twine.mtl</FileName><Shader>WN_Prop_Spear_Twine_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Wrap.mtl</FileName><Shader>WN_Prop_Spear_Wrap_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Blade.mtl</FileName><Shader>WN_Prop_Sword_Blade_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Gold.mtl</FileName><Shader>WN_Prop_Sword_Gold_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Handle.mtl</FileName><Shader>WN_Prop_Sword_Handle_Shader</Shader></Material><Material><FileName>Krawk\\WN_Tail_Krawk_Blue.mtl</FileName><Shader>WN_Tail_Krawk_Shader</Shader></Material><Material><FileName>Top\\WN_Top_ShirtArmor01.mtl</FileName><Shader>WN_Top_ShirtArmor01_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>head</CameraName><StartWidth>250</StartWidth><StartHeight>250</StartHeight><Height>250</Height><Width>250</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}

			//	Applying alpha texture -- jacket will fail (on purpose)
			case 'h':
			 //					{workingString = "<Job><JobInfo><JobName>job-acdfc31a-c50b-41fb-a933-3bd31605333b</JobName></JobInfo><Scene><FileName>C:\\projects\\VMTV\\Shots\\Stock\\vMTV_MainScene_000.mab</FileName></Scene><Objects><Models><BaseModel><Name>body</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Body_MAT_Current.chx</FileName></BaseModel><Model><Name>bottom</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Bottom_Jeans01_MAT_Current.chx</FileName></Model><Model><Name>hair</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Hair01_MAT_Current.chx</FileName></Model><Model><Name>shoes</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Shoes_Tennis01_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><Materials><Material><FileName>MTV_M01\\MTV_M01_Bottoms\\MTV_M01_Bottom_Jeans01\\VLES_M01_Bottom_Jeans01_Blue.mtl</FileName><Shader>VMTV_M01_Bottom_Jean01_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Eyes\\MTV_M01_Eyeball.mtl</FileName><Shader>MTV_M01_Eyeball_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Eyes\\MTV_M01_EyeLash.mtl</FileName><Shader>MTV_M01_EyeLash_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Eyes\\MTV_M01_Iris_Blue.mtl</FileName><Shader>MTV_M01_Iris_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Eyes\\MTV_M01_Lens.mtl</FileName><Shader>MTV_M01_Lens_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Hair\\MTV_M01_Brows_Brown.mtl</FileName><Shader>MTV_M01_Brows_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Hair\\MTV_M01_Hair01_Blonde.mtl</FileName><Shader>MTV_M01_Hair01_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Shoes\\MTV_M01_Shoe_Sneaker\\MTV_M01_Shoe_Sneaker.mtl</FileName><Shader>MTV_M01_Shoe_Tennis01_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Skin\\MTV_M01_Body_Normal.mtl</FileName><Shader>MTV_M01_Body_Shader1</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Tops\\MTV_M01_Top_Jacket01\\MTV_M01_Top_Jacket01_Brown.mtl</FileName><Shader>MTV_M01_Jacket01_Shader</Shader></Material><Material><FileName>MTV_M01\\MTV_M01_Tops\\MTV_M01_Top_Jacket01\\MTV_M01_Top_JacketShirt_Blue.mtl</FileName><Shader>VMTV_M01_JacketShirt01_Shader1</Shader></Material></Materials><Textures><Texture><Shader>MTV_M01_Body_Shader1</Shader><FileName>VMTV_M01_Jacket01_Alpha.dds</FileName><Layer>alpha</Layer></Texture></Textures></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>FRONTAL</CameraName><StartWidth>500</StartWidth><StartHeight>500</StartHeight><Height>500</Height><Width>500</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
			 //					{workingString = "<Job><JobInfo><JobName>job-d5989898-a742-4b5b-b3f3-b94df9365d5b</JobName></JobInfo><Scene><FileName>c:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>head</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>WN_Krawk_small</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_sm_MAT_Current.chx</FileName></BaseModel><Model><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Model><Model><Name>WN_Head_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Model><Model><Name>WN_Tail_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Tail_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><DeleteAllAnimation/><Rotates><Rotate>90</Rotate></Rotates><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>1</AngleCount><CameraName>head</CameraName></Animation></Animations><HideCharacters><Character><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Character><Character><Name>WN_Head_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Character><Character><Name>WN_Tail_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Tail_Krawk_MAT_Current.chx</FileName></Character><Character><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Character></HideCharacters><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Blue.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Bottom\\WN_Bottom_PantsArmor01.mtl</FileName><Shader>WN_Bottom_PantsArmor01_Shader</Shader></Material><Material><FileName>Feet\\WN_Feet_BootsArmor01.mtl</FileName><Shader>WN_Feet_BootsArmor01_Shader</Shader></Material><Material><FileName>Hands\\WN_Hands_GlovesArmor01.mtl</FileName><Shader>WN_Hands_GlovesArmor01_Shader</Shader></Material><Material><FileName>Hat\\WN_Hat_Helmet01.mtl</FileName><Shader>WN_Hat_Helmet01_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eyes.mtl</FileName><Shader>WN_Head_Krawk_Eyes_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_HairBlue.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_SkinBlue.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Orb.mtl</FileName><Shader>WN_Prop_MagicWand_Orb_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Staff.mtl</FileName><Shader>WN_Prop_MagicWand_Staff_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shield_Metal.mtl</FileName><Shader>WN_Prop_Shield_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Handle.mtl</FileName><Shader>WN_Prop_Shovel_Handle_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Shovel.mtl</FileName><Shader>WN_Prop_Shovel_Shovel_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Leaf.mtl</FileName><Shader>WN_Prop_Spear_Leaf_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Metal.mtl</FileName><Shader>WN_Prop_Spear_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Stick.mtl</FileName><Shader>WN_Prop_Spear_Stick_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Twine.mtl</FileName><Shader>WN_Prop_Spear_Twine_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Wrap.mtl</FileName><Shader>WN_Prop_Spear_Wrap_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Blade.mtl</FileName><Shader>WN_Prop_Sword_Blade_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Gold.mtl</FileName><Shader>WN_Prop_Sword_Gold_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Handle.mtl</FileName><Shader>WN_Prop_Sword_Handle_Shader</Shader></Material><Material><FileName>Krawk\\WN_Tail_Krawk_Blue.mtl</FileName><Shader>WN_Body_Krawk_Tail_Shader</Shader></Material><Material><FileName>Top\\WN_Top_ShirtArmor01.mtl</FileName><Shader>WN_Top_ShirtArmor01_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>head</CameraName><StartWidth>250</StartWidth><StartHeight>250</StartHeight><Height>250</Height><Width>250</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
			 //					{workingString = "<Job><JobInfo><JobName>job-caf627ae-885c-49a4-990d-5479299c9911</JobName></JobInfo><Scene><FileName>c:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>head</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>WN_Eyrie_small</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_sm_MAT_Current.chx</FileName></BaseModel><Model><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Model><Model><Name>WN_Head_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Head_Eyrie_MAT_Current.chx</FileName></Model><Model><Name>WN_Mane_Eyrie_sm_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Mane_Eyrie_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Model><Model><Name>WN_Tail_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Tail_Eyrie_MAT_Current.chx</FileName></Model><Model><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Wings_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Wings_Eyrie_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><DeleteAllAnimation></DeleteAllAnimation><Rotates><Rotate>0</Rotate></Rotates><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>1</AngleCount><CameraName>head</CameraName></Animation></Animations><ShowCharacters><Character><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Character></ShowCharacters><HideCharacters><Character><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Character><Character><Name>WN_Head_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Head_Eyrie_MAT_Current.chx</FileName></Character><Character><Name>WN_Mane_Eyrie_sm_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Mane_Eyrie_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Character><Character><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Character><Character><Name>WN_Tail_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Tail_Eyrie_MAT_Current.chx</FileName></Character><Character><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Character><Character><Name>WN_Wings_Eyrie_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Eyrie\\Models\\WN_Wings_Eyrie_MAT_Current.chx</FileName></Character></HideCharacters><Materials><Material><FileName>BaseAvatar\\Body_Eyrie\\WN_Body_Eyrie_Green.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Bottom\\WN_Bottom_PantsArmor01.mtl</FileName><Shader>WN_Bottom_PantsArmor01_Shader</Shader></Material><Material><FileName>Feet\\WN_Feet_BootsArmor01.mtl</FileName><Shader>WN_Feet_BootsArmor01_Shader</Shader></Material><Material><FileName>Hands\\WN_Hands_GlovesArmor01.mtl</FileName><Shader>WN_Hands_GlovesArmor01_Shader</Shader></Material><Material><FileName>Hat\\WN_Hat_Helmet01.mtl</FileName><Shader>WN_Hat_Helmet01_Shader</Shader></Material><Material><FileName>Eyrie\\WN_Head_Eyrie_Eyes.mtl</FileName><Shader>WN_Head_Eyrie_Eyes_Shader</Shader></Material><Material><FileName>Eyrie\\WN_Head_Eyrie_Beak.mtl</FileName><Shader>WN_Head_Eyrie_Skin_Shader</Shader></Material><Material><FileName>Eyrie\\WN_Mane_Eyrie_Green.mtl</FileName><Shader>WN_Mane_Eyrie_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Orb.mtl</FileName><Shader>WN_Prop_MagicWand_Orb_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Staff.mtl</FileName><Shader>WN_Prop_MagicWand_Staff_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shield_Metal.mtl</FileName><Shader>WN_Prop_Shield_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Handle.mtl</FileName><Shader>WN_Prop_Shovel_Handle_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Shovel.mtl</FileName><Shader>WN_Prop_Shovel_Shovel_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Leaf.mtl</FileName><Shader>WN_Prop_Spear_Leaf_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Metal.mtl</FileName><Shader>WN_Prop_Spear_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Stick.mtl</FileName><Shader>WN_Prop_Spear_Stick_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Twine.mtl</FileName><Shader>WN_Prop_Spear_Twine_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Wrap.mtl</FileName><Shader>WN_Prop_Spear_Wrap_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Blade.mtl</FileName><Shader>WN_Prop_Sword_Blade_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Gold.mtl</FileName><Shader>WN_Prop_Sword_Gold_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Handle.mtl</FileName><Shader>WN_Prop_Sword_Handle_Shader</Shader></Material><Material><FileName>Eyrie\\WN_Tail_Eyrie_Green.mtl</FileName><Shader>WN_Tail_Eyrie_Shader</Shader></Material><Material><FileName>Top\\WN_Top_ShirtArmor01.mtl</FileName><Shader>WN_Top_ShirtArmor01_Shader</Shader></Material><Material><FileName>Eyrie\\WN_Wings_Eyrie_Green.mtl</FileName><Shader>WN_Wings_Eyrie_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>head</CameraName><StartWidth>250</StartWidth><StartHeight>250</StartHeight><Height>250</Height><Width>250</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
			{ workingString = "<Job><JobInfo><JobName>job-2e7d58a7-35c8-4d9a-b58f-6ad8e37a6946</JobName></JobInfo><Scene><FileName>c:\\projects\\wallstreet\\Shots\\Stock\\WS_MainScene_000.mab</FileName><Cameras><Camera><CameraName>isometric</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>WS_M01_Body_MAT_Current</Name><FileName>c:\\projects\\WALLSTREET\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Body_MAT_Current.chx</FileName></BaseModel><Model><Name>WS_M01_Bottom_PSlacks01_MAT_Current</Name><FileName>c:\\projects\\wallstreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Bottom_PSlacks01_MAT_Current.chx</FileName></Model><Model><Name>WS_M01_Hair01_MAT_Current</Name><FileName>c:\\projects\\wallstreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Hair01_MAT_Current.chx</FileName></Model><Model><Name>WS_M01_Shoe_Loafer01_MAT_Current</Name><FileName>c:\\projects\\wallstreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Shoe_Loafer01_MAT_Current.chx</FileName></Model><Model><Name>WS_M01_Top_RelaxShirtTie_MAT_Current</Name><FileName>c:\\projects\\wallstreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Top_RelaxShirtTie_MAT_Current.chx</FileName></Model><Model><Name>WS_M01_Top_Suspenders_MAT_Current</Name><FileName>c:\\projects\\wallstreet\\Data\\Stock\\Characters\\WS_M01\\Models\\WS_M01_Top_Suspenders_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><DeleteAllAnimation></DeleteAllAnimation><Animations><Animation><AnimationName>walk</AnimationName><FileName>WS_M01_ANM_walk.cha</FileName><AngleCount>8</AngleCount><CameraName>isometric</CameraName></Animation></Animations><Materials><Material><FileName>WS_M01\\WS_M01_Skins\\WS_M01_Body_Dark.mtl</FileName><Shader>WS_M01_Body_Shader</Shader></Material><Material><FileName>WS_M01\\WS_M01_Hairs\\WS_M01_Brow_Red.mtl</FileName><Shader>WS_M01_Brow_Shader</Shader></Material><Material><FileName>WS_M01\\WS_M01_Eyes\\WS_M01_Iris_Brown.mtl</FileName><Shader>WS_M01_Iris_Shader</Shader></Material><Material><FileName>WS_M01\\WS_M01_Hairs\\WS_M01_Hair01_Red.mtl</FileName><Shader>WS_M01_Hair01_Shader</Shader></Material><Material><FileName>WS_M01\\WS_M01_Shoes\\WS_M01_Loafer01\\WS_M01_Shoe_Loafer01_Brown.mtl</FileName><Shader>WS_M01_Shoe_Loafer01_Shader</Shader></Material><Material><FileName>WS_M01\\WS_M01_Tops\\WS_M01_Top_RelaxShirtSuspend\\WS_M01_Top_RlxShirt_Pink.mtl</FileName><Shader>WS_M01_Top_RlxShirt01_Shader1</Shader></Material><Material><FileName>WS_M01\\WS_M01_Tops\\WS_M01_Top_RelaxShirtSuspend\\WS_M01_Top_Suspender01_Brown.mtl</FileName><Shader>WS_M01_Top_Suspender01_Shader</Shader></Material></Materials></Modifications><RenderRequest><Sequences><Sequence><CameraName>isometric</CameraName><ImageFormat>PNG</ImageFormat><MaskFormat>NONE</MaskFormat><FrameRate>12.0</FrameRate><Height>200</Height><Width>165</Width></Sequence></Sequences></RenderRequest></Job>"; break;}
			//					{workingString = "<Job><JobInfo><JobName>job-5c0ced6a-e7b2-45e9-a45c-10c4f9921814</JobName></JobInfo><Scene><FileName>c:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>frontal</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>WN_Krawk_small</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_sm_MAT_Current.chx</FileName></BaseModel><Model><Name>WN_Bottom_PantsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Bottom\\Models\\WN_Bottom_PantsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Feet_BootsArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Feet\\Models\\WN_Feet_BootsArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hands_GlovesArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hands\\Models\\WN_Hands_GlovesArmor01_sm_MAT_Current.chx</FileName></Model><Model><Name>WN_Hat_Helmet01_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Hat\\Models\\WN_Hat_Helmet01_MAT_Current.chx</FileName></Model><Model><Name>WN_Head_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_MagicWand_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_MagicWand_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shield_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shield_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Shovel_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Shovel_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Spear_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Spear_MAT_Current.chx</FileName></Model><Model><Name>WN_Prop_Sword_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\General\\Models\\WN_Prop_Sword_MAT_Current.chx</FileName></Model><Model><Name>WN_Tail_Krawk_MAT_Current</Name><FileName>c:\\projects\\neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Tail_Krawk_MAT_Current.chx</FileName></Model><Model><Name>WN_Top_ShirtArmor01_sm_MAT_Current</Name><FileName>c:\\projects\\NEOPIA\\Data\\Stock\\Characters\\Top\\Models\\WN_Top_ShirtArmor01_sm_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><DeleteAllAnimation></DeleteAllAnimation><Rotates><Rotate>90</Rotate></Rotates><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>1</AngleCount><CameraName>frontal</CameraName></Animation></Animations><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Blue.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Bottom\\WN_Bottom_PantsArmor01.mtl</FileName><Shader>WN_Bottom_PantsArmor01_Shader</Shader></Material><Material><FileName>Feet\\WN_Feet_BootsArmor01.mtl</FileName><Shader>WN_Feet_BootsArmor01_Shader</Shader></Material><Material><FileName>Hands\\WN_Hands_GlovesArmor01.mtl</FileName><Shader>WN_Hands_GlovesArmor01_Shader</Shader></Material><Material><FileName>Hat\\WN_Hat_Helmet01.mtl</FileName><Shader>WN_Hat_Helmet01_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eyes.mtl</FileName><Shader>WN_Head_Krawk_Eyes_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_HairBlue.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_SkinBlue.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Orb.mtl</FileName><Shader>WN_Prop_MagicWand_Orb_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_MagicWand_Staff.mtl</FileName><Shader>WN_Prop_MagicWand_Staff_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shield_Metal.mtl</FileName><Shader>WN_Prop_Shield_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Handle.mtl</FileName><Shader>WN_Prop_Shovel_Handle_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Shovel_Shovel.mtl</FileName><Shader>WN_Prop_Shovel_Shovel_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Leaf.mtl</FileName><Shader>WN_Prop_Spear_Leaf_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Metal.mtl</FileName><Shader>WN_Prop_Spear_Metal_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Stick.mtl</FileName><Shader>WN_Prop_Spear_Stick_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Twine.mtl</FileName><Shader>WN_Prop_Spear_Twine_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Spear_Wrap.mtl</FileName><Shader>WN_Prop_Spear_Wrap_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Blade.mtl</FileName><Shader>WN_Prop_Sword_Blade_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Gold.mtl</FileName><Shader>WN_Prop_Sword_Gold_Shader</Shader></Material><Material><FileName>Props\\WN_Prop_Sword_Handle.mtl</FileName><Shader>WN_Prop_Sword_Handle_Shader</Shader></Material><Material><FileName>Krawk\\WN_Tail_Krawk_Blue.mtl</FileName><Shader>WN_Body_Krawk_Tail_Shader</Shader></Material><Material><FileName>Top\\WN_Top_ShirtArmor01.mtl</FileName><Shader>WN_Top_ShirtArmor01_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>frontal</CameraName><StartWidth>250</StartWidth><StartHeight>250</StartHeight><Height>250</Height><Width>250</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}

			//	T,R - load MTV then load Neopets test.
			case 't':{ workingString = "<Job><JobInfo><JobName>job-7fb261c7-d45d-4074-8c05-516fbb55eac4</JobName></JobInfo><Scene><FileName>C:\\projects\\Neopia\\Shots\\Stock\\WN_MainScene_000.mab</FileName><Cameras><Camera><CameraName>ISOMETRIC</CameraName></Camera></Cameras></Scene><Objects><Models><BaseModel><Name>body</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\BaseAvatar\\Models\\WN_Body_BaseAvatar_lg_MAT_Current.chx</FileName></BaseModel><Model><Name>head</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Head_Krawk_MAT_Current.chx</FileName></Model><Model><Name>tail</Name><FileName>C:\\projects\\Neopia\\Data\\Stock\\Characters\\Krawk\\Models\\WN_Body_Krawk_Tail_MAT_Current.chx</FileName></Model></Models></Objects><Modifications><Animations><Animation><AnimationName>idle_animation</AnimationName><FileName>WN_RIG_ANIM_IDLE_current.cha</FileName><AngleCount>8</AngleCount><CameraName>ISOMETRIC</CameraName></Animation></Animations><Materials><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Yellow.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Spec.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>BaseAvatar\\Body_Krawk\\WN_Body_Krawk_Norm.mtl</FileName><Shader>WN_Body_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_Eye.mtl</FileName><Shader>WN_Head_Krawk_Eye_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_HairBlue.mtl</FileName><Shader>WN_Head_Krawk_Hair_Shader</Shader></Material><Material><FileName>Krawk\\WN_Head_Krawk_SkinGreen.mtl</FileName><Shader>WN_Head_Krawk_Skin_Shader</Shader></Material></Materials></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>ISOMETRIC</CameraName><StartWidth>500</StartWidth><StartHeight>500</StartHeight><Height>500</Height><Width>500</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
			case 'r':{ workingString = "<Job><JobInfo><JobName>job-7a77416b-6e31-4b28-bfb0-3c3c39ad9289</JobName></JobInfo><Scene><FileName>C:\\projects\\VMTV\\Shots\\Stock\\vMTV_MainScene_000.mab</FileName></Scene><Objects><Models><BaseModel><Name>body</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Body_MAT_Current.chx</FileName></BaseModel><Model><Name>top</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Top_Hoodie01_MAT_Current.chx</FileName></Model><Model><Name>bottom</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Bottom_Jeans01_MAT_Current.chx</FileName></Model><Model><Name>hat</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Hat_Baseball01_MAT_Current.chx</FileName></Model><Model><Name>hair</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Hair01_MAT_Current.chx</FileName></Model><Model><Name>glasses</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Glasses_Rayban01_MAT_Current.chx</FileName></Model><Model><Name>shoes</Name><FileName>C:\\projects\\VMTV\\Data\\Stock\\Characters\\vMTV_M01\\Models\\VMTV_M01_Shoes_Tennis01_MAT_Current.chx</FileName></Model></Models></Objects><Modifications></Modifications><RenderRequest><Frames><Frame><ImageFormat>JPG</ImageFormat><MaskFormat>NONE</MaskFormat><CameraName>FRONTAL</CameraName><StartWidth>500</StartWidth><StartHeight>500</StartHeight><Height>500</Height><Width>500</Width><Time>0</Time></Frame></Frames></RenderRequest></Job>"; break;}
		}

		// make sure that we start with an empty name
		if( !workingString.empty() )
		{
			orthoRemoteCommandMgr::parse_input_xml(workingString);
		}
	}
}


//============================================================================
//============================================================================
namespace LightWaitMessaging 
{
	extern Thread *l_ConsumerThread;
	extern Thread *l_ProducerThread;

	extern LightWaitMessageConsumer *l_Consumer;
	extern LightWaitMessageProducer *l_Producer;

	extern CRITICAL_SECTION remoteMessagingCriticalSection; 
};
using namespace LightWaitMessaging;



//============================================================================
//============================================================================
namespace
{
	typedef enum 
	{
		nullRemoteCommand = 0,
		nameRemoteCommand,
		partRemoteCommand,
		colorRemoteCommand,
		renderRemoteCommand,
		forwardRemoteCommand,
		backwardRemoteCommand,
		rotateRightRemoteCommand,
		rotateLeftRemoteCommand,
		resetAllRemoteCommand,
		resetColorsRemoteCommand,
		resetCameraRemoteCommand,
		resetTimeRemoteCommand,
		hidePartsRemoteCommand,
		showPartsRemoteCommand,
		loadSceneRemoteCommand,
		setRenderSizeRemoteCommand,
		setRenderTimeRemoteCommand,
		addPartRemoteCommand,
		removePartRemoteCommand,
		changeTextureRemoteCommand,
		pingRemoteCommand,
		responseFormatRemoteCommand,
		imageFormatCommand,
		maskFormatCommand,
		setRenderFPSRemoteCommand,
		addAnimationRemoteCommand,
		deleteAnimationRemoteCommand,
		loadCharacterRemoteCommand,
		changeMaterialRemoteCommand,
		renderSequenceRemoteCommand,
		saveXMLSceneRemoteCommand,
		deleteCharacterRemoteCommand,
		replaceCharacterRemoteCommand,
		hideCharacterRemoteCommand,
		showCharacterRemoteCommand,
		scaleCharacterRemoteCommand,
	} RemoteCommand;

	RemoteCommand remoteCommand = nullRemoteCommand;
	static int remotePartIndex = 0;
	static unsigned short remoteColorRedIndex = 0;
	static unsigned short remoteColorGreenIndex = 0;
	static unsigned short remoteColorBlueIndex = 0;
	static float remoteTimePoint = 0.0f;
	static int remoteRenderWidth = 512;
	static int remoteRenderHeight = 512;
	static int remoteRenderStartWidth = 512;
	static int remoteRenderStartHeight = 512;
	static float remoteRenderFPS = 24.0;

	int pingNumber = 0;
}


//============================================================================
//============================================================================
namespace orthoRemoteCommandMgr
{
	std::vector<RemoteRenderData>		m_RenderParameters;
	std::vector<RemoteAnimFrameData>	m_AnimFrameData;

	//----------------------------------------------------------------------------
	// orthoRemoteCommandMgr static constants
	//----------------------------------------------------------------------------
	std::string m_RenderModelName;
	std::string m_RenderCurrentModelName;
	std::string m_RenderAnimName;
	std::string m_RenderCharacterFileName;
	std::string m_RenderPartName;
	std::string m_RenderPartFileName;
	std::string m_RenderMaterial;
	std::string m_RenderTextureFileName;

	std::vector<std::string> m_CaptureCameras;

	int m_nDivisionCount;				//divide the animated rotation in this many parts

	const float lc_fRotateIncrement = maConstants::c_fPI / 200.0f;
	const float lc_fFocusRadius	= 10.0f;

	const maVector3d lc_RotateX( 1, 0, 0 );
	const maVector3d lc_RotateY( 0, 1, 0 );
	const maVector3d lc_RotateZ( 0, 0, 1 );

	std::string	l_CameraToRender("All");

	int l_MaterialPartIndex = 0;

	mnmObject* l_pSelectedObject = 0;
	mnmObject* l_pSelectedObjectLast = 0;

	bool l_bCompassManip = false;
	int	 l_nCompassType = 0;
	int  l_nCompassPart = 0;
	float l_fCompassScalePercent = 1.0f;
	float l_cfCOMPASSSCALEPERCENT_MAX = 3.0f;
	float l_cfCOMPASSSCALEPERCENT_INC = 0.05f;

	//local command file
	std::ifstream* l_pCommandFile = NULL;

	//zlib stream
	z_stream	l_strm;

	//thread
	HANDLE l_hCommandThread = NULL;
	DWORD l_CommandThreadID = 0;
	volatile bool l_bCommandThreadActive = false;

	LocalCharEventRequest* l_pCommandCharEvent = NULL;
}

orthoRemoteCommandMgr::RemoteRenderData::RemoteRenderData()
{
	renderCamera = "ISOMETRIC";
	renderName = "";
	responseFormat = "PLAIN";
	imageFormat = "JPG";
	maskFormat = "PNG";
	renderWidth = 0;
	renderHeight = 0;
	renderStartWidth = 0;
	renderStartHeight = 0;
	renderTimePoint = 0.0f;
	renderFPS = 24.0f;
	m_CurrentRenderResponse = NULL;
}

orthoRemoteCommandMgr::RemoteRenderData::~RemoteRenderData()
{
	delete m_CurrentRenderResponse;
}

void orthoRemoteCommandMgr::RemoteRenderData::SetRenderResponse( LightWaitResponse* response )
{
	if( m_CurrentRenderResponse ) delete m_CurrentRenderResponse;
	m_CurrentRenderResponse = response;
}

void orthoRemoteCommandMgr::RemoteRenderData::CreateRenderResponse( const std::string& modelName )
{
	if( m_CurrentRenderResponse ) delete m_CurrentRenderResponse;
	m_CurrentRenderResponse = new LightWaitResponse((char *)modelName.c_str(), (char *)renderName.c_str());
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoRemoteCommandMgr::Initialize()
{
	//	Initialize the remote render data
	//
	m_RenderParameters.resize(1);

	l_pCommandCharEvent = new LocalCharEventRequest;

	l_pCommandCharEvent->SetEnableCharEvents( true );

	l_hCommandThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)CheckInput, NULL, 0, &l_CommandThreadID );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoRemoteCommandMgr::DeInitialize()
{
	l_pCommandCharEvent->SetEnableCharEvents( false );
	delete l_pCommandCharEvent;

	l_bCommandThreadActive = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void set_command_xml(std::string& i_Command, std::string& i_Value)
{
	DBG_LOG2("(%s) value = %s", i_Command.c_str(), i_Value.c_str());

	//
	//	THE BIG IF STATEMENT (like in parse_input) GOES HERE
	//

	i_Value.clear();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void orthoRemoteCommandMgr::parse_input_xml(std::string& workingString)
{
	//	this IF is for testing only
	//
	//	display the workingString each time one is set and make it warning so it shows up in release also.
	//
	if (!workingString.empty()) 
	{
		DBG_WARNING1("COMMAND: %s", workingString.c_str());
	}

	if( !orthoRequestTaskUtil::ParseXMLCommandString( workingString ))
	{
		DBG_WARNING0("Parsing XML command had errors!");
	}
}

bool orthoRemoteCommandMgr::CompressGZIPString( std::string& workingString )
{
	int count = workingString.size();
	if( count > 0 )
	{
		l_strm.zalloc = Z_NULL;
		l_strm.zfree = Z_NULL;
		l_strm.opaque = Z_NULL;
		l_strm.data_type = Z_ASCII;
		int ret = deflateInit( &l_strm, Z_DEFAULT_COMPRESSION );
		if( ret == Z_OK )
		{
			char* compressed = new char[ count ];
			int count_Comp = 0;

			l_strm.avail_in = count;
			l_strm.next_in = (unsigned char*)const_cast<char*>(workingString.data());
			do
			{
				l_strm.avail_out = count;
				l_strm.next_out = (Bytef*)compressed;
				int ret = deflate( &orthoRemoteCommandMgr::l_strm, Z_FINISH );
				count_Comp += count - l_strm.avail_out;
			}while( l_strm.avail_out == 0 );
			deflateEnd( &l_strm );

			workingString.clear();
			workingString.append( compressed, count_Comp );

			delete [] compressed;
			return true;
		}
		return false;
	}
	return true;
}

bool orthoRemoteCommandMgr::DecompressGZIPString( std::string& workingString )
{
	int count = workingString.size();

	if( count > 0 )
	{
		l_strm.zalloc = Z_NULL;
		l_strm.zfree = Z_NULL;
		l_strm.opaque = Z_NULL;
		l_strm.avail_in = 0;
		l_strm.next_in = Z_NULL;
		l_strm.data_type = Z_ASCII;
		int ret = inflateInit( &l_strm );
		if( ret == Z_OK )
		{
			char* decompressed = new char[ count*2 ];
			int count_Decomp = 0;

			l_strm.avail_in = count;
			l_strm.next_in = (unsigned char*)const_cast<char*>(workingString.data());
			do 
			{
				l_strm.avail_out = count;
				l_strm.next_out = (Bytef*)decompressed;
				int ret = inflate( &l_strm, Z_NO_FLUSH );
				count_Decomp += count - l_strm.avail_out;
			}while( l_strm.avail_out == 0 );
			inflateEnd( &l_strm );

			workingString.clear();
			workingString.append( decompressed, count_Decomp );

			delete [] decompressed;
			return true;
		}
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
//	Check the input
//----------------------------------------------------------------------------
DWORD CheckInput( LPVOID param )
{
	orthoRemoteCommandMgr::l_bCommandThreadActive = true;

	string workingString;
	bool changedScene = false;
	static bool bTestCommands = false;
	static bool bTestAutomate = false;

	do 
	{
		if( !g_bRemoteRenderingCommand )
		{
			if( orthoRemoteCommandMgr::l_pCommandFile )
			{
				std::stringstream filestream;
				filestream << orthoRemoteCommandMgr::l_pCommandFile->rdbuf();
				workingString = filestream.str();
				orthoRemoteCommandMgr::l_pCommandFile->close();
				delete orthoRemoteCommandMgr::l_pCommandFile;
				orthoRemoteCommandMgr::l_pCommandFile = NULL;
			}
		}
		else
		{
			EnterCriticalSection(&remoteMessagingCriticalSection);
				
			queue<LightWaitMessage *> *messageQ = l_Consumer->getQueue();
			if (messageQ == 0) 
			{
				LeaveCriticalSection(&remoteMessagingCriticalSection);

				return false;
			}

			if (messageQ->empty()) 
			{
				LeaveCriticalSection(&remoteMessagingCriticalSection);

				return false;
			}

#if _DEBUG
			int oldCRTDEBUGFLAG = _crtDbgFlag;
			_crtDbgFlag = 5;
#endif

			LightWaitMessage* message = messageQ->front(); // messageQ->pop();
			messageQ->pop();

			LeaveCriticalSection(&remoteMessagingCriticalSection);

			workingString = (message->text.c_str());

			//check for compressed data
			static char compressFomat[] = "GZIP";

			int loc = workingString.find( compressFomat, 0);
			if( loc == 0 )
			{
				workingString.erase( loc, sizeof( compressFomat ) );	//remove the header

				//decompress working string
				orthoRemoteCommandMgr::DecompressGZIPString( workingString );
			}
		}

		if( !workingString.empty() )
		{
			orthoRemoteCommandMgr::parse_input_xml(workingString);
		}

	}while( orthoRemoteCommandMgr::l_bCommandThreadActive );

	return 0;
}
