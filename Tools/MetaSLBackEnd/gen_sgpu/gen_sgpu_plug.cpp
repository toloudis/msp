/******************************************************************************
 * Copyright 1986-2009 by mental images GmbH, Fasanenstr. 81, D-10623 Berlin,
 * Germany. All rights reserved.
 ******************************************************************************/
#include "gen_sgpu_plug.h"

#include "mslCodeGenerator.hpp"


/*! The pluging context is given in the init call and allows to get the mod_xxx
 *  variables as well as having access to the main programs allocator.
 */
static MI::PLUG::Plugin_context *plugin_context;

namespace MI { 

namespace GEN_MSL {

using namespace MSDK;

mslCodeGenerator_plugin::mslCodeGenerator_plugin()
    : m_code_generator(new mslCodeGenerator())
{}

mslCodeGenerator_plugin::~mslCodeGenerator_plugin()
{
    delete m_code_generator;
}

const char *mslCodeGenerator_plugin::get_name() const
{
    return "gen_sgpu";
}

const char *mslCodeGenerator_plugin::get_type() const
{
    return "Code_generator12_plugin";
}

int mslCodeGenerator_plugin::get_version() const
{
    return 2;
}

const char *mslCodeGenerator_plugin::get_compiler() const
{
    return "";
}

void mslCodeGenerator_plugin::release()
{
    delete this;
}

const char* mslCodeGenerator_plugin::get_language_name()
{
    return "sgpu";
}

ICode_generator12_plugin::Code_type mslCodeGenerator_plugin::get_code_type()
{
	//bga - which one should we be using? Does it matter? If I use HARWARE,
	// it generates two shaders for some reason.
    return ICode_generator12_plugin::SOFTWARE;
    //return ICode_generator12_plugin::HARDWARE;
}

ICode_generator *mslCodeGenerator_plugin::get_code_generator()
{
    return m_code_generator;
}

}

}

extern "C"
{

#if !(defined(WIN32) || defined(WIN64))
#  if defined(__GNUC__) && !defined(__ICC)
//#    define DLLEXPORT __attribute__ ((visibility("default")))
#    define DLLEXPORT __attribute__ ((visibility("protected")))
#    define DLLLOCAL  __attribute__ ((visibility("hidden")))
#  else
#    define DLLEXPORT
#    define DLLLOCAL
#  endif
#else
#  define DLLEXPORT __declspec ( dllexport )
#  define DLLLOCAL
#endif

//! The only external function needed by the plugin system.
/*! Init the library if this is a dynamic library.
 *  This library supports one plugin, the mslCodeGenerator_plugin.
 *  \param  index       The index of the plugin to be returned.
 *  \param  context     Not used.
 *  \returns            The plugin.
 */
DLLEXPORT MI::PLUG::Plugin *initializer(
    unsigned int index,
    MI::PLUG::Plugin_context *context)
{
    plugin_context = context;
    if(index)
	return 0;
    return new MI::GEN_MSL::mslCodeGenerator_plugin();
}

}
