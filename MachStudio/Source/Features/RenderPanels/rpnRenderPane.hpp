/*****************************************************************************
**  rpnRenderPane.hpp
**
**     Managed class for a single pane when using multiple views
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef RPN_RENDERPANE_HPP
#error rpnRenderPane.hpp multiply included
#endif
#define RPN_RENDERPANE_HPP
#ifndef RPN_PANELVIEWER_HPP
#include "Features/RenderPanels/rpnPanelViewer.hpp"
#endif
#ifndef RPN_OPERATIONS_HPP
#include "Features/RenderPanels/rpnOperations.hpp"
#endif
#ifndef CAMS_CAMERAMGR_HPP
#include "Support/cams/camsCameraMgr.hpp"
#endif
#ifndef CAMS_DIRECTORSCUTMGR_HPP
#include "Support/cams/camsDirectorsCutMgr.hpp"
#endif
#ifndef CAM3D_MGR_HPP
#include "Tool/cam3d/cam3dMgr.hpp"
#endif
#ifndef FGT_FRAMEMGR_HPP
#include "Features/FilmGates/fgtFrameMgr.hpp"
#endif
#ifndef G3D_VIEWER_HPP
#include "Graphics/g3d/g3dViewer.hpp"
#endif
#ifndef IN_DEVICEMGR_HPP
#include "Input/in/inDeviceMgr.hpp"
#endif
#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif
#ifndef GUI_STATUSBARMGR_HPP
#include "Tool/gui/guiStatusBarMgr.hpp"
#endif
#ifndef MUI_TIMECODEMGR_HPP
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef TMA3D_RENDERVIEW_HPP
#include "Tool/tma3d/tma3dRenderView.hpp"
#endif
#ifndef TMA3D_SCREENUTIL_HPP
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#endif
#ifndef TMA3D_CURSORMGR_HPP
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#endif
