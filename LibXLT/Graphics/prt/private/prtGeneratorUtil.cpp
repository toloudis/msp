/*****************************************************************************
**	prtGeneratorUtil.cpp
**
**		see .hpp
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtGeneratorUtil.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/an/an3StateAnimation.hpp"
#include "Graphics/an/anConstantAnimation.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/prt/prtBlockEmitter.hpp"
#include "Graphics/prt/prtCircleEmitter.hpp"
#include "Graphics/prt/prtConeParticleGenerator.hpp"
#include "Graphics/prt/prtPointEmitter.hpp"
#include "Graphics/prt/prtSpiralParticleGenerator.hpp"
#include "Graphics/prt/prtStaticParticleGenerator.hpp"

#include <map>


//============================================================================
//	Particle Generator file format
//
//	The format for the particle generators (or actually the particle
//	generator template) is a single TPAR chunk.  The TPAR chunk is
//	a container which can contain:
//
//	GENP (GENeral Parameters) - parameters which can apply to all
//								particle generators.  Not a container.
//		int8: generator_type -	this is the same value as in the
//								prtParticleGeneratorTemplate::Type enum.
//		int8: emitter_type -	this is the same value as in the 
//								prtParticleGeneratorTemplate::EmitterType enum.
//		vector3: emitter_scale - the "size" of the emitter
//
//	SGPP (Sprite Group Particle Parameters) -	parameters which apply to
//												sprite group particle 
//												generators.  Not a container.
//		int8: scale_mode -	This is the same value as in the 
//							prtSpriteGroupParticleGenerator::ScaleMode enum.
//		int8: uva_mode -	This is the same value as in the 
//							prtSpriteGroupParticleGenerator::UVAMode enum.
//		int8: render_mode -	This is the same value as in the 
//							prtSpriteGroupParticleGenerator::RenderMode enum.
//		null_terminated_double_byte_string: texture_name
//		int8: num_alpha_keys
//		float32: time1
//		float32: value1
//		float32: time2
//		float32: value2
//		...up to num_alpha_keys
//		bool: render streaks
//		
//	TDPP (3D Particle Parameters) -	parameters which apply to 3D particle
//									generators.  Not a container.
//		null_terminated_double_byte_string: geometry_name
//
//	PARP (PARameter Parameters)
//	
//		int8: num_parameters - The number of float parameters
//		float32: parameter1
//		float32: parameter2
//		...up to num_parameters
//
//
//
//============================================================================
namespace
{
	const chDefs::Name c_TPAR = chDefs::MakeName('T', 'P', 'A', 'R');
	const chDefs::Name c_GENP = chDefs::MakeName('G', 'E', 'N', 'P');
	const chDefs::Name c_SGPP = chDefs::MakeName('S', 'G', 'P', 'P');
	const chDefs::Name c_TDPP = chDefs::MakeName('T', 'D', 'P', 'P');
	const chDefs::Name c_PARP = chDefs::MakeName('P', 'A', 'R', 'P');

	bool	l_bAllowParticles = true;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void read_TPAR(chReader& i_Reader, prtParticleGeneratorTemplate& o_Template)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		i_Reader.ReadChunkHeader(name, version, size);

		DBG_ASSERT(name == c_TPAR, "Expected TPAR chunk");
		if ( name != c_TPAR )
			throw chInvalidChunkX();

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{	
				if ( name == c_GENP )
				{
					envType::Int8 generator_type;
					envType::Int8 emitter_type;
					envType::Float32 x, y, z;
					i_Reader.Read(generator_type);
					i_Reader.Read(emitter_type);
					i_Reader.Read(x);
					i_Reader.Read(y);
					i_Reader.Read(z);

					o_Template.SetType(prtParticleGeneratorTemplate::Type(generator_type));
					o_Template.SetEmitterType(prtParticleGeneratorTemplate::EmitterType(emitter_type));
					o_Template.SetEmitterScale(maVector3d(x, y, z));
				}
				else if ( name == c_SGPP )
				{
					envType::Int8 scale_mode;
					envType::Int8 render_mode;
					envType::Int8 uva_mode;
					itString texture_name;
					envType::Int8 num_alpha_keys;
					
					i_Reader.Read(scale_mode);
					i_Reader.Read(uva_mode);
					i_Reader.Read(render_mode);
					i_Reader.Read(texture_name);
					i_Reader.Read(num_alpha_keys);

					//	remove NULLs from texture_name
					int last_char = texture_name.GetLength() - 1;
					DBG_ASSERT(texture_name[last_char] == 0, "expected terminating NULL");
					texture_name.RemoveCharAt(last_char);
					fsLocator texture_locator;
					texture_locator.Push(texture_name);

					if ( num_alpha_keys )
					{
						if ( num_alpha_keys == 1 )
						{
							//	we can do this with a constant animation
							envType::Float32 time;
							envType::Float32 value;

							i_Reader.Read(time);
							i_Reader.Read(value);
							anConstantAnimation<float> animation(value);
							o_Template.SetAlphaAnimation(animation);
						}
						else if ( num_alpha_keys == 2 )
						{
							//	we need an an2StateAnimation
							envType::Float32 time1, time2;
							envType::Float32 value1, value2;

							i_Reader.Read(time1);
							i_Reader.Read(value1);
							i_Reader.Read(time2);
							i_Reader.Read(value2);
							an2StateAnimation<float> animation(value1, value2, 1.0f);
							o_Template.SetAlphaAnimation(animation);
						}
						else if ( num_alpha_keys == 3 )
						{
							//	an3StateAnimation
							envType::Float32 time1, time2, time3, time2b;
							envType::Float32 value1, value2, value3;

							i_Reader.Read(time1);
							i_Reader.Read(value1);
							i_Reader.Read(time2);
							i_Reader.Read(value2);
							i_Reader.Read(time3);
							i_Reader.Read(value3);

							//	read in middle percent end
							time2b = time2;	// for older versions
							if (version >= 1)
							{
								i_Reader.Read(time2b);
							}

							an3StateAnimation<float> animation(value1, value2, value3, time2, time2b, time3);
							o_Template.SetAlphaAnimation(animation);						
						}
						else
						{
							//	we need an anKeyAnimation
							std::vector<float> times(num_alpha_keys);
							std::vector<float> values(num_alpha_keys);

							int i;
							for( i = 0 ; i < num_alpha_keys ; i++ )
							{
								envType::Float32 time;
								envType::Float32 value;

								i_Reader.Read(time);
								i_Reader.Read(value);
								times[i] = time;
								values[i] = value;				
							}

							anKeyAnimation<float> animation(values[0]);
							
							for( i = 1 ; i < num_alpha_keys ; i++ )
								animation.AddKey(times[i], values[i]);
						
							o_Template.SetAlphaAnimation(animation);
						}
					}

					bool bRenderStreaks = false;
					if (version >= 2)
					{
						// Added in version 2:
						i_Reader.Read(bRenderStreaks);
					}

					o_Template.SetScaleMode(prtSpriteGroupParticleGenerator::ScaleMode(scale_mode));
					o_Template.SetRenderMode(prtSpriteGroupParticleGenerator::RenderMode(render_mode));
					o_Template.SetUVAMode(prtSpriteGroupParticleGenerator::UVAMode(uva_mode));
					o_Template.SetTextureLocator(texture_locator);
					o_Template.SetRenderStreaks(bRenderStreaks);
				}
				else if ( name == c_TDPP )
				{
					itString geometry_name;
					i_Reader.Read(geometry_name);

					//	remove NULLs from texture_name
					int last_char = geometry_name.GetLength() - 1;
					DBG_ASSERT(geometry_name[last_char] == 0, "expected terminating NULL");
					geometry_name.RemoveCharAt(last_char);
					fsLocator geometry_locator;
					geometry_locator.Push(geometry_name);
					o_Template.SetGeometryLocator(geometry_locator);
				}
				else if ( name == c_PARP )
				{
					envType::Int8 num_parameters;
					i_Reader.Read(num_parameters);

					// In older versions of files, newer parameters need
					// to be skipped over and set to default values.
					std::map<int, envType::Float32> to_skip;

					if ( version < 1 )	// handle the presim time parameter
					{
						to_skip[prtParticleGenerator::e_PreSimTime] = 0.0f;
					}
					if ( version < 2 )
					{
						to_skip[prtSpriteGroupParticleGenerator::e_StreakLength] = 1.0f / 24.0f;
						to_skip[prtSpriteGroupParticleGenerator::e_StreakTaper] = 1.0f;
						to_skip[prtSpriteGroupParticleGenerator::e_StreakFade] = 1.0f;
					}

					// Allocate number of parameters equal to number read plus
					// the number of new parameters needed to skip
					int total_num_params = num_parameters + to_skip.size();
					o_Template.SetNumParameters(total_num_params);
					envType::Float32 param;
					for (int i = 0; i < total_num_params ; i++ )
					{
						std::map<int, envType::Float32>::iterator it = to_skip.find(i);
						if ( it != to_skip.end())
						{
							// Skipped parameter, set to default value
							o_Template.SetParameter(i, it->second);
						}
						else 
						{
							// Read and set the parameter from file
							i_Reader.Read(param);
							o_Template.SetParameter(i, param);
						}
					}

				}

				i_Reader.FinishChunk();
			}	
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in prtGeneratorUtil.cpp - read_TPAR");
			throw;
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_GENP(chWriter& o_Writer, const prtParticleGeneratorTemplate& i_Template)
	{
		const int c_GENP_VERSION = 0;
		o_Writer.WriteChunkHeader(c_GENP, c_GENP_VERSION, false);

		envType::Int8 generator_type = i_Template.GetType();
		envType::Int8 emitter_type = i_Template.GetEmitterType();
		envType::Float32 x, y, z;
		x = i_Template.GetEmitterScale().m_X;
		y = i_Template.GetEmitterScale().m_Y;
		z = i_Template.GetEmitterScale().m_Z;
		o_Writer.Write(generator_type);
		o_Writer.Write(emitter_type);
		o_Writer.Write(x);
		o_Writer.Write(y);
		o_Writer.Write(z);

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_SGPP(chWriter& o_Writer, const prtParticleGeneratorTemplate& i_Template)
	{
		//	TODO - break apart this chunk into smaller chunks.  better 
		//	for versioning.

		//	SGPP Chunk
		//	version 1 - added another value to 3 anim key state animtion (middle % end)
		//	version 2 - added bool "render streaks"
		//
		const int c_SGPP_VERSION = 2;
		o_Writer.WriteChunkHeader(c_SGPP, c_SGPP_VERSION, false);

		envType::Int8 scale_mode	= i_Template.GetScaleMode();
		envType::Int8 render_mode	= i_Template.GetRenderMode();
		envType::Int8 uva_mode		= i_Template.GetUVAMode();
		itString texture_name		= i_Template.GetTextureLocator().GetLastName();
		// null-terminating not needed anymore
		//texture_name += 0;
		envType::Int8 num_alpha_keys;

		o_Writer.Write(scale_mode);
		o_Writer.Write(uva_mode);
		o_Writer.Write(render_mode);
		o_Writer.Write(texture_name.GetString());
		
		const anKeyAnimation<float>* key_anim = dynamic_cast<const anKeyAnimation<float>*>(&(i_Template.GetAlphaAnimation()));
		
		if ( key_anim )
		{
			num_alpha_keys = key_anim->GetNumKeys();
			o_Writer.Write(num_alpha_keys);
			envType::Int8 i;
			for( i = 0 ; i < num_alpha_keys ; i++ )
			{
				envType::Float32 time, value;
				key_anim->GetKeyData(i, time, value);
				o_Writer.Write(time);
				o_Writer.Write(value);
			}
		}
		else
		{
			const an3StateAnimation<float>* threestate_anim = dynamic_cast<const an3StateAnimation<float>*>(&(i_Template.GetAlphaAnimation()));
			if ( threestate_anim )
			{
				num_alpha_keys = 3;
				o_Writer.Write(num_alpha_keys);

				envType::Float32 time, value;
				time = 0.0f;
				value = threestate_anim->GetFirstValue();
				o_Writer.Write(time);
				o_Writer.Write(value);

				time = threestate_anim->GetMidTimeStart();
				value = threestate_anim->GetSecondValue();
				o_Writer.Write(time);
				o_Writer.Write(value);

				time = 1.0f;
				value = threestate_anim->GetThirdValue();
				o_Writer.Write(time);
				o_Writer.Write(value);

				time = threestate_anim->GetMidTimeEnd();
				o_Writer.Write(time);
			}
			else
			{
				const an2StateAnimation<float>* twostate_anim = dynamic_cast<const an2StateAnimation<float>*>(&(i_Template.GetAlphaAnimation()));
				if ( twostate_anim )
				{
					num_alpha_keys = 2;
					o_Writer.Write(num_alpha_keys);
					envType::Float32 time, value;
					time = 0.0f;
					value = twostate_anim->GetFirstValue();
					o_Writer.Write(time);
					o_Writer.Write(value);
					time = twostate_anim->GetLength();
					value = twostate_anim->GetSecondValue();
					o_Writer.Write(time);
					o_Writer.Write(value);
				}
				else
				{
					const anConstantAnimation<float>* constant_anim = dynamic_cast<const anConstantAnimation<float>*>(&(i_Template.GetAlphaAnimation()));
					if ( constant_anim )
					{
						num_alpha_keys = 1;
						o_Writer.Write(num_alpha_keys);
						o_Writer.Write(envType::Float32(0.0f));
						o_Writer.Write(envType::Float32(constant_anim->GetValue(0.0f)));
					}
					else
					{
						o_Writer.Write(envType::Int8(0));
					}
				}
			}
		}

		// Added in version 2:
		bool bRenderStreaks = i_Template.GetRenderStreaks();
		o_Writer.Write(bRenderStreaks);

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_TDPP(chWriter& o_Writer, const prtParticleGeneratorTemplate& i_Template)
	{
		const int c_TDPP_VERSION = 0;
		o_Writer.WriteChunkHeader(c_TDPP, c_TDPP_VERSION, false);

		itString geometry_name = i_Template.GetGeometryLocator().GetLastName();
		// null-terminating not needed anymore
		//geometry_name += 0;
		o_Writer.Write(geometry_name.GetString());
		
		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_PARP(chWriter& o_Writer, const prtParticleGeneratorTemplate& i_Template)
	{
		const int c_PARP_VERSION = 2;
		o_Writer.WriteChunkHeader(c_PARP, c_PARP_VERSION, false);

		envType::Int8 num_params = i_Template.GetNumParameters();

		o_Writer.Write(num_params);

		envType::Int8 i;
		for( i = 0 ; i < num_params ; i++ )
		{
			envType::Float32 val = i_Template.GetParameter(i);
			o_Writer.Write(val);
		}

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_TPAR(chWriter& o_Writer, const prtParticleGeneratorTemplate& i_Template)
	{
		const int c_TPAR_VERSION = 0;
		o_Writer.WriteChunkHeader(c_TPAR, c_TPAR_VERSION, true);

		write_GENP(o_Writer, i_Template);

		int generator_type = i_Template.GetType();
		if ( generator_type == prtParticleGeneratorTemplate::e_Static ||
			 generator_type == prtParticleGeneratorTemplate::e_Cone ||
			 generator_type == prtParticleGeneratorTemplate::e_Spiral )
		{
			write_SGPP(o_Writer, i_Template);
		}
		else
		{
			write_TDPP(o_Writer, i_Template);
		}

		write_PARP(o_Writer, i_Template);

		o_Writer.FinishChunk();
	}

}

//----------------------------------------------------------------------------
//	Read reads the template from a binary chunk file given by the locator.
//----------------------------------------------------------------------------
void prtGeneratorUtil::Read(const fsLocator& i_Locator, prtParticleGeneratorTemplate& o_Template)
{
	gfFileBin particle_file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	particle_file.ReadHeader();
	chBinReader reader(particle_file);

	read_TPAR(reader, o_Template);	
}

//----------------------------------------------------------------------------
//	Write writes the template to a binary chunk file given by the locator.
//----------------------------------------------------------------------------
void prtGeneratorUtil::Write(const fsLocator& i_Locator, const prtParticleGeneratorTemplate& i_Template)
{
	//if ( fsFileUtil::FileExists(i_Locator) )
	//	fsFileUtil::DeleteFile(i_Locator);
	if ( !fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::CreateFile(i_Locator);

	gfFileBin particle_file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	const int c_TPARFILE_VERSION = 0;
	particle_file.WriteHeader(0, c_TPARFILE_VERSION, 0);
	chBinWriter writer(particle_file);

	write_TPAR(writer, i_Template);	
}

//----------------------------------------------------------------------------
//	MakeTemplateFromGenerator does just that.  To make generator from a
//	template, you'll want to use the scScene.
//----------------------------------------------------------------------------
void prtGeneratorUtil::MakeTemplateFromGenerator(	prtParticleGeneratorTemplate& o_Template,
													const prtParticleGenerator& i_Generator)
{
	prtParticleGeneratorTemplate::Type generator_type;
	
	if ( dynamic_cast<const prtStaticParticleGenerator*>(&i_Generator) )
		generator_type = prtParticleGeneratorTemplate::e_Static;
	else if ( dynamic_cast<const prtConeParticleGenerator*>(&i_Generator) )
		generator_type = prtParticleGeneratorTemplate::e_Cone;
	else if ( dynamic_cast<const prtSpiralParticleGenerator*>(&i_Generator) )
		generator_type = prtParticleGeneratorTemplate::e_Spiral;
//	else if ( dynamic_cast<const prtCone3DParticleGenerator*>(&i_Generator) )
//		generator_type = prtParticleGeneratorTemplate::e_Cone3D;
	else
		DBG_ASSERT(false, "Unknown particle generator type");

	prtParticleGeneratorTemplate::EmitterType emitter_type;
	maVector3d emitter_scale(1, 1, 1);
	const prtPointEmitter* point_emitter = dynamic_cast<const prtPointEmitter*>(i_Generator.GetEmitter());
	if ( point_emitter || (i_Generator.GetEmitter() == NULL) )
		emitter_type = prtParticleGeneratorTemplate::e_Point;
	else
	{
		const prtCircleEmitter* circle_emitter = dynamic_cast<const prtCircleEmitter*>(i_Generator.GetEmitter());
		if ( circle_emitter )
		{
			emitter_type = prtParticleGeneratorTemplate::e_Circle;
			emitter_scale.m_X = emitter_scale.m_Y = emitter_scale.m_Z = circle_emitter->GetRadius();
		}
		else
		{
			const prtBlockEmitter* block_emitter = dynamic_cast<const prtBlockEmitter*>(i_Generator.GetEmitter());
			if ( block_emitter )
			{
				emitter_type = prtParticleGeneratorTemplate::e_Block;
				block_emitter->GetSides(emitter_scale.m_X, emitter_scale.m_Y, emitter_scale.m_Z);
			}
			else
				DBG_ASSERT(block_emitter, "Unknown emitter type");
		}
	}

	const prtSpriteGroupParticleGenerator* sg_generator = dynamic_cast<const prtSpriteGroupParticleGenerator*>(&i_Generator);
	if ( sg_generator )
	{
		prtSpriteGroupParticleGenerator::RenderMode render_mode = sg_generator->GetRenderMode();
		prtSpriteGroupParticleGenerator::UVAMode uva_mode = sg_generator->GetUVAMode();
		prtSpriteGroupParticleGenerator::ScaleMode scale_mode = sg_generator->GetScaleMode();

		o_Template.SetRenderMode(render_mode);
		o_Template.SetUVAMode(uva_mode);
		o_Template.SetScaleMode(scale_mode);
		o_Template.SetAlphaAnimation(*(sg_generator->GetAlphaProfile()));
	}

	o_Template.SetType(generator_type);
	o_Template.SetEmitterType(emitter_type);
	o_Template.SetEmitterScale(emitter_scale);

	int num_params = i_Generator.GetNumParameters();
	o_Template.SetNumParameters(num_params);
	int i;

	for( i = 0 ; i < num_params ; i++ )
		o_Template.SetParameter(i, i_Generator.GetParameter(i));
}

//----------------------------------------------------------------------------
//	MakeGenerator makes a generator from the parameters in the given template.
//
//	NOTE: this function will return a NULL if the SetAllowParticles is set
//	to false.
//----------------------------------------------------------------------------
prtParticleGenerator* prtGeneratorUtil::MakeGenerator( const prtParticleGeneratorTemplate& i_Template, float i_SimulationTime )
{
	if ( !l_bAllowParticles )
	{
		return NULL;
	}

	prtParticleGenerator* pGenerator = NULL;
	//prt3DParticleGenerator* p3DGenerator = NULL;
	prtSpriteGroupParticleGenerator* pSGGenerator = NULL;

	switch( i_Template.GetType() )
	{
		case prtParticleGeneratorTemplate::e_Static:
			pGenerator = pSGGenerator = new prtStaticParticleGenerator( i_SimulationTime );
			break;
		case prtParticleGeneratorTemplate::e_Cone:
			pGenerator = pSGGenerator = new prtConeParticleGenerator( i_SimulationTime );
			break;
		case prtParticleGeneratorTemplate::e_Spiral:
			pGenerator = pSGGenerator = new prtSpiralParticleGenerator( i_SimulationTime );
			break;
		case prtParticleGeneratorTemplate::e_Cone3D:
		//	pGenerator = p3DGenerator = new prtCone3DParticleGenerator( i_SimulationTime );
			break;
		default:
			DBG_ASSERT(false, "Unknown generator type");
			break;
	}

	/*if ( p3DGenerator )
	{
	// this needs to be switched to use entImport

		fsLocator tex_dir = i_Template.GetTextureLocator();

		//	load model
		fsLocator g_locator = i_Template.GetGeometryLocator();
		if ( g_locator.GetLastName().HasSubString(itString(".mx")) )
		{
			//	it's a simple (one fragment) model
			mdlImport::LoadWorldFragments( g_locator,
										   tex_dir,
										   p3DGenerator->GetFragments(),
										   p3DGenerator->GetMaterials(),
										   p3DGenerator->GetTextures() );
			p3DGenerator->SetModelType( prt3DParticleGenerator::e_Simple );
		}
		else if ( g_locator.GetLastName().HasSubString( itString(".mhx") ) )
		{
			//	it's a hierarchical model
			g3dSceneNode* pHierarchy;

			mdlImport::LoadHierarchicalModel( g_locator,
											  tex_dir,
											  pHierarchy,
											  p3DGenerator->GetFragments(),
											  p3DGenerator->GetMaterials(),
											  p3DGenerator->GetTextures() );
			p3DGenerator->SetModelType( prt3DParticleGenerator::e_Hierarchical );
			p3DGenerator->SetHierarchy( pHierarchy );
		}
	}
	else */
	if ( pSGGenerator )
	{
		pSGGenerator->SetUVAMode(i_Template.GetUVAMode());
		pSGGenerator->SetScaleMode(i_Template.GetScaleMode());
		pSGGenerator->SetRenderMode(i_Template.GetRenderMode());
		pSGGenerator->SetAlphaProfile(i_Template.GetAlphaAnimation());
		pSGGenerator->SetTexture(i_Template.GetTexture());
		pSGGenerator->SetRenderStreaks(i_Template.GetRenderStreaks());
	}

	pGenerator->SetEmitter( i_Template.MakeEmitter() );

	int nParameters = i_Template.GetNumParameters();
	for( int i = 0 ; i < nParameters ; ++i )
	{
		pGenerator->SetParameter( i, i_Template.GetParameter(i) );
	}

	return pGenerator;
}


//----------------------------------------------------------------------------
//	Replace the emitter in the generator with the passed in type.  if the 
//	types are the same, this function will do nothing.
//----------------------------------------------------------------------------
void prtGeneratorUtil::ReplaceGeneratorEmitter( prtParticleGenerator* io_pGenerator,
												prtParticleGeneratorTemplate::EmitterType i_EmitterType )
{
	DBG_ASSERT( io_pGenerator != 0, "Cannot replace an emitter on a NULL generator" );
	if (io_pGenerator == 0)
		return;

	switch ( i_EmitterType )
	{
		case prtParticleGeneratorTemplate::e_Point:
		{
			if ( dynamic_cast<prtPointEmitter*>( io_pGenerator->GetEmitter() ) == 0 )
			{
				io_pGenerator->SetEmitter( new prtPointEmitter() );
			}
			break;
		}
		case prtParticleGeneratorTemplate::e_Block:
		{
			if ( dynamic_cast<prtBlockEmitter*>( io_pGenerator->GetEmitter() ) == 0 )
			{
				io_pGenerator->SetEmitter( new prtBlockEmitter( 5.0f ) );
			}
			break;
		}
		case prtParticleGeneratorTemplate::e_Circle:
		{
			if ( dynamic_cast<prtCircleEmitter*>( io_pGenerator->GetEmitter() ) == 0 )
			{
				io_pGenerator->SetEmitter( new prtCircleEmitter( 5.0f ) );
			}
			break;
		}
		default:
		{
			DBG_LOG( "Unsupported emitter type " << i_EmitterType );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//	resize the parameters for the template given the current type
//----------------------------------------------------------------------------
void prtGeneratorUtil::ResizeTemplateParameters( prtParticleGeneratorTemplate& io_Template )
{
	prtParticleGeneratorTemplate::Type PGType = io_Template.GetType();

	//	set the number of parameters properly
	//
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			io_Template.SetNumParameters( prtSpriteGroupParticleGenerator::e_NextParameter );
			break;
		}
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			io_Template.SetNumParameters( prtSpiralParticleGenerator::e_NextParameter );
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone:
		{
			io_Template.SetNumParameters( prtConeParticleGenerator::e_NextParameter );
			break;
		}
		case prtParticleGeneratorTemplate::e_Static:
		{
			io_Template.SetNumParameters(prtSpriteGroupParticleGenerator::e_NextParameter);
			break;
		}
		default:
		{
			DBG_ASSERT(false, "Invalid Generator Type" );
			break;
		}
	}

	//DBG_LOG( "Number of Parameters = " << io_Template.GetNumParameters() );
}

//----------------------------------------------------------------------------
//	set the template to default values based on the type
//	this will also resize the template parameters
//----------------------------------------------------------------------------
void prtGeneratorUtil::SetTemplateToDefault( prtParticleGeneratorTemplate& io_Template, bool i_bReplaceGenericParamsToo )
{
	ResizeTemplateParameters( io_Template );

	prtParticleGeneratorTemplate::Type PGType	= io_Template.GetType();

	if ( i_bReplaceGenericParamsToo )
	{
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MinParticleLifetime, 1.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticleLifetime, 2.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, 10.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticles, 1000.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_InitialScale, 1.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_ScaleCoeff, 1.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MinStartAngle, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MaxStartAngle, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularVelocity, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 0.0f );
		io_Template.SetParameter( prtSpriteGroupParticleGenerator::e_PreSimTime, 0.0f );
	}

	//	set the number of parameters properly
	//
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MinEmitSpeed, 1.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MaxEmitSpeed, 1.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_EmitDirectionX, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_EmitDirectionY, 1.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_EmitDirectionZ, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_AccelerationX, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_AccelerationY, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_AccelerationZ, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MinRotStartAngle, 0.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MaxRotStartAngle, 360.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MinRotAngularVel, 90.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_MaxRotAngularVel, 90.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_RotRadius, 3.0f );
			io_Template.SetParameter( prtSpiralParticleGenerator::e_RotRadiusScaleRate, 0.0f );
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone:
		{
			io_Template.SetParameter( prtConeParticleGenerator::e_ConeAngle, 1.57f );
			io_Template.SetParameter( prtConeParticleGenerator::e_MinSpeed, 5.0f );
			io_Template.SetParameter( prtConeParticleGenerator::e_MaxSpeed, 5.0f );
			io_Template.SetParameter( prtConeParticleGenerator::e_AccelerationX, 0.0f );
			io_Template.SetParameter( prtConeParticleGenerator::e_AccelerationY, 0.0f );
			io_Template.SetParameter( prtConeParticleGenerator::e_AccelerationZ, 0.0f );
			break;
		}
		case prtParticleGeneratorTemplate::e_Static:
		{
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			break;
		}
		default:
		{
			DBG_ASSERT(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//	SetAllowParticles() - allow generation of particles to happen or not
//----------------------------------------------------------------------------
void prtGeneratorUtil::SetAllowParticles( bool i_bAllowParticles )
{
	l_bAllowParticles = i_bAllowParticles;
}

