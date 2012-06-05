/*****************************************************************************
**  chChunkParserUtil.hpp
**
**      chChunkParserUtil contains utility functions for
**		for reading and writing data
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/ch/chChunkParserUtil.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/it/itStringUtil.hpp"


namespace chChunkParserUtil
{

//========================================================================
//========================================================================
void Read(chReader& i_Reader, bool& o_bValue)
{
	envType::UInt8 val;
	i_Reader.Read(val);
	o_bValue = val ? true : false;
}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, float& o_fValue)
{
	envType::Float32 val;
	i_Reader.Read(val);
	o_fValue = val;
}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, int& o_nFirst)
{
	envType::Int32 val;
	i_Reader.Read(val);
	o_nFirst = val;
}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, unsigned int& o_nFirst)
{
	envType::UInt32 val;
	i_Reader.Read(val);
	o_nFirst = val;
}

////========================================================================
////========================================================================
//void Read(chReader& i_Reader, maVector4d& o_Vec)
//{
//	envType::Float32 val;
//	i_Reader.Read(val);
//	o_Vec.SetX(val);
//	i_Reader.Read(val);
//	o_Vec.SetY(val);
//	i_Reader.Read(val);
//	o_Vec.SetZ(val);
//	i_Reader.Read(val);
//	o_Vec.SetW(val);
//}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, maPoint3d& o_Point)
{
	envType::Float32 val;
	i_Reader.Read(val);
	o_Point.SetX(val);
	i_Reader.Read(val);
	o_Point.SetY(val);
	i_Reader.Read(val);
	o_Point.SetZ(val);
}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, maPoint2d& o_Point)
{
	envType::Float32 val;
	i_Reader.Read(val);
	o_Point.SetX(val);
	i_Reader.Read(val);
	o_Point.SetY(val);
}

////========================================================================
////========================================================================
//void Read(chReader& i_Reader, maIntPoint2d& o_Point)
//{
//	envType::Int32 val;
//	i_Reader.Read(val);
//	o_Point.SetX(val);
//	i_Reader.Read(val);
//	o_Point.SetY(val);
//}
//
////========================================================================
////========================================================================
//void Read(chReader& i_Reader, maRotation& o_Orientation)
//{
//	envType::Float32 x, y, z, angle;
//	i_Reader.Read(x);
//	i_Reader.Read(y);
//	i_Reader.Read(z);
//	i_Reader.Read(angle);
//
//	o_Orientation.SetValue( maVector3d( x, y, z ), angle );
//}

//========================================================================
//========================================================================
void Read(chReader& i_Reader, maFloatRGBA& o_Color)
{
	envType::Float32 val;
	i_Reader.Read(val);
	o_Color.SetAlpha(val);
	i_Reader.Read(val);
	o_Color.SetRed(val);
	i_Reader.Read(val);
	o_Color.SetGreen(val);
	i_Reader.Read(val);
	o_Color.SetBlue(val);
}

//========================================================================
//========================================================================
void Read( chReader& i_Reader, itString& o_Name )
{
	itString null_terminated_string;
	i_Reader.Read(null_terminated_string);
	o_Name = itString(0, null_terminated_string.GetLength() - 1, null_terminated_string); // un-NULL terminate
}

//========================================================================
//========================================================================
void Read( chReader& i_Reader, std::string& o_Name )
{
	i_Reader.Read(o_Name);
}

//========================================================================
//========================================================================
//void Read( chReader& i_Reader, maMatrix4x4& o_Matrix )
//{
//	int row, column;
//	envType::Float32 val;
//	for( row = 0 ; row < 4 ; row++ )
//	{
//		for( column = 0 ; column < 4 ; column++ )
//		{
//			i_Reader.Read(val);
//			o_Matrix(row, column) = val;
//		}
//	}
//}
//
////========================================================================
////========================================================================
//void Read( chReader& i_Reader, maAngle& o_Angle )
//{
//	envType::Float32 val;
//	i_Reader.Read(val);
//	o_Angle.SetDegrees(val);
//}
//
////========================================================================
////========================================================================
//void Write( chWriter& i_Writer, const itString& i_Name, chDefs::Name i_ChunkName )
//{
//	itString null_terminated_string = i_Name;
//	null_terminated_string += 0;
//
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	i_Writer.Write(null_terminated_string.GetString());
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write( chWriter& i_Writer, const std::string& i_Name, chDefs::Name i_ChunkName )
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_Name);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,const maFloatRGBA& i_Color, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_Color);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,const maVector4d& i_Vec, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_Vec);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,const maPoint3d& i_Point, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_Point);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,const maIntPoint2d& i_Point, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_Point);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,const maRotation& i_Orientation, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//
//	maVector3d axis;
//	float angle;
//	i_Orientation.GetValue( axis, angle );
//	Write(i_Writer, axis);
//	Write(i_Writer, angle);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,bool i_bValue, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_bValue);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,int i_nValue, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_nValue);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer,float i_fValue, chDefs::Name i_ChunkName)
//{
//	i_Writer.WriteChunkHeader(i_ChunkName, 0, false);
//	Write(i_Writer, i_fValue);
//	i_Writer.FinishChunk();
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer, const maMatrix4x4& i_Matrix )
//{
//	int row, column;
//	envType::Float32 val;
//	for( row = 0 ; row < 4 ; row++ )
//	{
//		for( column = 0 ; column < 4 ; column++ )
//		{
//			val = i_Matrix(row, column);
//			i_Writer.Write(val);
//		}
//	}	
//}
//
////========================================================================
////========================================================================
//void Write(chWriter& i_Writer, const maAngle& i_Angle )
//{
//	envType::Float32 val;
//	val = i_Angle.GetDegrees();
//	i_Writer.Write(val);
//}
//
////========================================================================
////========================================================================
//void Write( chWriter& i_Writer, const itString& i_Name )
//{
//	itString null_terminated_string = i_Name;
//	null_terminated_string += 0;
//	i_Writer.Write(null_terminated_string.GetString());
//}
//

//========================================================================
//	Convert the chunk "name" to a string for output to the debug log
//========================================================================
void DebugDisplayChunkName(chDefs::Name i_ChunkName)
{
	char *name_str = (char*)(&i_ChunkName);
	DBG_LOG4("Chunk: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);
}

};
