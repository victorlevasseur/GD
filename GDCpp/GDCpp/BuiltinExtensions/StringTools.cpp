/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include <string>
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/String.h"
#include "GDCpp/GraphemeIterator.h"

using namespace std;

namespace GDpriv
{
namespace StringTools
{

/**
 * Expression function for getting a substring from a string
 */
gd::String GD_API SubStr(const gd::String & str, size_t start, size_t length )
{
    auto it = gd::grapheme::GetIterator(str, str.begin());
    while(start > 0 && it != gd::grapheme::GetIterator(str, str.end()))
    {
        start--;
        ++it;
    }
    if(start > 0)
        return "";

    auto it2 = it;
    while(length > 0 && it2 != gd::grapheme::GetIterator(str, str.end()))
    {
        length--;
        ++it2;
    }
    if(length > 0)
        return str.substr(
            std::distance(str.begin(), it.base()),
            gd::String::npos
            );
    else
        return str.substr(
            std::distance(str.begin(), it.base()),
            std::distance(str.begin(), it2.base())
            );
}

/**
 * Expression function for getting a character from a string
 */
gd::String GD_API StrAt(const gd::String & str, size_t pos )
{
    auto it = gd::grapheme::GetIterator(str, str.begin());
    while(pos > 0 && it != gd::grapheme::GetIterator(str, str.end()))
    {
        pos--;
        ++it;
    }
    if(pos > 0)
        return "";

    return *it;
}

/**
 * Expression function for getting a substring from a string
 */
unsigned int GD_API StrLen(const gd::String & str)
{
    return std::distance(gd::grapheme::GetIterator(str, str.begin()), gd::grapheme::GetIterator(str, str.end()));
}

/**
 * Expression function for finding a string in another
 */
int GD_API StrFind(const gd::String & str, const gd::String & findwhat)
{
    size_t pos = str.find(findwhat);

    if ( pos != gd::String::npos ) return pos;
    return -1;
}

/**
 * Expression function for finding a string in another
 */
int GD_API StrRFind(const gd::String & str, const gd::String & findwhat)
{
    size_t pos = str.rfind(findwhat);

    if ( pos != gd::String::npos ) return pos;
    return -1;
}

/**
 * Expression function for finding a string in another
 */
int GD_API StrFindFrom(const gd::String & str, const gd::String & findwhat, unsigned int start)
{
    size_t pos = str.find(findwhat, start);

    if ( pos != gd::String::npos ) return pos;
    return -1;
}

/**
 * Expression function for finding a string in another
 */
int GD_API StrRFindFrom(const gd::String & str, const gd::String & findwhat, unsigned int start)
{
    size_t pos = str.rfind(findwhat, start);

    if ( pos != gd::String::npos ) return pos;
    return -1;
}

gd::String GD_API NewLine()
{
    return "\n";
};

}
}
