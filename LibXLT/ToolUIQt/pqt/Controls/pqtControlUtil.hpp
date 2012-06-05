/****************************************************************************\
**	pqtControlUtil.hpp
**
**		Utility for getting and setting values from multiple properties
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_CONTROLUTIL_HPP
#error pqtControlUtil.hpp multiply included
#endif
#define PQT_CONTROLUTIL_HPP

#ifndef PQT_CONTROL_HPP
#include "ToolUIQt/pqt/pqtControl.hpp"
#endif
#ifndef PQT_CONTROLMGR_HPP
#include "ToolUIQt/pqt/pqtControlMgr.hpp"
#endif
#ifndef PRTY_PROPERTYUIINFO_HPP
#include "Core/prty/prtyPropertyUIInfo.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif
#ifndef UNDO_MULTIPLEOPERATION_HPP
#include "Core/undo/undoMultipleOperation.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef GUI_STATUSBARMGR_HPP
#include "Tool/gui/guiStatusBarMgr.hpp"
#endif

#ifdef USE_QT
#include <QtGui/QWidget>


//============================================================================
//============================================================================
namespace pqtControlUtil
{
	//------------------------------------------------------------------------
	// Gets value from set of properties in UIInfo. Returns true
	//	if all properties have the same value.
	//------------------------------------------------------------------------
	template <class T, class P>
	bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, T& o_NewValue)
	{
		bool bSameValue = true;
		for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
		{
			P* pActualProperty = dynamic_cast<P*>(i_pUIInfo->GetProperty(i));
			DBG_ASSERT(pActualProperty != NULL, "Invalid property type, dynamic cast failed.");
			if (i==0)
				o_NewValue = pActualProperty->GetValue();
			else
				if (!(o_NewValue == pActualProperty->GetValue())) // color did not have != operator
					bSameValue = false;
		}
		return bSameValue;
	}	
	template <class T, class P>
	bool GetScaledCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, T& o_NewValue)
	{
		bool bSameValue = true;
		for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
		{
			P* pActualProperty = dynamic_cast<P*>(i_pUIInfo->GetProperty(i));
			DBG_ASSERT(pActualProperty != NULL, "Invalid property type, dynamic cast failed.");
			if (i==0)
				o_NewValue = pActualProperty->GetScaledValue();
			else
				if (!(o_NewValue == pActualProperty->GetScaledValue())) // color did not have != operator
					bSameValue = false;
		}
		return bSameValue;
	}

	//------------------------------------------------------------------------
	// Sets value into set of properties in pqtControl
	//------------------------------------------------------------------------
	template <class T, class P>
	bool SetCommonValue(pqtControl* io_pControl, T i_NewValue)
	{
		if (!io_pControl->ConfirmChange()) // Display Yes/No dialog if needed
			return false;

		guiStatusBarMgr::ClearErrorMessage();

		const int num_properties = io_pControl->GetNumberOfProperties();
		undoUndoOperation *pSubmitUndoOp = NULL;
		std::auto_ptr<undoMultipleOperation> pMultipleBlock;
		if (num_properties > 1)
			pMultipleBlock.reset( new undoMultipleOperation(io_pControl->GetName().c_str()) );

		for (int i=0; i < num_properties; ++i)
		{
			P* pActualProperty = static_cast<P*>(io_pControl->GetProperty(i));

			// Prepare an undo operation
			shared_ptr<prtyPropertyReference> prop_ref = io_pControl->CreatePropertyReference(i);
			if (prop_ref)
			{
				undoUndoOperation *pUndoOp = pActualProperty->CreateUndoOperation(prop_ref);
				if (pUndoOp)
				{
					if (pMultipleBlock.get())
						pMultipleBlock->AddOperation(pUndoOp);
					else
						pSubmitUndoOp = pUndoOp;
				}
			}

			const bool bSetDirty = true;
			pActualProperty->SetValue( i_NewValue, bSetDirty );	
		}

		// If some operations were added to the multiple block, then
		// use this operation as the one we submit. Otherwise,
		// let the auto_ptr delete it.
		if (pMultipleBlock.get() && (pMultipleBlock->GetNumOperations() > 0))
			pSubmitUndoOp = pMultipleBlock.release();
		
		// Submit the undo operation, if one was created
		SubmitOrContinueUndo(io_pControl, pSubmitUndoOp);
		return true;
	}	
	template <class T, class P>
	bool SetScaledCommonValue(pqtControl* io_pControl, T i_NewValue)
	{
		if (!io_pControl->ConfirmChange()) // Display Yes/No dialog if needed
			return false;

		guiStatusBarMgr::ClearErrorMessage();

		const int num_properties = io_pControl->GetNumberOfProperties();
		undoUndoOperation *pSubmitUndoOp = NULL;
		std::auto_ptr<undoMultipleOperation> pMultipleBlock;
		if (num_properties > 1)
			pMultipleBlock.reset( new undoMultipleOperation(io_pControl->GetName().c_str()) );

		for (int i=0; i < num_properties; ++i)
		{
			P* pActualProperty = static_cast<P*>(io_pControl->GetProperty(i));

			// Prepare an undo operation
			shared_ptr<prtyPropertyReference> prop_ref = io_pControl->CreatePropertyReference(i);
			if (prop_ref)
			{
				undoUndoOperation *pUndoOp = pActualProperty->CreateUndoOperation(prop_ref);
				if (pUndoOp)
				{
					if (pMultipleBlock.get())
						pMultipleBlock->AddOperation(pUndoOp);
					else
						pSubmitUndoOp = pUndoOp;
				}
			}
			const bool bSetDirty = true;
			pActualProperty->SetScaledValue( i_NewValue, bSetDirty );	
		}

		// If some operations were added to the multiple block, then
		// use this operation as the one we submit. Otherwise,
		// let the auto_ptr delete it.
		if (pMultipleBlock.get() && (pMultipleBlock->GetNumOperations() > 0))
			pSubmitUndoOp = pMultipleBlock.release();
		
		// Submit the undo operation, if one was created
		SubmitOrContinueUndo(io_pControl, pSubmitUndoOp);
		return true;
	}

	//------------------------------------------------------------------------
	// Create derivation of widget that notifies the control manager
	// when it is deleted by wxWidgets
	//------------------------------------------------------------------------
	template <class T>
	class CleanUpWidget : public T
	{
	public:
		CleanUpWidget(QWidget* i_pParent)
			: T(i_pParent) {}
#ifdef QT_FINISH_PORT
		CleanUpWidget(QWidget* i_pParent, 
					 QWidgetID i_Id)
			: T(i_pParent, i_Id) {}
#endif
		~CleanUpWidget()
		{
			pqtControlMgr::DeleteControl(this);
		}
	};

	//----------------------------------------------------------------------------
	// Checks if a new undo operation is needed, or if an old one can be
	//	continued. It will either submit the undo operation or delete it.
	//----------------------------------------------------------------------------
	void SubmitOrContinueUndo(pqtControl* i_pControl, undoUndoOperation *i_pOperation);
}

#endif // USE_QT
