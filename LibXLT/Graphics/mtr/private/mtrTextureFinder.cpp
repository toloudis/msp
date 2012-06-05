/****************************************************************************\
**	mtrTextureFinder.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mtr/mtrTextureFinder.hpp"

#include "Core/Fs/fsFileEnum.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	//	FindFileEnum is used to find a file within subdirectories
	//============================================================================
	class FindFileEnum : public fsFileEnum::EnumTarget
	{
		itString m_Filename;
	public:
		fsLocator m_FullPath;
		bool m_bFound;

		// constructor takes vector to fill
		FindFileEnum(const itString& i_Filename)
			: m_Filename(i_Filename), m_bFound(false)
		{
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual bool Notify( const fsLocator& i_Directory, const fsLocator& i_File )
		{
			// Case-insensitive comparison
			if (itStringUtil::Equal(i_File.GetLastName(), m_Filename))
			{
				m_FullPath = i_Directory;
				m_FullPath.Push( i_File );
				m_bFound = true;
				return false; // can stop the enumeration now
			}

			return true;
		}
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool search_material_library(const itString& i_FileName, 
								 fsLocator& o_FoundLocator)
	{
		// See if we have a material library path
		fsLocator material_root;
		if (gfPaths::GetNumPathsInList(gfPaths::e_MaterialLibrary) > 0)
		{
			fsLocator material_root = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
			bool bUseMaterialLibrary = (material_root.GetNumNames() > 0) ;
			if (bUseMaterialLibrary)
			{
				FindFileEnum find_target(i_FileName);
				const bool bSearchSubdirs = true;
				fsFileEnum::EnumerateFiles(material_root, find_target, bSearchSubdirs);

				if (find_target.m_bFound)
				{
					o_FoundLocator = find_target.m_FullPath;
					return true;
				}
			}
		}
		return false;
	}
}

//----------------------------------------------------------------
// A constructor is provided for the 2-finder join which
//	is the most likely use of this class.
//----------------------------------------------------------------
mtrTextureFinder::mtrTextureFinder()
	: m_pFinder(NULL)
{
}
mtrTextureFinder::mtrTextureFinder(const fsResourceFinder& i_Finder)
	: m_pFinder(&i_Finder)
{
}
mtrTextureFinder::mtrTextureFinder(const mtrTextureFinder& i_Finder)
	: m_pFinder(i_Finder.m_pFinder)
{
}

//----------------------------------------------------------------
//	Trying to find resource i_FileName.  If this finder
//	knows where it is, it should return true and
//	put the full path to the resource (including filename)
//	into o_FoundLocator.
//----------------------------------------------------------------
bool mtrTextureFinder::FindResource(const itString& i_FileName, 
									fsLocator& o_FoundLocator) const
{
	if (m_pFinder->FindResource(i_FileName, o_FoundLocator))
		return true;

	return search_material_library(i_FileName, o_FoundLocator);
}

//----------------------------------------------------------------
// This function works like the other FindResource, but this
//	one provides a locator of the resource that is asking
//	for the file to be found, which may be important for
//	some implementations.
//----------------------------------------------------------------
bool mtrTextureFinder::FindResource(const itString& i_FileName, 
							const fsLocator& i_AskingLocator,
							fsLocator& o_FoundLocator) const
{
	if (m_pFinder->FindResource(i_FileName, o_FoundLocator))
		return true;

	return search_material_library(i_FileName, o_FoundLocator);
}

