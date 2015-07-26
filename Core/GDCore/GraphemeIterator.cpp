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

}

}
