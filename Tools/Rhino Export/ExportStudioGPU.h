#ifndef _EXPORTSTUDIOGPU_H_
#define _EXPORTSTUDIOGPU_H_


bool
ExportStudioGPU(CRhinoDoc& doc, ON_wString filename, bool onlySelected, bool isInteractive, const wchar_t* pluginName, bool& isCancelled);


#endif