/********************************************************************************************\
**  dynControlDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Support/dyn/dynControlDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace dynControlDataParser
{

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_CTPD = chDefs::MakeName('C', 'T', 'P', 'D');
	const chDefs::Name c_CTRD = chDefs::MakeName('C', 'T', 'R', 'D'); // legacy format

	//========================================================================
	//   ReadControlData_Legacy - parse CTRD chunk which had
	//	multiple types of control data.
	//========================================================================
	void ReadControlData_Legacy(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					dynControlData& o_Data )
	{

		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Node.Read(i_Reader);

		// don't need minAngle, maxAngle or axis
		float minAngle, maxAngle;
		chChunkParserUtil::Read(i_Reader, minAngle);
		chChunkParserUtil::Read(i_Reader, maxAngle);
		envType::Int32 axis;
		i_Reader.Read(axis);

		if (i_Version >= 1)
		{
			// don't need value
			float value;
			chChunkParserUtil::Read(i_Reader, value);

			if (i_Version >= 2)
			{
				// don't need control type anymore
				//	e_RotateControl = 0,
				//	e_TransformControl = 1
				envType::Int32 control_type;
				i_Reader.Read(control_type);

				// was vector3d, now 3 individual floats
				o_Data.m_RotateX.Read(i_Reader);
				o_Data.m_RotateY.Read(i_Reader);
				o_Data.m_RotateZ.Read(i_Reader);
				o_Data.m_Translation.Read(i_Reader);

				if (i_Version >= 3)
				{
					o_Data.m_Scale.Read(i_Reader);
				}
			}
		}
	}

	//========================================================================
	//   ReadControlData
	//========================================================================
	void ReadControlData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					dynControlData& o_Data )
	{

		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Node.Read(i_Reader);
		o_Data.m_RotateX.Read(i_Reader);
		o_Data.m_RotateY.Read(i_Reader);
		o_Data.m_RotateZ.Read(i_Reader);
		o_Data.m_Translation.Read(i_Reader);
		o_Data.m_Scale.Read(i_Reader);
	}

	//========================================================================
	//   WriteControlData
	//========================================================================
	void WriteControlData(	chWriter& o_Writer,
					const dynControlData& i_Data )
	{
		// version 4 switched to CTPD instead of CTRD
		// only supporting one type of control and using properties now.
		const int c_CTPD_Version = 4;
		o_Writer.WriteChunkHeader( c_CTPD, c_CTPD_Version, false );
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Node.Write(o_Writer);
		i_Data.m_RotateX.Write(o_Writer);
		i_Data.m_RotateY.Write(o_Writer);
		i_Data.m_RotateZ.Write(o_Writer);
		i_Data.m_Translation.Write(o_Writer);
		i_Data.m_Scale.Write(o_Writer);
		o_Writer.FinishChunk();
	}

}	// local namespace

//========================================================================
//   ReadControlData
//========================================================================
void ReadControlData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::vector<dynControlData>& o_Controls )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_CTPD)
		{
			// New chunk format with properties
			dynControlData data;
			ReadControlData(i_Reader, version, size, data);
			o_Controls.push_back(data);
		}
		else if (name == c_CTRD)
		{
			// Old file formats
			dynControlData data;
			ReadControlData_Legacy(i_Reader, version, size, data);
			o_Controls.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteControlData
//========================================================================
void WriteControlData(	chWriter& o_Writer,
					  const std::vector<dynControlData>& i_Controls )
{
	int num_controls = i_Controls.size();
	//DBG_LOG("Num controls writing: " << num_controls);
	for (int i=0; i<num_controls; i++)
	{
		// Write control info
		WriteControlData( o_Writer, i_Controls[i] );
	}
}




}	// end of namespace

