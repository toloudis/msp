/****************************************************************************\
**	chnlChannelClip.hpp
**
**		Data class for a clip to display in a channel control
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_CHANNELCLIP_HPP
#error chnlChannelClip.hpp multiply included
#endif
#define CHNL_CHANNELCLIP_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

#include <string>

//============================================================================
//============================================================================
class chnlChannelClip 
{
	public:
		//--------------------------------------------------------------------
		// Callback type for being notified when a clip is changed
		//--------------------------------------------------------------------
		class ChangedCallback
		{
		public:
			virtual void ClipChanged(chnlChannelClip*) = 0;
		};

		//--------------------------------------------------------------------
		// enumeration of blend types
		//--------------------------------------------------------------------
		enum BlendType
		{
			e_NoBlend = 0,	// immediate switch to this driver as of begin time
			e_Previous,		// blend in from the end of the prvious driver
			e_BlendTime,	// blend in a fixed amount of time determined by "BlendTime"
			e_Overwrite,		// this driver takes over as soon as no other driver is responsible
			e_SmoothBlend		// driver uses spline tangents to ease in
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlChannelClip();
		chnlChannelClip(const std::string& i_Name, 
						float i_BeginTime, 
						float i_EndTime, 
						BlendType i_Blend = e_NoBlend, 
						float i_BlendTime = 0.0f, 
						bool i_Restore = false);
		chnlChannelClip(const chnlChannelClip& i_Clip);

		//--------------------------------------------------------------------
		// Set callback pointer for when clip changes
		//--------------------------------------------------------------------
		void SetChangedCallback(ChangedCallback* i_pCallback);

		//--------------------------------------------------------------------
		// Name
		//--------------------------------------------------------------------
		void SetName(const std::string& i_Name);
		const std::string& GetName() const;

		//--------------------------------------------------------------------
		// Category
		//--------------------------------------------------------------------
		void SetCategory(const std::string& i_Category);
		const std::string& GetCategory() const;

		//--------------------------------------------------------------------
		// BeginTime
		//--------------------------------------------------------------------
		void SetBeginTime(float i_BeginTime);
		float GetBeginTime() const;

		//--------------------------------------------------------------------
		// EndTime
		//--------------------------------------------------------------------
		void SetEndTime(float i_EndTime);
		float GetEndTime() const;

		//--------------------------------------------------------------------
		// Duration - end time minus begin time
		//--------------------------------------------------------------------
		float GetDuration() const;

		//--------------------------------------------------------------------
		// Blend
		//--------------------------------------------------------------------
		void SetBlendType(BlendType i_Blend);
		BlendType GetBlendType() const;

		//--------------------------------------------------------------------
		// BlendTime
		//--------------------------------------------------------------------
		void SetBlendTime(float i_BlendTime);
		float GetBlendTime() const;

		//--------------------------------------------------------------------
		// Restore
		//--------------------------------------------------------------------
		//void SetRestore(bool i_Restore);
		bool GetRestore() const;

		//--------------------------------------------------------------------
		// FillColor - return either standard of highlight fill color,
		//	depending on settings
		//--------------------------------------------------------------------
		wxColour GetFillColor() const;

		//--------------------------------------------------------------------
		// UseHighlightFill - use either standard of highlight fill color
		//--------------------------------------------------------------------
		void SetUseHighlightFill(bool i_bUseHighlight);
		bool GetUseHighlightFill()const;

		//--------------------------------------------------------------------
		// StandardFillColor
		//--------------------------------------------------------------------
		void SetStandardFillColor(const wxColour& i_Color);
		const wxColour& GetStandardFillColor() const;

		//--------------------------------------------------------------------
		// HighlightFillColor
		//--------------------------------------------------------------------
		void SetHighlightFillColor(const wxColour& i_Color);
		const wxColour& GetHighlightFillColor() const;

		//--------------------------------------------------------------------
		// Interaction values - potential new position for the clip
		//	while interacting with the mouse
		//--------------------------------------------------------------------
		void SetInteraction(float i_InteractStart, float i_InteractDuration);
		float GetInteractStart() const;
		float GetInteractDuration() const;
		float GetInteractEnd() const;
		int GetInteractRoot() const;

		//--------------------------------------------------------------------
		// While moving the clips, the begin and end times may be snapped
		//	to important times. The snapping is always done by altering the
		//	InteractStart value, but you can align either the begin or end.
		//--------------------------------------------------------------------
		void AlignInteractStart(float i_Time);
		void AlignInteractEnd(float i_Time);

	//============================================================================
	//	Virtual functions to be overriden
	//============================================================================

		//--------------------------------------------------------------------
		// Set begin and end times using duration
		//--------------------------------------------------------------------
		virtual void SetTime(float i_BeginTime, float i_Duration);

		//--------------------------------------------------------------------
		// Set blend attributes
		//--------------------------------------------------------------------
		virtual void SetBlend(BlendType i_Blend, float i_BlendTime);

		//--------------------------------------------------------------------
		// Set standard filll color
		//--------------------------------------------------------------------
		virtual void SetFillColor( float i_R, float i_G, float i_B );

		//--------------------------------------------------------------------
		// Set the restore flag
		//--------------------------------------------------------------------
		virtual void SetRestore(bool i_bRestore);

		//--------------------------------------------------------------------
		// display properties dialog for this clip
		//--------------------------------------------------------------------
		virtual void ShowProperties();

		//--------------------------------------------------------------------
		// allow driver to handle selection in its own way
		//--------------------------------------------------------------------
		virtual void Select();

		//--------------------------------------------------------------------
		// select the 3D icon for this clip
		//--------------------------------------------------------------------
		virtual void SelectIcon();

		//--------------------------------------------------------------------
		/// Return string to display when mouse hovers over control
		//--------------------------------------------------------------------
		virtual std::string GetHoverDescription();

		//--------------------------------------------------------------------
		/// Return string to display when mouse is interacting 
		//		(moving, resizing) over control
		//--------------------------------------------------------------------
		virtual std::string GetInteractionDescription(float i_InteractStart, float i_InteractDuration);

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void notify();

		std::string m_Name;
		std::string m_Category;
		float m_BeginTime;
		float m_EndTime;
		BlendType m_Blend;
		float m_BlendTime;
		bool m_bRestore;
		bool m_bUseHighlightFill;
		wxColour m_StandardFillColor;
		wxColour m_HighlightFillColor;
		float m_InteractStart;
		float m_InteractDuration;
		int m_InteractRoot;
		ChangedCallback* m_pChangedCallback;
};

#endif // USE_WXWIDGETS
