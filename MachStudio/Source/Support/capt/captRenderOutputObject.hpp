/*****************************************************************************
**	captRenderOutputObject.hpp
**
**		This object contains all properties for the capture process
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/
#ifdef CAPT_RENDEROUTPUTOBJECT_HPP
#error captRenderOutputObject.hpp multiply included
#endif
#define CAPT_RENDEROUTPUTOBJECT_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif


//============================================================================
//============================================================================
class prtyProperty;
class prtyListBoxUIInfo;
class prtyComboBoxUIInfo;
class prtyPropertyUIInfo;
class prtyFileChooserUIInfo;
class prtyFolderChooserUIInfo;
class prtyButtonUIInfo;
class prtyCheckBoxUIInfo;
class prtyTextBoxUIInfo;
class prtyFloatEditUIInfo;
class prtyNumericUpDownUIInfo;

//============================================================================
//	typedefs + enums
//============================================================================
typedef std::vector<std::vector<std::string>>	category_list_type; // "category", "resolution"


//============================================================================
//============================================================================
class captRenderOutputObject : public prtyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderOutputObject(bool i_bIsLayerOutput = false, bool i_bIsMasterLayer = false);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderOutputObject( captRenderOutputData& i_Data, bool i_bIsLayerOutput = false, bool i_bIsMasterLayer = false );

	//------------------------------------------------------------------------
	//	called right before showing the dialog
	//------------------------------------------------------------------------
	void SetupControls( bool i_bBatchMode );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCameraList( captRenderOutputData& i_NewData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Read( captRenderOutputData& o_Data );
	void Read(const fsLocator& i_ConfigFile,
				captRenderOutputData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Write( const captRenderOutputData& i_Data );
	void Write( const fsLocator& i_ConfigFile,
				const captRenderOutputData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ConvertQuickTime();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderOutputObject* operator = (const captRenderOutputObject& i_CopyFrom);
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetToDefault(fsLocator& i_CurrentScene, 
					  captRenderOutputData& o_Data);
protected:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetConfigFilename(const char* i_ConfigFile, const char*i_DefaultConfigFile);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReadFromConfigFile(const fsLocator& i_ConfigFile,
							captRenderOutputData& o_Data );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteToConfigFile( const fsLocator& i_ConfigFile,
							const captRenderOutputData& i_Data );

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void RegisterProperties();

	//--------------------------------------------------------------------
	// Callbacks for when properties change, updates member data
	//--------------------------------------------------------------------
	void UpdateData(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void update_compresscodeUII( const std::string& i_CaptureType );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetDefaultCompressCode(const std::string& i_CaptureType);

	//--------------------------------------------------------------------
	// Callbacks for when properties change, updates member data
	//--------------------------------------------------------------------
	void AllCamerasChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void CaptureFormatChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void UseMarkerTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void RenderFrameRangeChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void RenderPosUseChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void RenderPosDeleteClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void ResolutionClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void ResolutionChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void SamplingChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void OutputFileCustomizeClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseSceneFilenameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseCameraNameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseLayerNameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseCompressionInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseRenderPassInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void CounterDigitsClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void OutputDirectoryCustomizeClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void OutputDirectoryRootChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void UseSceneFilenameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseCameraNameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseLayerNameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseRenderPassAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void UseResolutionAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty);

public:
	captRenderOutputData m_Data;

private:
	// Need to keep track of these UI Infos because we need to
	// adjust them after the constructor
	prtyListBoxUIInfo *m_pCamerasUIInfo;
	prtyComboBoxUIInfo *m_pCompressCodeUII;
	prtyPropertyUIInfo *m_pStartTimeUII;
	prtyPropertyUIInfo *m_pEndTimeUII;
	prtyPropertyUIInfo *m_pRenderPosSceneUII;
	prtyPropertyUIInfo *m_pRenderPosCameraUII;
	prtyPropertyUIInfo *m_pRenderPosLayerUII;
	prtyPropertyUIInfo *m_pRenderPosTimeUII;
	prtyFileChooserUIInfo *m_pRenderPosSaveFileUII;
	prtyButtonUIInfo *m_pRenderPosDeleteUII;
	prtyCheckBoxUIInfo *m_pRenderPosUseUII;
	prtyFloatEditUIInfo *m_pRenderWidthUII;
	prtyFloatEditUIInfo *m_pRenderHeightUII;
	prtyComboBoxUIInfo* m_pResolutions;
	prtyNumericUpDownUIInfo* m_pCaptureSampling;
	prtyCheckBoxUIInfo* m_bUseSceneFilenameAsDirectoryUI;
	prtyCheckBoxUIInfo* m_bUseCameraNameAsDirectoryUI;
	prtyCheckBoxUIInfo* m_bUseLayerNameAsDirectoryUI;
	prtyCheckBoxUIInfo* m_bUseRenderPassAsDirectoryUI;
	prtyCheckBoxUIInfo* m_bUseResolutionAsDirectoryUI;
	prtyFolderChooserUIInfo* m_OutputDirectoryRootUI;
	prtyTextBoxUIInfo* m_OutputDirectoryTagsUI;
	prtyTextBoxUIInfo* m_OutputFilenameTagsUI;

	bool m_bIsLayerOutput;
	bool m_bIsMasterLayer;
};
