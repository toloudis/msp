/*****************************************************************************
**  chChunkParserUtil.hpp
**
**      chChunkParserUtil contains utility functions for
**		for reading and writing data
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CH_CHUNKPARSERUTIL_HPP
#error chChunkParserUtil.hpp multiply included
#endif
#define CH_CHUNKPARSERUTIL_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
//#ifndef CH_WRITER_HPP
//#include "Core/ch/chWriter.hpp"
//#endif
//#ifndef MA_ANGLE_HPP
//#include "Core/ma/maAngle.hpp"
//#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
//#ifndef MA_INTPOINT2D_HPP
//#include "Core/ma/maIntPoint2d.hpp"
//#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
//#ifndef MA_ROTATION_HPP
//#include "Core/ma/maRotation.hpp"
//#endif
//#ifndef MA_VECTOR4D_HPP
//#include "Core/ma/maVector4d.hpp"
//#endif

#include <string>
#include <vector>

class chReader;
class itString;
//class maIntPoint2d;

namespace chChunkParserUtil
{
	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, bool& o_bValue);

	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, float& o_fValue);

	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, int& o_nValue);

	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, unsigned int& o_nValue);

	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, maPoint3d& o_Point);
	
	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, maPoint2d& o_Point);

	////========================================================================
	////========================================================================
	//void Read(chReader& i_Reader, maIntPoint2d& o_Point);

	////========================================================================
	////========================================================================
	//void Read(chReader& i_Reader, maRotation& o_Orientation);

	////========================================================================
	////========================================================================
	//void Read(chReader& i_Reader, maVector4d& o_Point);

	//========================================================================
	//========================================================================
	void Read(chReader& i_Reader, maFloatRGBA& o_Color);

	//========================================================================
	//========================================================================
	void Read( chReader& i_Reader, itString& o_Name );

	//========================================================================
	//========================================================================
	void Read( chReader& i_Reader, std::string& o_Name );

	////========================================================================
	////========================================================================
	//void Read( chReader& i_Reader, maMatrix4x4& o_Matrix );

	////========================================================================
	////========================================================================
	//void Read( chReader& i_Reader, maAngle& o_Angle );

	////========================================================================
	////========================================================================
	//void Write( chWriter& i_Writer, const itString& i_Name, chDefs::Name i_ChunkName );

	////========================================================================
	////========================================================================
	//void Write( chWriter& i_Writer, const std::string& i_Name, chDefs::Name i_ChunkName );

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,const maFloatRGBA& i_Color, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,const maPoint3d& i_Point, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,const maIntPoint2d& i_Point, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,const maRotation& i_Orientation, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,const maVector4d& i_Point, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,bool i_bValue, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,int i_Value, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer,float i_fValue, chDefs::Name i_ChunkName);

	////========================================================================
	////========================================================================
	//void Write(chWriter& i_Writer, const maMatrix4x4& i_Matrix );

	////========================================================================
	////========================================================================
	//void Write( chWriter& i_Writer, const itString& i_Name );

	////========================================================================
	////========================================================================
	//void Write( chWriter& i_Writer, const maAngle& i_Angle );

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const std::string& i_Name)
	//{
	//	i_Writer.Write(i_Name);
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maVector4d& i_Vec)
	//{
	//	i_Writer.Write(envType::Float32(i_Vec.GetX()));
	//	i_Writer.Write(envType::Float32(i_Vec.GetY()));
	//	i_Writer.Write(envType::Float32(i_Vec.GetZ()));
	//	i_Writer.Write(envType::Float32(i_Vec.GetW()));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maPoint3d& i_Point)
	//{
	//	i_Writer.Write(envType::Float32(i_Point.GetX()));
	//	i_Writer.Write(envType::Float32(i_Point.GetY()));
	//	i_Writer.Write(envType::Float32(i_Point.GetZ()));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maPoint2d& i_Point)
	//{
	//	i_Writer.Write(envType::Float32(i_Point.GetX()));
	//	i_Writer.Write(envType::Float32(i_Point.GetY()));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maIntPoint2d& i_Point)
	//{
	//	i_Writer.Write(envType::Int32(i_Point.GetX()));
	//	i_Writer.Write(envType::Int32(i_Point.GetY()));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maFloatRGBA& i_Color)
	//{
	//	i_Writer.Write(envType::Float32(i_Color.GetAlpha()));
	//	i_Writer.Write(envType::Float32(i_Color.GetRed()));
	//	i_Writer.Write(envType::Float32(i_Color.GetGreen()));
	//	i_Writer.Write(envType::Float32(i_Color.GetBlue()));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,bool i_bValue)
	//{
	//	i_Writer.Write(envType::UInt8(i_bValue));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,unsigned int i_nValue)
	//{
	//	i_Writer.Write(envType::UInt32(i_nValue));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,int i_nValue)
	//{
	//	i_Writer.Write(envType::Int32(i_nValue));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,float i_fValue)
	//{
	//	i_Writer.Write(envType::Float32(i_fValue));
	//}

	////========================================================================
	////========================================================================
	//inline void Write(chWriter& i_Writer,const maRotation& i_Orientation)
	//{
	//	maVector3d axis;
	//	float angle;
	//	i_Orientation.GetValue( axis, angle );
	//	Write(i_Writer, axis);
	//	i_Writer.Write(envType::Float32(angle));
	//}

	//========================================================================
	//========================================================================
	template<class C> void Read(chReader& i_Reader, std::vector<C>& o_Vector)
	{
		// read in the size
		envType::Int32 size;
		i_Reader.Read(size);

		C temp;
		int i = 0;
		for ( i = 0; i < size; ++i )
		{
			Read( i_Reader, temp );
			o_Vector.push_back(temp);
		}
	}

	//========================================================================
	//========================================================================
	//template<class C> void Write( chWriter& i_Writer, const std::vector<C>& i_Vector, chDefs::Name i_ChunkName )
	//{
	//	i_Writer.WriteChunkHeader(i_ChunkName, 0, true);

	//	// Write out the size
	//	i_Writer.Write(envType::Int32(i_Vector.size()));

	//	std::vector<C>::const_iterator it, end;
	//	it = i_Vector.begin();
	//	end = i_Vector.end();
	//	for( ; it != end; ++it)
	//	{
	//		Write(i_Writer, (*it));
	//	}

	//	i_Writer.FinishChunk();
	//}

	//========================================================================
	//========================================================================
	//template<class C> void Write( chWriter& i_Writer, const std::vector<C>& i_Vector )
	//{
	//	// Write out the size
	//	i_Writer.Write(envType::Int32(i_Vector.size()));

	//	std::vector<C>::const_iterator it, end;
	//	it = i_Vector.begin();
	//	end = i_Vector.end();
	//	for( ; it != end; ++it)
	//	{
	//		Write(i_Writer, (*it));
	//	}
	//}

	//========================================================================
	//	Convert the chunk "name" to a string for output to the debug log
	//========================================================================
	void DebugDisplayChunkName(chDefs::Name i_ChunkName);
};
