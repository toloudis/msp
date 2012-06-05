/*****************************************************************************
**	mslGeneratedShader.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslGeneratedShader.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <stdarg.h>

namespace
{
}

mslGeneratedShader::mslGeneratedShader()
    : m_ref_count(1)
{}

mslGeneratedShader::~mslGeneratedShader() {}

void mslGeneratedShader::reference()
{
    ++m_ref_count;
}

void mslGeneratedShader::release()
{
    if(!--m_ref_count)
        delete this;
}

Interface *mslGeneratedShader::get_interface(int interface_id)
{
    return 0;
}

Uint64 mslGeneratedShader::get_interface_target() const
{
    return reinterpret_cast<Uint64>(m_code.c_str());
}

const char *mslGeneratedShader::get_language() const
{
    return "msl";
}

int mslGeneratedShader::get_source_code_count() const
{
    return 1;
}

const char *mslGeneratedShader::get_source_code_ext(int index) const
{
    return "msl";
}

int mslGeneratedShader::get_source_code_size(int index) const
{
    return index ? 0 : int(m_code.size());
}

void mslGeneratedShader::get_source_code(
    int index,char *out_buffer,int buffer_size) const
{
    switch(index) {
    case 0:
        ::strncpy(out_buffer,m_code.c_str(),buffer_size);
        break;
    default:
        /* skip */;
    }
}

void mslGeneratedShader::print_code(const char *code,va_list args)
{
    char buffer[2048];
    ::vsprintf(buffer,code,args);
    buffer[2047] = '\0';
    m_code += buffer;
}

void mslGeneratedShader::print_code(const char *code,...)
{
    va_list args;
    va_start(args,code);
    print_code(code,args);
    va_end(args);
}

void mslGeneratedShader::append_string(const std::string& i_String)
{
    m_code += i_String;
}

void mslGeneratedShader::print_tabs(int tabs)
{
    for(int i = 0; i < tabs; i++)
        print_code("\t");
}
