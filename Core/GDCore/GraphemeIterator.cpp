/*
 * GDevelop Core
 * Copyright 2015 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#include "GDCore/GraphemeIterator.h"

namespace gd
{

namespace grapheme
{

GraphemeIterator<gd::String::const_iterator> GD_CORE_API GetIterator(const gd::String &str, gd::String::const_iterator it)
{
    return GraphemeIterator<gd::String::const_iterator>(it, str.begin(), str.end());
}

GraphemeIterator<gd::String::iterator> GD_CORE_API GetIterator(gd::String &str, gd::String::iterator it)
{
    return GraphemeIterator<gd::String::iterator>(it, str.begin(), str.end());
}

gd::String::size_type GetCodepointPosFromGrapheme( const gd::String &str, size_t n )
{
    auto it = GetIterator(str, str.begin());
    auto beginIt = GetIterator(str, str.begin());
    auto endIt = GetIterator(str, str.end());

    while(it != endIt && n > 0)
    {
        --n;
        ++it;
    }
    if(n > 0)
        return gd::String::npos;

    return std::distance(beginIt.base(), it.base());
}

size_t GetGraphemePosFromCodepoint( const gd::String &str, gd::String::size_type codepointPos)
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
    auto itGr = GetIterator(str, str.begin());
    size_t graphemePos = 0;
    for(; itGr.base() < itCp; ++itGr, graphemePos++);

    return graphemePos;
}

}

}
