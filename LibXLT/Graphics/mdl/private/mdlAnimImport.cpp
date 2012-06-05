/****************************************************************************\
**  mdlAnimImport.cpp
**
**      mdlAnimImport.hpp supplies functions used to import 
**	animation files.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlAnimImport.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlRotationOrder.hpp"
#include "Graphics/vtx/vtxVertexAnimImport.hpp"


//============================================================================
// This compiler define is just here so we could switch off the
// the vertex animation swapping quickly if needed.
//============================================================================
#define USE_DYNAMIC_VERTEX_ANIM


//============================================================================
//============================================================================
namespace mdlAnimImport
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
const bool l_bDebugAnimNodes = false;

//------------------------------------------------------------------------
// Chunk names
//------------------------------------------------------------------------
const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_DELT = chDefs::MakeName('D', 'E', 'L', 'T');

const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
const chDefs::Name c_ANIH = chDefs::MakeName('A', 'N', 'I', 'H');
const chDefs::Name c_ANLV = chDefs::MakeName('A', 'N', 'L', 'V');
const chDefs::Name c_APKY = chDefs::MakeName('A', 'P', 'K', 'Y');
const chDefs::Name c_ARKY = chDefs::MakeName('A', 'R', 'K', 'Y');
const chDefs::Name c_RORD = chDefs::MakeName('R', 'O', 'R', 'D');
const chDefs::Name c_ASKY = chDefs::MakeName('A', 'S', 'K', 'Y');
const chDefs::Name c_ABLS = chDefs::MakeName('A', 'B', 'L', 'S');
const chDefs::Name c_AMKY = chDefs::MakeName('A', 'M', 'K', 'Y');
const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
const chDefs::Name c_AVIS = chDefs::MakeName('A', 'V', 'I', 'S');
const chDefs::Name c_ASKN = chDefs::MakeName('A', 'S', 'K', 'N');
const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');

const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
const chDefs::Name c_GCHE = chDefs::MakeName('G', 'C', 'H', 'E');
const chDefs::Name c_VTXC = chDefs::MakeName('V', 'T', 'X', 'C');


//------------------------------------------------------------------------
// Read skin animation (visibility)
//------------------------------------------------------------------------
void read_ASKN(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::map<std::string, smdlGeoAnimKeys>& o_SkinKeys,
				std::vector<anKeyDataBase<bool>*>& o_VisibleChannels)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	envType::UInt16 num_keys;
	std::string skin_name;

	try
	{
		while ( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_NNAM )
			{
				i_Reader.Read(skin_name);
				//DBG_LOG("Read anim skin named " << skin_name.c_str());
			}
			else if ( name == c_AVIS )
			{
				i_Reader.Read(num_keys);
				//DBG_LOG("Reading visibility keys: " << num_keys);
				if ( num_keys )
				{
					float time;
					bool val;

					anKeyDataBase<bool>* new_anim = new anKeyDataBase<bool>(true);
					for (int i = 0 ; i < num_keys ; ++i )
					{
						i_Reader.Read(val);
						i_Reader.Read(time);
						//DBG_LOG2("Reading AVIS: %f, %s", time, val ? "True" : "False");
						new_anim->AddKey(time, val);
					}

					o_SkinKeys[skin_name].SetVisible(new_anim);
					o_VisibleChannels.push_back(new_anim);
					//new_anim->SetLooping(true);
				}
			}

			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::read_ASKN");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}
}

//------------------------------------------------------------------------
// Read a level of a hierarchical animation
//------------------------------------------------------------------------
void read_ANLV(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::string &o_RootName,
				smdlTree<smdlGeoAnimKeys>::iterator& io_It,
				std::vector<anKeyData<maPoint3d>*>& o_TranslationChannels,
				std::vector<anKeyData<maRotation>*>& o_RotationChannels,
				std::vector<anKeyData<maVector3d>*>& o_ScaleChannels,
				std::vector<anKeyDataBase<bool>*>& o_VisibleChannels,
				int i_Depth)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	envType::UInt16 num_keys;
	envType::UInt16 i;
	int children_added = 0;

	if (l_bDebugAnimNodes)
	{
		std::string tabs;
		for (i=0; i<i_Depth; i++)
			tabs += " ";
		DBG_LOG("Read anim node: %s" << tabs.c_str() << "anim");
	}

	mdlRotationOrder::RotationOrder rotation_order = mdlRotationOrder::e_XYZ;

	try
	{
		while ( i_Reader.ReadChunkHeader(name, version, size) )
		{
			//DBG_LOG("read_ANLV, Read chunk name " << (char*)(&name));

			if ( name == c_NNAM )
			{
				i_Reader.Read(o_RootName);
				//DBG_LOG("Read anim node named, " << o_RootName.c_str());
			}
			else if ( name == c_APKY )
			{
				i_Reader.Read(num_keys);
//				DBG_LOG("Reading position keys: " << num_keys);
				if ( num_keys )
				{
					float time;
					maPoint3d val;

					anKeyData<maPoint3d>* new_anim = new anKeyData<maPoint3d>();
					for ( i = 0 ; i < num_keys ; ++i )
					{
						chChunkParserUtil::Read(i_Reader, val);
						i_Reader.Read(time);
//						DBG_LOG("Reading APKY: " << time);
						new_anim->AddKey(time, val);
					}

					io_It.GetData().SetTranslate(new_anim);
					o_TranslationChannels.push_back(new_anim);
					//new_anim->SetLooping(true);
				}
			}
			else if ( name == c_ASKY )
			{
				i_Reader.Read(num_keys);
				//DBG_LOG("Reading scale keys: " << num_keys);
				if ( num_keys )
				{
					float time;
					maPoint3d val;

					anKeyData<maVector3d>* new_anim = new anKeyData<maVector3d>();
					for ( i = 0 ; i < num_keys ; ++i )
					{
						chChunkParserUtil::Read(i_Reader, val);
						i_Reader.Read(time);
						//DBG_LOG("Reading ASKY: " << time);
						new_anim->AddKey(time, val);
					}

					io_It.GetData().SetScale(new_anim);
					o_ScaleChannels.push_back(new_anim);
					//new_anim->SetLooping(true);
				}
			}
			else if (name == c_RORD)
			{
				envType::UInt8 order;
				i_Reader.Read(order);
				rotation_order = (mdlRotationOrder::RotationOrder)(order); // enumeration set up to match file format
			}
			else if ( name == c_ARKY )
			{
				i_Reader.Read(num_keys);
				//DBG_LOG("Reading rotation keys: " << num_keys);
				if ( num_keys )
				{
					float time, last_time;
					maPoint3d val, last_val;
					maRotation rot;
					chChunkParserUtil::Read(i_Reader, val);
					i_Reader.Read(time);

					mdlRotationOrder::SetOrderedEuler(rot, rotation_order, val.m_X, val.m_Y, val.m_Z);
					//rot.SetEuler(val.m_X, val.m_Y, val.m_Z);
					last_val = val;
					last_time = time;
					anKeyData<maRotation>* new_anim = new anKeyData<maRotation>();
					new_anim->AddKey(time, rot);

					for ( i = 1 ; i < num_keys ; ++i )
					{
						chChunkParserUtil::Read(i_Reader, val);
						i_Reader.Read(time);

						const float c_HalfTurn = 180 * maConstants::c_fAngleToRad;
						float max_turn = maFunctions::Highest(
							(maFunctions::Highest( fabsf(val.m_X - last_val.m_X),
								fabsf(val.m_Y - last_val.m_Y))),
							fabsf(val.m_Z - last_val.m_Z) );

						// This was breaking the motion blur code. 
						// It should be fairly safe if we always have one 
						// key frame per frame as in the "baked" animations.

						//if (max_turn >= c_HalfTurn)
						//{
						//	// If more than 180 degree turn,
						//	// add in new keys to make sure
						//	// quaternions turn in correct direction
						//	//
						//	int num_new_keys = int (max_turn / c_HalfTurn);
						//	float delta = 1.0f / float(num_new_keys+1);

						//	for (int i=0; i<num_new_keys; i++)
						//	{
						//		float alpha = delta * (i+1);
						//		maVector3d int_val = val * alpha + last_val * (1 - alpha);
						//		float int_time = time * alpha + last_time * (1- alpha);
						//		rot.SetEuler(int_val.m_X, int_val.m_Y, int_val.m_Z);
						//		new_anim->AddKey(int_time, rot);
						//	}
						//}

						mdlRotationOrder::SetOrderedEuler(rot, rotation_order, val.m_X, val.m_Y, val.m_Z);
						//rot.SetEuler(val.m_X, val.m_Y, val.m_Z);
						last_val = val;
						last_time = time;
						new_anim->AddKey(time, rot);
					}

					io_It.GetData().SetRotate(new_anim);
					o_RotationChannels.push_back(new_anim);
					//new_anim->SetLooping(true);
				}
			}
			else if ( name == c_AVIS )
			{
				i_Reader.Read(num_keys);
				//DBG_LOG("Reading visibility keys: " << num_keys);
				if ( num_keys )
				{
					float time;
					bool val;

					anKeyDataBase<bool>* new_anim = new anKeyDataBase<bool>(true);
					for ( i = 0 ; i < num_keys ; ++i )
					{
						i_Reader.Read(val);
						i_Reader.Read(time);
						//DBG_LOG2("Reading AVIS: %f, %s", time, val ? "True" : "False");
						new_anim->AddKey(time, val);
					}

					io_It.GetData().SetVisible(new_anim);
					o_VisibleChannels.push_back(new_anim);
					//new_anim->SetLooping(true);
				}
			}
			else if ( name == c_ANLV )
			{
				io_It.AddChild();
				io_It.MoveToChild(children_added);
				children_added++;
				std::string child_name;
				read_ANLV(	i_Reader,
							i_Version,
							i_Size,
							child_name,
							io_It,
							o_TranslationChannels,
							o_RotationChannels,
							o_ScaleChannels,
							o_VisibleChannels,
							i_Depth+1);
				io_It.MoveToParent();
			}

			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::read_ANLV");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}
}


//------------------------------------------------------------------------
// BLend Shape animation, a named set of weights varying over time.
//------------------------------------------------------------------------
void read_ABLS(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::map<std::string, smdlMorphAnimKeys>& o_BlendShapeKeys,
				std::vector<anKeyData<float>*>& o_MorphChannels)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	envType::UInt16 num_keys;
	envType::UInt16 i;

	std::string target_name, alias_name;

	try
	{
		while ( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_NNAM)
			{
				i_Reader.Read(target_name);
				//DBG_LOG(" blend shape animation named, " << target_name.c_str());
			}
			else if ( name == c_WNAM)
			{
				i_Reader.Read(alias_name);
				//DBG_LOG(" blend shape animation aliased, " << alias_name.c_str());
			}
			else if ( name == c_AMKY )
			{
				// morph weight keys
				i_Reader.Read(num_keys);
//				DBG_LOG(" blend shape animation num keys, " << num_keys);
				if ( num_keys )
				{
					float time;
					envType::Float32 val;

					anKeyData<float>* new_anim = new anKeyData<float>();
					for ( i = 0 ; i < num_keys ; ++i )
					{
						i_Reader.Read(val);
						i_Reader.Read(time);
						new_anim->AddKey(time, val);
					}

					if (target_name.empty())
					{
						DBG_ASSERT(!target_name.empty(), "No name for blend shape animation.");
						throw mdlInvalidModelFileX(i_Reader.GetLocator());
					}

					// Submit keys under both names, the name of the
					// blend shape node and the name of the alias.
					// In newer formats, the alias is important to distinguish
					// between multiple weights in one blend shape node.
					// However, we still have to be able to play new animations
					// on old file formats.
					smdlMorphAnimKeys keys;
					keys.SetBlend(new_anim);
					o_BlendShapeKeys[target_name] = keys;
					if (!alias_name.empty())
						o_BlendShapeKeys[alias_name] = keys;
					o_MorphChannels.push_back(new_anim);
				}
			}
			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::read_ABLS");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}
}

}	// end of local namespace



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
					float &o_FramesPerSecond)
{
	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//o_Animation.SetHead(smdlGeoAnimKeys());
	//o_NameOfRoot = "";
	std::string name_of_root;
	o_FramesPerSecond = g3dConstants::c_fDefaultFrameRate;

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	reader.ReadChunkHeader(name, version, size);
	DBG_ASSERT(name == c_ANIH, "Expected ANIH chunk for animation file");

	bool bReadANLV = false;
	try
	{
		while ( reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_NNAM )
			{
				// sub animations have name in root joint, tells how to
				// connect this animation
				//reader.Read(o_NameOfRoot);
				reader.Read(name_of_root);
			}
			else if ( name == c_AFPS )
			{
				// Read fps to play this animation
				reader.Read(o_FramesPerSecond);
			}
			else if ( name == c_ANLV )
			{
				bReadANLV = true;

				shared_ptr< smdlTree<smdlGeoAnimKeys> > key_root( new smdlTree<smdlGeoAnimKeys>() );
				key_root->SetHead(smdlGeoAnimKeys());
				std::string root_name;

				const int depth = 0;
				read_ANLV(	reader,
							version,
							size,
							root_name,
							key_root->GetIterator(),
							o_TranslationChannels,
							o_RotationChannels,
							o_ScaleChannels,
							o_VisibleChannels,
							depth);

				// If we have read a NNAM chunk earlier, then it is an old file format
				// and we should use that string to name this set of keys (old files
				// only had one root).
				if (!name_of_root.empty())
					o_KeyRoots[name_of_root] = key_root;
				else
					o_KeyRoots[root_name] = key_root; // otherwise, use the string in the ANLV chunk

			}
			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::LoadAnimation");
		throw mdlInvalidModelFileX(i_Locator);
	}

	DBG_ASSERT(bReadANLV, "Did not read animation levels in LoadAnimation");

}

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
					std::map<std::string, smdlGeoAnimKeys>& o_SkinKeys)
{
	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//o_JointKeys.SetHead(smdlGeoAnimKeys());
	o_FramesPerSecond = g3dConstants::c_fDefaultFrameRate;

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	reader.ReadChunkHeader(name, version, size);
	DBG_ASSERT(name == c_ACHR, "Expected ACHR chunk for character animation file");

	std::string name_of_root;
	//o_NameOfRoot = "";
	o_DeltaAnimation = false;
	fsFileStream::FilePosType geom_cache_offset = 0;
	try
	{
		while ( reader.ReadChunkHeader(name, version, size) )
		{
			//DBG_LOG("LoadCharacterAnimation, got chunk: " << (char*)(&name));

			if ( name == c_NNAM )
			{
				// sub animations have name in root joint, tells how to
				// connect this animation
				reader.Read(name_of_root);
				//DBG_LOG("Read name of root: " << name_of_root.c_str());
			}
			else if ( name == c_DELT)
			{
				// some sub animations have translation and rotation data
				// that are only "deltas" from the base pose so that their
				// info can be added to the main animation.
				reader.Read(o_DeltaAnimation);
				//DBG_LOG("Read delta animation: " << (o_DeltaAnimation ? "True" : "False"));
			}
			else if ( name == c_AFPS )
			{
				// Read fps to play this animation
				reader.Read(o_FramesPerSecond);
			}
			else if ( name == c_BGFR )
			{
				// Read first frame of animation when exported
				reader.Read(o_BeginFrame);
			}
			else if ( name == c_ANLV )
			{
//				DBG_LOG("Got animation key root");

				shared_ptr< smdlTree<smdlGeoAnimKeys> > key_root( new smdlTree<smdlGeoAnimKeys>() );
				key_root->SetHead(smdlGeoAnimKeys());
				std::string root_name;

				// animation tree
				const int depth = 0;
				read_ANLV(	reader,
							version,
							size,
							root_name,
							key_root->GetIterator(),
							o_TranslationChannels,
							o_RotationChannels,
							o_ScaleChannels,
							o_VisibleChannels,
							depth);

				//DBG_LOG("Read ANLV, name of root: " << root_name.c_str());

				// If we have read a NNAM chunk earlier, then it is an old file format
				// and we should use that string to name this set of keys (old files
				// only had one root).
				if (!name_of_root.empty())
					o_KeyRoots[name_of_root] = key_root;
				else
					o_KeyRoots[root_name] = key_root; // otherwise, use the string in the ANLV chunk
			}
			else if (name == c_ABLS)
			{
//				DBG_LOG("Got blend shape animation");
				// blend shape animation
				read_ABLS(  reader,
							version,
							size,
							o_BlendShapeKeys,
							o_MorphChannels);
			}
			else if ( name == c_VTXA )
			{
//				DBG_LOG("Got baked vertex animation");
				// baked vertex animation
				std::string surface_name;
				smdlVertexAnimKeys keys;
#ifdef USE_DYNAMIC_VERTEX_ANIM
				vtxVertexAnimImport::ReadDynamicVertexAnimation(reader, version, size, surface_name, keys, o_VertexFrames);
#else
				vtxVertexAnimImport::ReadStaticVertexAnimation(reader, version, size, surface_name, keys, o_VertexFrames);
#endif
				//keys.PruneFrames();
				if (!surface_name.empty())
					o_VertexKeys[surface_name] = keys;
			}
			else if (name == c_GCHE)
			{
				// goemetry cache holds reference frames and compressed 
				// deltas blocks for compressed vertex animation
				geom_cache_offset = file.GetFilePos();
			}
			else if ( name == c_VTXC )
			{
				//DBG_LOG("Got compressed baked vertex animation");
				// baked vertex animation
				std::string surface_name;
				smdlVertexAnimKeys keys;
				vtxVertexAnimImport::ReadCompressedVertexAnimation(reader, version, size, 
					geom_cache_offset, surface_name, keys, o_VertexFrames);
				if (!surface_name.empty())
					o_VertexKeys[surface_name] = keys;
			}
			else if ( name == c_ASKN )
			{
				//DBG_LOG("Got skin visibility animation");
				read_ASKN(	reader,
							version,
							size,
							o_SkinKeys,
							o_VisibleChannels);
			}
				
			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::LoadCharacterAnimation");
		throw mdlInvalidModelFileX(i_Locator);
	}
}

//------------------------------------------------------------------------
//	LoadVertexAnimation loads the baked vertex and normal 
//	animation from the given file.
//------------------------------------------------------------------------
void LoadVertexAnimation( const fsLocator& i_Locator,
					std::vector<smdlVertexAnimKeys>& o_VertexKeys,
					smdlVertexFrames& o_VertexFrames,
					float &o_FramesPerSecond)
{
	o_FramesPerSecond = g3dConstants::c_fDefaultFrameRate;

	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	try
	{
		while ( reader.ReadChunkHeader(name, version, size) )
		{
//			DBG_LOG("LoadVertexAnimation, got chunk: " << (char*)(&name));
			if ( name == c_VTXA )
			{
				smdlVertexAnimKeys keys;
				std::string surface_name; // ignoring surface name in VTA files
#ifdef USE_DYNAMIC_VERTEX_ANIM
				vtxVertexAnimImport::ReadDynamicVertexAnimation(reader, version, size, surface_name, keys, o_VertexFrames);
#else
				vtxVertexAnimImport::ReadStaticVertexAnimation(reader, version, size, surface_name, keys, o_VertexFrames);
#endif
				o_VertexKeys.push_back(keys);
			}
			else if ( name == c_AFPS )
			{
				// Read fps to play this animation
				reader.Read(o_FramesPerSecond);
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlAnimImport::LoadVertexAnimation");
		throw mdlInvalidModelFileX(i_Locator);
	}
}

}	// end of namespace

