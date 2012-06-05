/*****************************************************************************
**	mslQualifiedName.hpp
**
**  mslQualifiedName represents (potentially) qualified names.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_QUALIFIEDNAME_HPP
#error mslQualifiedName.hpp multiply included
#endif
#define MSL_QUALIFIEDNAME_HPP

#include <vector>
#include <string>

//============================================================================
//============================================================================
class mslQualifiedName {

private:

    //! The string representation of the name.
    std::string m_name;

    //! The type of the component vector.
    typedef std::vector<std::string> String_vector;

    //! The name components.
    String_vector m_components;

public:

    //! Constructor.
    mslQualifiedName()
    {
    }

    //! Constructor.
    /*!
     *  \param  name    The first component of the name.
     */
    mslQualifiedName(const char *name)
        : m_name(name)
    {
        std::string name_str = name;
        std::string::size_type s;
        
        while((s = name_str.find("::")) != std::string::npos) {
            m_components.push_back(name_str.substr(0,s));
            name_str = name_str.substr(s+2);
        }
        m_components.push_back(name_str);
    }

    //! Destructor.
    virtual ~mslQualifiedName()
    {
    }

    //! Append a compnent to the qualified name.
    /*!
     *  \param  component   The component to append.
     */
    virtual void append(const char *component)
    {
        if(m_components.size())
            m_name += "::";
        m_name += component;
        m_components.push_back(component);
    }

    //! Get the string representation of the qualified name.
    /*!
     *  \returns    The string representation of the qualified name.
     */
    const char *c_str() const
    {
        return m_name.c_str();
    }

    //! Get the number of components.
    /*!
     *  \returns    The number of components of the name.
     */
    int size() const
    {
        return (int)m_components.size();
    }

    //! Get a component of the name.
    /*!
     *  \param  index   The index of the component.
     *  \returns        The name component.
     */
    const char *operator[](int index) const
    {
        return m_components[index].c_str();
    }

};
