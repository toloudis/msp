/****************************************************************************\
**	cam3dImport.cpp
**
**		cam3dImport supplies functions used to import camera scripts
**	from Maya (written by our Maya plugin).
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dImport.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileUtil.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Tool/cam3d/cam3dAnimKeys.hpp"

#undef FindResource


//============================================================================
//============================================================================
namespace cam3dImport
{

namespace
{

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_ACAM = chDefs::MakeName('A', 'C', 'A', 'M');
	const chDefs::Name c_APKY = chDefs::MakeName('A', 'P', 'K', 'Y');
	const chDefs::Name c_ARKY = chDefs::MakeName('A', 'R', 'K', 'Y');
	const chDefs::Name c_RORD = chDefs::MakeName('R', 'O', 'R', 'D');
	const chDefs::Name c_ASKY = chDefs::MakeName('A', 'S', 'K', 'Y');
	const chDefs::Name c_AFCL = chDefs::MakeName('A', 'F', 'C', 'L');
	const chDefs::Name c_ACOI = chDefs::MakeName('A', 'C', 'O', 'I');
	//const chDefs::Name c_CASP = chDefs::MakeName('C', 'A', 'S', 'P');
	const chDefs::Name c_HAPT = chDefs::MakeName('H', 'A', 'P', 'T');
	//const chDefs::Name c_VAPT = chDefs::MakeName('V', 'A', 'P', 'T');
	const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');
	const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
	const chDefs::Name c_ISEP = chDefs::MakeName('I', 'S', 'E', 'P');
	const chDefs::Name c_ZPLX = chDefs::MakeName('Z', 'P', 'L', 'X');


	// Designed to match Maya's enumeration order
	enum RotationOrder
	{
		e_XYZ = 0,
		e_YZX = 1,
		e_ZXY = 2,
		e_XZY = 3,
		e_YXZ = 4,
		e_ZYX = 5
	};


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void set_ordered_euler(maRotation &o_Rot, RotationOrder i_Order, 
						 float i_X, float i_Y, float i_Z)
	{
		// safe, but slow way
		maRotation rotx(maVector3d(1,0,0), i_X);
		maRotation roty(maVector3d(0,1,0), i_Y);
		maRotation rotz(maVector3d(0,0,1), i_Z);

		// although thew order is XYZ, we have to multiply
		// in the opposite direction because of the organization
		// of our matrices
		switch (i_Order)
		{
		default:
		case e_XYZ:
			o_Rot = (rotz * roty * rotx);
			break;
		case e_YZX:
			o_Rot = (rotx * rotz * roty);
			break;
		case e_ZXY:
			o_Rot = (roty * rotx * rotz);
			break;
		case e_XZY:
			o_Rot = (roty * rotz * rotx);
			break;
		case e_YXZ:
			o_Rot = (rotz * rotx * roty);
			break;
		case e_ZYX:
			o_Rot = (rotx * roty * rotz);
			break;

		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	inline void read_vector(chReader& i_Reader, maVector3d& o_Vector)
	{
		envType::Float32 val;
		i_Reader.Read(val);
		o_Vector.m_X = val;
		i_Reader.Read(val);
		o_Vector.m_Y = val;
		i_Reader.Read(val);
		o_Vector.m_Z = val;
	}



	cam3dAnimKeys* read_ACAM(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		envType::UInt16 num_keys;
		envType::UInt16 i;

		RotationOrder rotation_order = e_XYZ;
		std::auto_ptr< anKeyData<maPoint3d> > pTranslationChannel;
		std::auto_ptr< anKeyData<maRotation> > pRotationChannel;
		std::auto_ptr< anKeyData<float> > pFocalLengthChannel;
		std::auto_ptr< anKeyData<float> > pCenterOfInterestChannel;
		std::auto_ptr< anKeyData<float> > pHorizApertChannel;
		std::auto_ptr< anKeyData<float> > pInterSepChannel;
		std::auto_ptr< anKeyData<float> > pZeroParallaxChannel;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_APKY )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading position keys: " << num_keys);
					if( num_keys )
					{
						float time;
						maPoint3d val;
						pTranslationChannel.reset( new anKeyData<maPoint3d>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							read_vector(i_Reader, val);
							i_Reader.Read(time);
							//DBG_LOG4("Reading position key: %f - %f %f %f", time, val[0], val[1], val[2]);
							pTranslationChannel->AddKey(time, val);
						}

						//pTranslationChannel->SetLooping(true);
					}
				}
				else if (name == c_RORD)
				{
					envType::UInt8 order;
					i_Reader.Read(order);
					rotation_order = (RotationOrder)(order); // enumeration set up to match file format
				}
				else if( name == c_ARKY )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading rotation keys: " << num_keys);
					if( num_keys )
					{
						float time, last_time;
						maPoint3d val, last_val;
						maRotation rot;
						read_vector(i_Reader, val);
						i_Reader.Read(time);
						//DBG_LOG4("Reading rotation key: %f - %f %f %f", time, val[0], val[1], val[2]);

						set_ordered_euler(rot, rotation_order, val.m_X, val.m_Y, val.m_Z);
						//rot.SetEuler(val.m_X, val.m_Y, val.m_Z);
						last_val = val;
						last_time = time;
						pRotationChannel.reset( new anKeyData<maRotation>() );
						pRotationChannel->AddKey(time, rot);

						for( i = 1 ; i < num_keys ; ++i )
						{
							read_vector(i_Reader, val);
							i_Reader.Read(time);
							//DBG_LOG4("Reading rotation key: %f - %f %f %f", time, val[0], val[1], val[2]);

							//const float c_HalfTurn = 180 * maConstants::c_fAngleToRad;
							//float max_turn = maFunctions::Highest(
							//	(maFunctions::Highest( fabsf(val.m_X - last_val.m_X),
							//		fabsf(val.m_Y - last_val.m_Y))),
							//	fabsf(val.m_Z - last_val.m_Z) );

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
							//		pRotationChannel->AddKey(int_time, rot);
							//	}
							//}


							set_ordered_euler(rot, rotation_order, val.m_X, val.m_Y, val.m_Z);
							//rot.SetEuler(val.m_X, val.m_Y, val.m_Z);
							last_val = val;
							last_time = time;
							pRotationChannel->AddKey(time, rot);
						}

						//pRotationChannel->SetLooping(true);
					}
				}
				else if( name == c_AFCL )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading focal length keys: " << num_keys);
					if( num_keys )
					{
						float time, val;

						pFocalLengthChannel.reset( new anKeyData<float>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							i_Reader.Read(val);
							i_Reader.Read(time);
							//DBG_LOG2("Reading focal length key: %f - %f", time, val);
							pFocalLengthChannel->AddKey(time, val);
						}

						//pFocalLengthChannel->SetLooping(true);
					}
				}
				else if( name == c_ACOI )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading center of interest keys: " << num_keys);
					if( num_keys )
					{
						float time, val;

						pCenterOfInterestChannel.reset( new anKeyData<float>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							i_Reader.Read(val);
							i_Reader.Read(time);
							//DBG_LOG2("Reading center of interest key: %f - %f", time, val);
							pCenterOfInterestChannel->AddKey(time, val);
						}

						//pCenterOfInterestChannel->SetLooping(true);
					}
				}
				else if( name == c_HAPT )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading horizontal aperture keys: " << num_keys);
					if( num_keys )
					{
						float time, val;

						pHorizApertChannel.reset( new anKeyData<float>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							i_Reader.Read(val);
							i_Reader.Read(time);
							//DBG_LOG2("Reading horizontal aperture key: %f - %f", time, val);
							pHorizApertChannel->AddKey(time, val);
						}

						//pHorizApertChannel->SetLooping(true);
					}
				}
				else if( name == c_ISEP )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading interaxial separation keys: " << num_keys);
					if( num_keys )
					{
						float time, val;

						pInterSepChannel.reset( new anKeyData<float>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							i_Reader.Read(val);
							i_Reader.Read(time);
							//DBG_LOG2("Reading interaxial separation key: %f - %f", time, val);
							pInterSepChannel->AddKey(time, val);
						}

						//pHorizApertChannel->SetLooping(true);
					}
				}
				else if( name == c_ZPLX )
				{
					i_Reader.Read(num_keys);
					//DBG_LOG("Reading zero parallax keys: " << num_keys);
					if( num_keys )
					{
						float time, val;

						pZeroParallaxChannel.reset( new anKeyData<float>() );
						for( i = 0 ; i < num_keys ; ++i )
						{
							i_Reader.Read(val);
							i_Reader.Read(time);
							//DBG_LOG2("Reading zero parallax key: %f - %f", time, val);
							pZeroParallaxChannel->AddKey(time, val);
						}

						//pHorizApertChannel->SetLooping(true);
					}
				}

				// We don't need to read the VAPT - vertical aperture keys because 
				// our camera model is horizontal fit, so only the width matters.

				//	we don't read ASKY right now

				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in cam3dImport::read_ACAM");
			throw;
		}

		// convert to cam3dAnimKeys
		if ( pTranslationChannel.get() || pRotationChannel.get() || 
			 pFocalLengthChannel.get() || pCenterOfInterestChannel.get() ||
			 pHorizApertChannel.get() || pInterSepChannel.get() || 
			 pZeroParallaxChannel.get())
		{
			// cam3dAnimKeys makes a copy, so we don't want to use "release()" here
			cam3dAnimKeys *pInfo = new cam3dAnimKeys(pTranslationChannel.get(), 
													 pRotationChannel.get(), 
													 pCenterOfInterestChannel.get(), 
													 pFocalLengthChannel.get(), 
													 pHorizApertChannel.get(), 
													 pInterSepChannel.get(), 
													 pZeroParallaxChannel.get());
			return pInfo;
		}
			
		return NULL;
	}

	//------------------------------------------------------------------------
	//	do actual load of camera animation, exceptions caught outside
	//------------------------------------------------------------------------
	cam3dAnimKeys* do_load_animation(	const fsLocator& i_Locator,
									float &o_FramesPerSecond,
									float &o_BeginFrame )
	{
		gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

		//	If this isn't a real Terawatt/XLT binary file this will throw
		file.ReadHeader();

		chBinReader reader(file);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		std::auto_ptr<cam3dAnimKeys> cam_anim_keys;
		try
		{
			while( reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_ACAM)
				{
					cam_anim_keys.reset( read_ACAM(	reader,	version, size) );
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

				reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in cam3dImport::LoadAnimation");
			return NULL;
		}

		return cam_anim_keys.release();
	}

} // end of anonymous namespace

//------------------------------------------------------------------------
//	LoadAnimation loads an camera script animation from a file and
//		returns new script info. Ownership passes to caller.
//------------------------------------------------------------------------
cam3dAnimKeys* LoadAnimation(	const fsLocator& i_Locator,
								float &o_FramesPerSecond,
								float &o_BeginFrame )
{
	try
	{
		return do_load_animation(i_Locator, o_FramesPerSecond, o_BeginFrame);
	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred loading camera animation: " << i_Ex.GetErrorMessage());
	}
	catch (const std::bad_alloc&)
	{
		DBG_ERROR("Out of memory while loading camera animation " <<  i_Locator);
	}
	catch (...)
	{
		DBG_ERROR("General exception while loading camera animation " <<  i_Locator);
	}

	return NULL;
}

}	// end of namespace
