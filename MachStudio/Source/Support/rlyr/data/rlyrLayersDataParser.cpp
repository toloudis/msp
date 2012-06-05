/********************************************************************************************\
**  rlyrLayersDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#include "Support/rlyr/data/rlyrLayersDataParser.hpp"

#include "Support/capt/captOutputDataParser.hpp"
#include "Support/pfx/pfxDataParser.hpp"
#include "Support/rprf/rprfPrefsDataParser.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

//============================================================================
//============================================================================
namespace rlyrLayersDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_RLYS = chDefs::MakeName('R', 'L', 'Y', 'S');	//   +-- render layer list data
const chDefs::Name c_RLYR = chDefs::MakeName('R', 'L', 'Y', 'R');	// main data
const chDefs::Name c_LYRD = chDefs::MakeName('L', 'Y', 'R', 'D');	// +-- layer data
const chDefs::Name c_CAPD = chDefs::MakeName('C', 'A', 'P', 'D');	//   +-- capture data
const chDefs::Name c_OBJS = chDefs::MakeName('O', 'B', 'J', 'S');	// +-- object list
const chDefs::Name c_OBJD = chDefs::MakeName('O', 'B', 'J', 'D');	// +-- object data
const chDefs::Name c_NDES = chDefs::MakeName('N', 'D', 'E', 'S');	// +-- node list
const chDefs::Name c_NDED = chDefs::MakeName('N', 'D', 'E', 'D');	// +-- node data
const chDefs::Name c_PRFD = chDefs::MakeName('P', 'R', 'F', 'D');	//   +-- render prefs data
const chDefs::Name c_PASD = chDefs::MakeName('P', 'A', 'S', 'D');	//   +-- render passes data
const chDefs::Name c_PFXD = chDefs::MakeName('P', 'F', 'X', 'D');	//   +-- pfx data


////------------------------------------------------------------------------
////   ReadLayerData
////------------------------------------------------------------------------
void ReadLayerData(chReader& io_Reader,
						chDefs::Version i_Version,
						rlyrLayerDataItem& o_LayerData )
{
	o_LayerData.m_Name.Read(io_Reader);
	//o_LayerData.m_ParentName.Read(io_Reader);
	o_LayerData.m_IsActive.Read(io_Reader);
}

////------------------------------------------------------------------------
////   WriteLayerData
////------------------------------------------------------------------------
void WriteLayerData(chWriter& o_Writer,
					const rlyrLayerDataItem& i_LayerData )
{
	const int l_cLYRD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_LYRD, l_cLYRD_VERSION, true );

	i_LayerData.m_Name.Write(o_Writer);
	//i_LayerData.m_ParentName.Write(o_Writer);
	i_LayerData.m_IsActive.Write(o_Writer);

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   WriteCaptureData
////------------------------------------------------------------------------
void WriteCaptureData(chWriter& o_Writer,
					const captRenderOutputData& i_CaptureData )
{
	captOutputDataParser::WriteCaptureData(o_Writer, i_CaptureData);
}

////------------------------------------------------------------------------
////   WritePrefsData
////------------------------------------------------------------------------
void WritePrefsData(chWriter& o_Writer,
					const rprfPrefsData& i_PrefsData )
{
	rprfPrefsDataParser::WritePrefsData( o_Writer, i_PrefsData );
}

//------------------------------------------------------------------------
//   WritePfxData
//------------------------------------------------------------------------
void WritePfxData(chWriter& o_Writer,
					const pfxData& i_PfxData )
{
	pfxDataParser::WritePfxData( o_Writer, i_PfxData );
}

////------------------------------------------------------------------------
////   WritePassesData
////------------------------------------------------------------------------
void WritePassesData(chWriter& o_Writer,
					const rlyrPassesData& i_PassesData)
{
	const int l_cPASD_VERSION = 7;
	o_Writer.WriteChunkHeader( c_PASD, l_cPASD_VERSION, false );

	i_PassesData.m_Beauty.Write(o_Writer);

	// v2 write the rest of the darn passes
	i_PassesData.m_AOOnly.Write(o_Writer);
	i_PassesData.m_Depth.Write(o_Writer);
	i_PassesData.m_ShadowMask.Write(o_Writer);
	i_PassesData.m_IlluminationOnly.Write(o_Writer);
	i_PassesData.m_Normals.Write(o_Writer);
	i_PassesData.m_DirtyMatte.Write(o_Writer);
	i_PassesData.m_Wireframe.Write(o_Writer);
	i_PassesData.m_Materials.Write(o_Writer);
	i_PassesData.m_ReflectionsOnly.Write(o_Writer);
	i_PassesData.m_Velocity.Write(o_Writer);
	i_PassesData.m_Diffuse.Write(o_Writer);
	i_PassesData.m_Specular.Write(o_Writer);

	// v3 new passes
	i_PassesData.m_Bloom.Write(o_Writer);
	i_PassesData.m_Star.Write(o_Writer);
	i_PassesData.m_CameraDOF.Write(o_Writer);
	i_PassesData.m_Preview.Write(o_Writer);

	// v4 new passes
	i_PassesData.m_Emissive.Write(o_Writer);
	i_PassesData.m_SpecEnv.Write(o_Writer);
	i_PassesData.m_SpecLit.Write(o_Writer);
	i_PassesData.m_DiffEnv.Write(o_Writer);
	i_PassesData.m_DiffLit.Write(o_Writer);

	// v5 new pass
	i_PassesData.m_GI.Write(o_Writer);

	// v6 new pass
	i_PassesData.m_Glow.Write(o_Writer);

	// v7 new passes
	i_PassesData.m_RmanColorBleed.Write(o_Writer);
	i_PassesData.m_MrayFinalGather.Write(o_Writer);

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   ReadPassesData
////------------------------------------------------------------------------
void ReadPassesData(chReader& io_Reader,
					chDefs::Version i_Version,
					rlyrPassesData& i_PassesData)
{
	i_PassesData.m_Beauty.Read(io_Reader);
	if (i_Version >= 2)
	{
		i_PassesData.m_AOOnly.Read(io_Reader);
		i_PassesData.m_Depth.Read(io_Reader);
		i_PassesData.m_ShadowMask.Read(io_Reader);
		i_PassesData.m_IlluminationOnly.Read(io_Reader);
		i_PassesData.m_Normals.Read(io_Reader);
		i_PassesData.m_DirtyMatte.Read(io_Reader);
		i_PassesData.m_Wireframe.Read(io_Reader);
		i_PassesData.m_Materials.Read(io_Reader);
		i_PassesData.m_ReflectionsOnly.Read(io_Reader);
		i_PassesData.m_Velocity.Read(io_Reader);
		i_PassesData.m_Diffuse.Read(io_Reader);
		i_PassesData.m_Specular.Read(io_Reader);

		if (i_Version >= 3)
		{
			i_PassesData.m_Bloom.Read(io_Reader);
			i_PassesData.m_Star.Read(io_Reader);
			i_PassesData.m_CameraDOF.Read(io_Reader);
			i_PassesData.m_Preview.Read(io_Reader);

			if (i_Version >= 4)
			{
				i_PassesData.m_Emissive.Read(io_Reader);
				i_PassesData.m_SpecEnv.Read(io_Reader);
				i_PassesData.m_SpecLit.Read(io_Reader);
				i_PassesData.m_DiffEnv.Read(io_Reader);
				i_PassesData.m_DiffLit.Read(io_Reader);
				
				if (i_Version >= 5)
				{
					i_PassesData.m_GI.Read(io_Reader);

					if (i_Version >= 6)
					{
						i_PassesData.m_Glow.Read(io_Reader);

						if (i_Version >= 7)
						{
							i_PassesData.m_RmanColorBleed.Read(io_Reader);
							i_PassesData.m_MrayFinalGather.Read(io_Reader);
						}
					}

				}

			}


		}

	}
}

////------------------------------------------------------------------------
////   ReadNodeData
////------------------------------------------------------------------------
void ReadNodeData(chReader& io_Reader,
					chDefs::Version i_Version,
					rlyrNodeDataItem& o_NodeData )
{
	o_NodeData.m_Index.Read( io_Reader );
	o_NodeData.m_IsVisible.Read( io_Reader );
}

////------------------------------------------------------------------------
////   ReadNodeList
////------------------------------------------------------------------------
void ReadNodeList(chReader& io_Reader,
					chDefs::Version i_Version,
					std::vector<rlyrNodeDataItem>& o_Nodes )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	
	int index = 0;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_NDED )
		{
			if (index >= o_Nodes.size())
				o_Nodes.resize( index+1 );

			ReadNodeData(io_Reader, version, o_Nodes[index++]);
			io_Reader.FinishChunk();
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
	}
}

////------------------------------------------------------------------------
////   ReadObjectData
////------------------------------------------------------------------------
void ReadObjectData(chReader& io_Reader,
					chDefs::Version i_Version,
					rlyrObjectDataItem& o_ObjectData )
{
	o_ObjectData.m_Name.Read( io_Reader );
	o_ObjectData.m_IsVisible.Read( io_Reader );

	if( i_Version > 1 )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( io_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_NDES )
			{
				ReadNodeList(io_Reader, i_Version, o_ObjectData.m_Nodes);
				io_Reader.FinishChunk();
			}
			else
			{
				//DBG_ASSERT(false, "invalid chunk header");
				DBG_TRACE("invalid chunk header" << name);
			}
		}
	}
}

////------------------------------------------------------------------------
////   ReadObjectList
////------------------------------------------------------------------------
void ReadObjectList(chReader& io_Reader,
					chDefs::Version i_Version,
					std::vector<rlyrObjectDataItem>& o_Objects )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	
	int index = 0;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_OBJD )
		{
			if (index >= o_Objects.size())
				o_Objects.resize( index+1 );

			ReadObjectData(io_Reader, version, o_Objects[index++]);
			io_Reader.FinishChunk();
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
	}
}


////------------------------------------------------------------------------
////   WriteNodeData
////------------------------------------------------------------------------
void WriteNodeData(chWriter& o_Writer,
					 rlyrNodeDataItem& i_NodeData)
{
	const int l_cNDED_VERSION = 1;
	o_Writer.WriteChunkHeader( c_NDED, l_cNDED_VERSION, true );

	i_NodeData.m_Index.Write( o_Writer );
	i_NodeData.m_IsVisible.Write( o_Writer );

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   WriteNodeList
////------------------------------------------------------------------------
void WriteNodeListData(chWriter& o_Writer,
					   std::vector<rlyrNodeDataItem>& i_Nodes)
{
	const int l_cNDES_VERSION = 1;
	o_Writer.WriteChunkHeader( c_NDES, l_cNDES_VERSION, true );

	std::vector<rlyrNodeDataItem>::iterator it, end = i_Nodes.end();
	for (it = i_Nodes.begin(); it != end; ++it)
	{
		WriteNodeData(o_Writer, (*it));
	}

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   WriteObjectData
////------------------------------------------------------------------------
void WriteObjectData(chWriter& o_Writer,
					 rlyrObjectDataItem& i_ObjectData)
{
	const int l_cOBJD_VERSION = 2;
	o_Writer.WriteChunkHeader( c_OBJD, l_cOBJD_VERSION, true );

	i_ObjectData.m_Name.Write( o_Writer );
	i_ObjectData.m_IsVisible.Write( o_Writer );

	// version 2
	WriteNodeListData( o_Writer, i_ObjectData.m_Nodes );

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   WriteObjectList
////------------------------------------------------------------------------
void WriteObjectListData(chWriter& o_Writer,
						 std::vector<rlyrObjectDataItem> i_Objects)
{
	const int l_cOBJS_VERSION = 1;
	o_Writer.WriteChunkHeader( c_OBJS, l_cOBJS_VERSION, true );

	std::vector<rlyrObjectDataItem>::iterator it, end = i_Objects.end();
	for (it = i_Objects.begin(); it != end; ++it)
	{
		WriteObjectData(o_Writer, (*it));
	}

	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   TranslateRendererToPassFlag
////	This is an i/o remapping to assign the pass flag corresponding to 
////	an old render layer's renderer type from its render prefs.
////------------------------------------------------------------------------
void TranslateRendererToPassFlag(rlyrLayerDataItem& i_Layer)
{
	g3dSceneRendererTypes::RendererType rtype;
	rtype = rprfPrefsObject::GetRendererType(i_Layer.m_RenderPrefs.m_RendererType.GetValue());

	// assume that beauty pass is on by default
	// and legacy code was only enabling a single renderer type.

	switch(rtype)
	{
	case g3dSceneRendererTypes::e_Default:
	case g3dSceneRendererTypes::e_HDR:
		i_Layer.m_RenderPasses.m_Beauty.SetValue(true);
		break;

	case g3dSceneRendererTypes::e_AmbientOcclusion:
		i_Layer.m_RenderPasses.m_AOOnly.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_Depth:
		i_Layer.m_RenderPasses.m_Depth.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_ShadowMask:
		i_Layer.m_RenderPasses.m_ShadowMask.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_IlluminationOnly:
		i_Layer.m_RenderPasses.m_IlluminationOnly.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_Normals:
		i_Layer.m_RenderPasses.m_Normals.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_DirtyMatte:
		i_Layer.m_RenderPasses.m_DirtyMatte.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_Wireframe:
		i_Layer.m_RenderPasses.m_Wireframe.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_Materials:
		i_Layer.m_RenderPasses.m_Materials.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_ReflectionOnly:
		i_Layer.m_RenderPasses.m_ReflectionsOnly.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	case g3dSceneRendererTypes::e_VelocityMap:
		i_Layer.m_RenderPasses.m_Velocity.SetValue(true);
		i_Layer.m_RenderPasses.m_Beauty.SetValue(false);
		break;
	default:
		DBG_LOG("unknown mapping of renderer " << (int)rtype << " to render pass type");
		break;
	};
}


}	// local namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_RLYS;
}

////------------------------------------------------------------------------
////   Write each piece of data for the current layer
////------------------------------------------------------------------------
void WriteAllLayerData(chWriter& o_Writer,
				const rlyrLayerDataItem& i_Data )
{
	// incremented to version 2 for addition of render passes chunk and a remapping step.
	const int l_cRLYR_VERSION = 2;
	o_Writer.WriteChunkHeader( c_RLYR, l_cRLYR_VERSION, true );

	WriteLayerData(o_Writer, i_Data);
	WriteCaptureData(o_Writer, i_Data.m_OutputFormat);
	WritePrefsData(o_Writer, i_Data.m_RenderPrefs);
	WriteObjectListData(o_Writer, i_Data.m_Objects);
	WritePassesData(o_Writer, i_Data.m_RenderPasses);
	WritePfxData(o_Writer, i_Data.m_PostEffect);
	
	o_Writer.FinishChunk();
}

////------------------------------------------------------------------------
////   Read each piece of data for the current layer
////------------------------------------------------------------------------
void ReadAllLayerData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				rlyrLayerDataItem& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{

		if ( name == c_LYRD )
		{
			ReadLayerData(i_Reader, version, o_Data);
		}
		else if ( name == c_CAPD )
		{
			captOutputDataParser::ReadCaptureData(i_Reader, version, size, o_Data.m_OutputFormat);
		}
		else if ( name == c_PRFD )
		{
			rprfPrefsDataParser::ReadPrefsData( i_Reader, version, o_Data.m_RenderPrefs );
		}
		else if ( name == c_PASD )
		{
			ReadPassesData( i_Reader, version, o_Data.m_RenderPasses );
		}
		else if ( name == c_OBJS )
		{
			ReadObjectList( i_Reader, version, o_Data.m_Objects );
		}
		else if ( name == c_PFXD )
		{
			pfxDataParser::ReadPfxData( i_Reader, version, o_Data.m_PostEffect);
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				rlyrLayersData& o_LayerData )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_RLYR )
		{
			rlyrLayerDataItem cur_layer;
			ReadAllLayerData(i_Reader, i_Version, size, cur_layer);

			// remapping step for render passes
			if (version < 2)
			{
				TranslateRendererToPassFlag(cur_layer);
			}
			
			o_LayerData.m_Layers.push_back(cur_layer);

			i_Reader.FinishChunk();
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const rlyrLayersData& i_LayerData )
{
	const int l_cRLYS_VERSION = 1;
	o_Writer.WriteChunkHeader( c_RLYS, l_cRLYS_VERSION, true );

	for(int i = 0; i < i_LayerData.m_Layers.size(); ++i)
		WriteAllLayerData(o_Writer, i_LayerData.m_Layers[i]);

	o_Writer.FinishChunk();
}

}	// end of namespace

