/****************************************************************************\
**  sgpuPathReference.hpp
**
**      sgpuPathReference.hpp defines class for a indexed triangle mesh. A single
**	set of indices maps into vertex information which contains position,
**	normal and texture coordinates.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_PATHREFERENCE_HPP
#define SGPU_PATHREFERENCE_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuNodeContent.hpp"



//============================================================================
//============================================================================
struct sgpuPathReferenceImpl;
class sgpuMaterial;
class sgpuNode;
class sgpuMeshConstructor;
class  sgpuString;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuPathReference : public sgpuNodeContent
{
public:
	sgpuPathReference( const sgpuPathReference &i_Other );
	sgpuPathReference& operator= ( const sgpuPathReference & i_Other );
	~sgpuPathReference();
	//push the component to the back of the path
	void PushPathComponent( const sgpuString &i_PathComponent );
	//will  raise sgpuException, if the path is empty
	sgpuString PopPathComponent();
	int GetNumPathComponents()const;
	sgpuString GetPathComponent( int i_nComponent )const;
	bool operator== ( const sgpuPathReference &other ) const;
	friend sgpuNode;
private:
	sgpuPathReference();
	sgpuPathReferenceImpl *m_pImpl;

};

#endif // #ifndef SGPU_MESH_HPP