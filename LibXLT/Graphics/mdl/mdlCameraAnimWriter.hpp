/****************************************************************************\
**	mdlCameraAnimWriter.hpp
**
**		mdlCameraAnimWriter.hpp supplies functions used to export camera anim data into
**		our animation file formats	
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_CAMERAANIMWRITER_HPP
#error mdlCameraAnimWriter.hpp multiply included
#endif
#define MDL_CAMERAANIMWRITER_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 
#ifndef  MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif

#include <map>
#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class chWriter;


//============================================================================
//============================================================================
namespace mdlCameraAnimWriter
{
	//----------------------------------------------------------------------------
	//	structure to hold camera animation
	//----------------------------------------------------------------------------
	struct CameraKeys
	{
		std::map<float, maVector3d > m_TranslateKeys;
		envType::UInt8 m_RotationOrder;
		std::map<float, maVector3d  > m_RotateKeys;

		std::map<float, float> m_FocalLengthKeys;

		std::map<float, float> m_CenterOfInterestKeys;
		std::map<float, float> m_HorizFilmAperKeys;
		std::map<float, float> m_VertFilmAperKeys;	

		//----------------------------------------------------------------------------
		//	size determination which helps us in progress reporting
		//----------------------------------------------------------------------------
		static float EstimateSizeOfSingleFrame()
		{
			size_t sizeOfSingleFrame =  sizeof( maVector3d ) + //size of a translate vector
				sizeof( maVector3d ) + //size of rotarte euler angles
				sizeof( float ) +  //size of a focal length key-value
				sizeof(float ) +  //size of a center of interst key-value
				sizeof( float ) +  //size of a horiz filem aperture key-value
				sizeof (float ); //size of a vert film aperture key-value
			return static_cast< float > ( sizeOfSingleFrame );
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		template <class Key, class Type, class Traits, class Alloc,  template < class Key, class Type, class Traits, class Alloc > class Cont >
		static size_t EstimateChannelSize( const Cont< Key, Type, Traits, Alloc > & c )
		{
			typedef Cont< Key, Type, Traits, Alloc >::mapped_type mType;
			return sizeof( mType ) * c.size();
		}
	};

	//----------------------------------------------------------------------------
	//	A call back structure that can be operated on each frame of camera animation
	//	Eg: We could use this to update a progress bar, as we write the camera animation data.
	//----------------------------------------------------------------------------
	struct WriteCameraAnimCallback : public std::unary_function< float , void >
	{
		virtual void operator()( float updateProgress ) const {}
	};

	//----------------------------------------------------------------------------
	//	write the camera animation data
	//	o_Writer is the binary chunk writer
	//	i_Callback, is the callback struct we can call  on each 
	//	i_CameraAnimKeys is the animation data
	//----------------------------------------------------------------------------
	void WriteCameraAnim(chWriter &o_Writer,
						 WriteCameraAnimCallback &i_Callback,
						 const std::string &i_CameraName,
						 CameraKeys &i_CameraAnimkeys,
						 float i_OfsetFrame);

}

