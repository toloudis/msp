/****************************************************************************\
**  sgpuAnimUtils.hpp
**
**      sgpuAnimUtils.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_ANIMUTILS_HPP
#define SGPU_ANIMUTILS_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"

#ifndef CH_BINWRITER_HPP
#include "Core/ch/chBinWriter.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
//#include "Core/dbg/dbgAssert.hpp"



class chWriter;
class maAxisBox;
class sgpuVector3;

extern const	chDefs::Name c_ACHR;
extern const	chDefs::Name c_VTXA;
extern  const chDefs::Name c_NNAM;
extern  const chDefs::Name c_FRAM;
extern  const chDefs::Name c_TIME;
extern  const chDefs::Name c_GVER;
extern  const chDefs::Name c_NVER;
extern  const chDefs::Name c_BBOX;
extern  const chDefs::Name c_AFPS;
extern  const chDefs::Name c_BGFR;
extern  const chDefs::Name c_EXPV;

extern const envType::UInt64 maxAnimationBudget;
void write_master_chunk_header( chWriter &io_Writer );

void write_master_chunk_finish( chWriter &io_Writer );

void write_fps( chWriter &io_Writer, float fps );

void write_begin_frame( chWriter &io_Writer, float beginFrame );

void write_bbox( chWriter &io_Writer, const maAxisBox &bbox );

void write_frame_for_mesh(
						  chWriter &io_Writer, 
						  float i_fFrameOffset, 
						  bool i_bSubdiv,  
						  int i_NumVerts, 
						  int i_NumNormals,
						  const sgpuVector3 *i_pPositions, 
						  const sgpuVector3 *i_pNormals  );
void write_exporter_version_stamp(chWriter &io_Writer);

bool check_validity_of_anim_params(
								   float i_FrameRate,
								   float	i_StartFrame,
								   float	i_EndFrame,
								   float	i_StepFrame);

#endif // #ifndef SGPU_ANIMUTILS_HPP