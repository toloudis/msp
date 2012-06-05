/*****************************************************************************
**	fcuiUtils.hpp
**
**		handles helper functions to separate app functionality from the UI.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_UTILS_HPP
#error fcuiUtils.hpp multiply defined
#endif
#define FCUI_UTILS_HPP

#include <vector>
#include <QtGui/QWidget>
#include <vector>


//============================================================================
// Forward References
//============================================================================
class QtFC3D;
class fsLocator;
class nameString;
class itString;
class tmlnScriptObject;
class tmlnDriver;
class matTexture;
class maTime;

//============================================================================
//============================================================================
namespace fcuiUtils
{
	//------------------------------------------------------------------------
	///	Exit the application
	//------------------------------------------------------------------------
	void ExitApplication();

	//------------------------------------------------------------------------
	///	Ask the user to save the project (.mab)
	//------------------------------------------------------------------------
	void UserSaveProject(QWidget* i_pParent);

	//------------------------------------------------------------------------
	///	Ask the user to load the project (.mab)
	//------------------------------------------------------------------------
	void LaunchMovie(fsLocator& i_projectpath);
	void UserLoadProject();

	//------------------------------------------------------------------------
	///	make (or "capture") the project to a movie file
	//------------------------------------------------------------------------
	void MakeProject();

	//------------------------------------------------------------------------
	///	Abort Render
	//------------------------------------------------------------------------
	void AbortMake();

	//------------------------------------------------------------------------
	/// reload object based on the category name
	//------------------------------------------------------------------------
	void ReloadObject(const itString& i_CurrentCategoryName, fsLocator& io_ObjectName);

	//------------------------------------------------------------------------
	/// given a path to a gxb file, load that file to the scene
	//------------------------------------------------------------------------
	void LoadObject(const fsLocator& i_Path);

	//------------------------------------------------------------------------
	/// load a video to a billboard
	//------------------------------------------------------------------------
	void LoadVideo(const itString& i_CurrentCategoryName, const fsLocator& i_VideoFilePath);

	//------------------------------------------------------------------------
	/// load a texture to an object
	///
	///	if a thumbnail file is passed it, this function will generate it.
	//------------------------------------------------------------------------
	void LoadTexture(const itString& i_CurrentCategoryName, const fsLocator& i_TextureFilePath);
	void LoadTexture(const itString& i_CurrentCategoryName, const fsLocator& i_TextureFilePath, const fsLocator& i_Thumbnail);

	//------------------------------------------------------------------------
	/// load a material to an object
	//------------------------------------------------------------------------
	void LoadMaterial(const itString& i_ObjectName, const itString& i_MaterialName, const fsLocator& i_MaterialFilePath);

	//------------------------------------------------------------------------
	/// load a gmb to an object
	//------------------------------------------------------------------------
	void LoadMaterials(const itString& i_ObjectName, const fsLocator& i_MaterialFilePath);

	//------------------------------------------------------------------------	
	/// Get script object by name
	//------------------------------------------------------------------------
	tmlnScriptObject* GetTimelineScriptObject(const nameString& i_ObjectName);

	//----------------------------------------------------------------------------
	/// create a capture driver for the given camera for the specified time
	//----------------------------------------------------------------------------
	void CreateCameraCaptureDriver( const nameString& i_CameraName,
									const maTime& i_StartTime, const maTime& i_EndTime );

	
	//----------------------------------------------------------------------------
	/// Extend the capture drivers for the director cam to the new end time
	//----------------------------------------------------------------------------
	void ExtendDirectorDriver(const maTime& i_EndTime);

	//----------------------------------------------------------------------------
	/// create a driver for the given object for the specified time
	//----------------------------------------------------------------------------
	tmlnDriver* CreateDriver( const std::string& i_driverName,
							  const nameString& i_ObjectName,
							  const maTime& i_StartTime, const maTime& i_EndTime );

	//----------------------------------------------------------------------------
	/// Return the number of drivers for the object
	//----------------------------------------------------------------------------
	int GetNumDrivers( const nameString& i_ObjectName );

	//----------------------------------------------------------------------------
	/// Get a unique texture driver name for the title card
	//----------------------------------------------------------------------------
	void GetUniqueDriverName( const nameString& i_ObjectName, 
							  const itString& i_ProposedName,
							  itString& o_UniqueName);

	//----------------------------------------------------------------------------
	/// Create the texture driver and set its property values
	//----------------------------------------------------------------------------
	void CreateTitlecardTextureDriver( const nameString& i_ObjectName, 
									   const fsLocator& i_TextureFile, 
									   const maTime& i_StartTime, const maTime& i_EndTime,
									   itString& o_TextureName);

	///---------------------------------------------------------------------------
	/// Assign a new texture file to the title card driver
	///---------------------------------------------------------------------------
	void ReloadTitlecardTextureValue( const nameString& i_ObjectName,
									  const itString& i_DriverName,
									  const fsLocator& i_NewTextureFile );

	///---------------------------------------------------------------------------
	/// check if the billboard is missing the texture
	///---------------------------------------------------------------------------
	bool CheckforMissingFile( const nameString& i_ObjectName,
		   					  const itString& i_DriverName);

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	const maTime SetObjectAnims( const itString& i_AnimationName, 
								 const fsLocator& i_CurrentAnimFile, 
								 const maTime& i_StartTime );

	//------------------------------------------------------------------------
	/// Create the animation driver and set its property values
	//------------------------------------------------------------------------
	void CreateObjectAnimationDriver( const nameString& i_ObjectName, 
									  const fsLocator& i_AnimationFile, 
									  const maTime& i_StartTime, maTime& o_EndTime, 
									  bool i_bLooping = false );

	//----------------------------------------------------------------------------
	/// When a cast member's item is changed, we need to change the animation 
	/// drivers to point to the proper item animation files
	//----------------------------------------------------------------------------
	void UpdateAnimationDrivers( const nameString& i_ObjectName,
								 const itString& i_NewObjectName );
	
	//----------------------------------------------------------------------------
	/// Shift a driver from its old start time to its new start time
	//----------------------------------------------------------------------------
	void ShiftDriverToTime( const nameString& i_ObjectName,
							const std::string& i_DriverName,
  						    const maTime& i_OldStartTime, 
						    const maTime& i_NewStartTime);

	void ShiftDriverToTimeAtPosX( const nameString& i_ObjectName,
								  const std::string& i_DriverName,
								  const maTime& i_OldStartTime, 
								  const maTime& i_NewStartTime,
								  float i_PositionX);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	tmlnDriver* GetDriverAtTime( const nameString& i_ObjectName,
								 const std::string& i_DriverName,
								 const maTime& i_StartTime );

	//----------------------------------------------------------------------------
	/// Get all drivers for the object within the given time
	//----------------------------------------------------------------------------
	void GetDrivers( const nameString& i_ObjectName,
					 std::vector<tmlnDriver*>& o_Drivers );

	//----------------------------------------------------------------------------
	/// Shift all drivers in a list to the new time
	//----------------------------------------------------------------------------
	void ShiftDriversToTime( const nameString& i_ObjectName,
							 const maTime& i_StartOffset,
							 const std::vector<tmlnDriver*>& i_Drivers );

	//----------------------------------------------------------------------------
	/// Remove a driver at the start time
	//----------------------------------------------------------------------------
	void RemoveDriverAtTime( const nameString& i_ObjectName,
							 const std::string& i_DriverName,
							 const maTime& i_StartTime);

	//------------------------------------------------------------------------
	///	Play a movie file();
	//------------------------------------------------------------------------
	void PlayMovie(QWidget* i_pParent);

	//------------------------------------------------------------------------
	///Set the name of the movie to be played in theater
	//------------------------------------------------------------------------
	void SetMovieName(const fsLocator& i_MovieName);
	
	//------------------------------------------------------------------------
	///	Get the name of the movie to be played in theater
	//------------------------------------------------------------------------
	fsLocator GetMovieName();

	//------------------------------------------------------------------------
	///Set the title of the movie to be played in theater
	//------------------------------------------------------------------------
	void SetMovieTitle(const fsLocator& i_MovieTitle);
	fsLocator GetMovieTitle();
	
	//------------------------------------------------------------------------
	///	Get/Set the checked state for the append countdown CB
	//------------------------------------------------------------------------
	void SetChecked(bool i_Checkedstate);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool GetChecked();

	//------------------------------------------------------------------------
	//	Strip the beginning number from a text string.
	//
	//	e.g. 01.Wardrobe -> Wardrobe
	//------------------------------------------------------------------------
	void StripNumber(itString &io_FileList);

	//------------------------------------------------------------------------
	//	Sort file list with preferences to GXBs
	//------------------------------------------------------------------------
	void SortFileList_GXB(std::vector<fsLocator> &io_FileList);

	//------------------------------------------------------------------------
	//	Sort file list with preferences to GXBs
	//------------------------------------------------------------------------
	//void SortDataList(cmmDialogDataList &io_DataList);

	//------------------------------------------------------------------------
	//	Force the projected lights to not show their icons (on by default)
	//------------------------------------------------------------------------
	void ProjectedLight_HideIcons();

	//------------------------------------------------------------------------
	//	Generate a thumbnail from a matTexture
	//------------------------------------------------------------------------
	void CreateUserThumbnail( matTexture* i_pTexture, const fsLocator& i_DestinationFile );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetWidescreenHeight( int i_Width );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetWidescreenWidth( int i_Height );

	//------------------------------------------------------------------------
	/// Copy the data from the base camera to the copy camera
	///-----------------------------------------------------------------------
	void CopyCamProperties(const std::string& i_CopyCamString, const std::string& i_BaseCamString);

	//-----------------------------------------------------------------------
	/// Get/Set Object name to be used to set audio driver
	//-----------------------------------------------------------------------
	void SetObjectName(const itString& i_CastmemberName);
	itString GetObjectName();

	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void SetCountdownMoviepath(const fsLocator& i_CountdownMoviePath);
	fsLocator GetCountdownMoviePath();

}	//end namespace fcuiUtils