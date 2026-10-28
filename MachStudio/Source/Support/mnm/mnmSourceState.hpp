/*****************************************************************************
**  mnmSourceState.hpp
**
**      mnmSourceState is the read-only "Source" line on the property page
**	of an object brought in from a scene file: whether it still follows
**	the file (a reference) or has been edited (a copy), and the file.
**	Features/Import (imptSceneRefs) sets the text; it isn't part of the
**	object's data and isn't saved.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MNM_SOURCESTATE_HPP
#error mnmSourceState.hpp multiply included
#endif
#define MNM_SOURCESTATE_HPP

#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class prtyPropertyUIInfo;


//============================================================================
//============================================================================
class mnmSourceState
{
public:
	//--------------------------------------------------------------------
	// The property is named "Source" and starts empty
	//--------------------------------------------------------------------
	mnmSourceState();
	virtual ~mnmSourceState();

	//--------------------------------------------------------------------
	// Show i_Text (no undo, doesn't change the document)
	//--------------------------------------------------------------------
	void SetSourceState(const std::string& i_Text);

protected:
	//--------------------------------------------------------------------
	// The read-only text box an object registers for it (AddProperty)
	//--------------------------------------------------------------------
	prtyPropertyUIInfo* CreateSourceStateUIInfo();

	prtyText m_SourceState;
};
