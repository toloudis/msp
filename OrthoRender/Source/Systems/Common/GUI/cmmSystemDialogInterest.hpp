/****************************************************************************\
**	cmmSystemDialogInterest.hpp
**
**		An interest related to the SystemDialog.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SYSTEMDIALOGINTEREST_HPP
#error cmmSystemDialogInterest.hpp multiply included
#endif
#define CMM_SYSTEMDIALOGINTEREST_HPP


//============================================================================
//============================================================================
class cmmSystemDialogInterest
{
	public:
		//--------------------------------------------------------------------
		//	SceneDialogOpen - perform tasks (like adding tabs) relating
		//	to the scene/system dialog opening.  These tasks happen each time
		//	the scene dialog is launched.
		//--------------------------------------------------------------------
		virtual void SceneDialogOpen() = 0;

		//--------------------------------------------------------------------
		//	SceneDialogClose - perform tasks (like adding tabs) relating
		//	to the scene/system dialog closing.  These tasks happen each time
		//	the scene dialog is closed.
		//--------------------------------------------------------------------
		virtual void SceneDialogClose() = 0;
};

