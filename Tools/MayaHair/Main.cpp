

#include <iostream>
#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

#include "SgpuShaveExport.h"

#include "Graphics/mat/matShaderParser.hpp"


MStatus initializePlugin(MObject obj)
{ 
    MStatus   status;
    MFnPlugin plugin(obj, "Studio GPU Inc.", "1.0", "Any");

    status = plugin.registerCommand(
                "sgpuShaveExport",
                sgpuShaveExport::creator,
                sgpuShaveExport::createSyntax
            );

    if (!status) 
    {
        status.perror("registering sgpuShaveExport command");
        return status;
    }
	matShaderParser::Initialize();
    return status;
}


MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);
	matShaderParser::DeInitialize();
    status = plugin.deregisterCommand("sgpuShaveExport");

    if (!status)
    {
        status.perror("deregistering sgpuShaveExport command");
        return status;
    }

    return status;
}
