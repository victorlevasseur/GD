#include "GDCore/GraphemeTools.h"

namespace gd
{

GraphemeTools::const_iterator GraphemeTools::begin( const gd::String &str )
{
    return const_iterator(str.begin(), str.begin(), str.end());
}

GraphemeTools::const_iterator GraphemeTools::end( const gd::String &str )
{
    return const_iterator(str.end(), str.begin(), str.end());
}

GraphemeTools::iterator GraphemeTools::begin( gd::String &str )
{
    return iterator(str.begin(), str.begin(), str.end());
}

GraphemeTools::iterator GraphemeTools::end( gd::String &str )
{
    return iterator(str.end(), str.begin(), str.end());
}
GraphemeTools::size_type GraphemeTools::size( const gd::String &str )
{
    return std::distance(begin(str), end(str));
}

GraphemeTools::size_type GraphemeTools::CodepointPosFromGraphemePos( const gd::String &str, size_type graphemePos )
{
    auto it = begin(str);
    auto beginIt = begin(str);
    auto endIt = end(str);

    while(it != endIt && graphemePos > 0)
    {
        --graphemePos;
        ++it;
    }
    if(graphemePos > 0)
        return gd::String::npos;

    return std::distance(beginIt.base(), it.base());
}

GraphemeTools::size_type GraphemeTools::GraphemePosFromCodepointPos( const gd::String &str, String::size_type codepointPos)
{
    auto itCp = str.begin();
    while(itCp != str.end() && codepointPos > 0)
    {
        --codepointPos;
        ++itCp;
    }
    if(codepointPos > 0)
        return -1;

    //We count manually the grapheme count because itCp might be in the middle of
    //a grapheme
    auto itGr = begin(str);
    size_type graphemePos = 0;
    for(; itGr.base() < itCp; ++itGr, ++graphemePos);

    return graphemePos;
}

GraphemeTools::value_type GraphemeTools::GetGraphemeAt( const gd::String &str, const size_type position)
{
    auto it = begin(str);
    std::advance(it, position);

    return *it;
}


String& GraphemeTools::replace( gd::String &str, iterator i1, iterator i2, const String &replacement )
{
    str.replace(i1.base(), i2.base(), replacement);

    return str;
}

String& GraphemeTools::replace( gd::String &str, size_type pos, size_type len, const String &replacement )
{
    auto it = begin(str);
    for(; it != end(str) && pos > 0; ++it, --pos);
    if(pos > 0)
        throw std::out_of_range("[gd::GraphemeTools::replace] starting pos greater than size");

    auto it2 = it;
    for(; it2 != end(str) && len > 0; ++it2, --len);

    str.replace(it.base(), it2.base(), replacement);

    return str;
}

}
