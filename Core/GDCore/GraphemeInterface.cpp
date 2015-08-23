#include "GDCore/GraphemeInterface.h"

namespace gd
{

ConstGraphemeInterface::ConstGraphemeInterface(const String &str) : m_str(str)
{

}

ConstGraphemeInterface::const_iterator ConstGraphemeInterface::begin() const
{
    return const_iterator(m_str.begin(), m_str.begin(), m_str.end());
}

ConstGraphemeInterface::const_iterator ConstGraphemeInterface::end() const
{
    return const_iterator(m_str.end(), m_str.begin(), m_str.end());
}

String::size_type ConstGraphemeInterface::size() const
{
    return std::distance(begin(), end());
}

String::size_type ConstGraphemeInterface::CodepointPosFromGraphemePos( size_type graphemePos ) const
{
    auto it = begin();
    auto beginIt = begin();
    auto endIt = end();

    while(it != endIt && graphemePos > 0)
    {
        --graphemePos;
        ++it;
    }
    if(graphemePos > 0)
        return gd::String::npos;

    return std::distance(beginIt.base(), it.base());
}

ConstGraphemeInterface::size_type ConstGraphemeInterface::GraphemePosFromCodepointPos( String::size_type codepointPos) const
{
    auto itCp = m_str.begin();
    while(itCp != m_str.end() && codepointPos > 0)
    {
        --codepointPos;
        ++itCp;
    }
    if(codepointPos > 0)
        return -1;

    //We count manually the grapheme count because itCp might be in the middle of
    //a grapheme
    auto itGr = begin();
    size_type graphemePos = 0;
    for(; itGr.base() < itCp; ++itGr, ++graphemePos);

    return graphemePos;
}

ConstGraphemeInterface::value_type ConstGraphemeInterface::operator[](const size_type position) const
{
    auto it = begin();
    std::advance(it, position);

    return *it;
}

GraphemeInterface::GraphemeInterface(String &str) : ConstGraphemeInterface(str), m_str(str)
{

}

GraphemeInterface::iterator GraphemeInterface::begin()
{
    return iterator(m_str.begin(), m_str.begin(), m_str.end());
}

GraphemeInterface::iterator GraphemeInterface::end()
{
    return iterator(m_str.end(), m_str.begin(), m_str.end());
}

String& GraphemeInterface::replace( iterator i1, iterator i2, const String &str )
{
    m_str.replace(i1.base(), i2.base(), str);

    return m_str;
}

String& GraphemeInterface::replace( size_type pos, size_type len, const String &str )
{
    auto it = begin();
    for(; it != end() && pos > 0; ++it, --pos);
    if(pos > 0)
        throw std::out_of_range("[gd::String::Grph().replace] starting pos greater than size");

    auto it2 = it;
    for(; it2 != end() && len > 0; ++it2, --len);

    m_str.replace(it.base(), it2.base(), str);

    return m_str;
}

}
