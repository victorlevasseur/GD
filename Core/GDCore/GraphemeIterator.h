/*
 * GDevelop Core
 * Copyright 2015 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#ifndef GDCORE_GRAPHEME_STRING_ADAPTER
#define GDCORE_GRAPHEME_STRING_ADAPTER

#include <iostream>
#include "GDCore/String.h"
#include "GDCore/Utf8/utf8proc.h"

namespace gd
{

namespace grapheme
{

template<class T>
class GD_CORE_API GraphemeIterator : public std::iterator<std::bidirectional_iterator_tag, String::value_type, String::difference_type>
{
    friend class GraphemeStringAdapter;

public:
    GraphemeIterator() : strIt(), beginIt(), endIt() {};
    GraphemeIterator(T strIt, T beginIt, T endIt) : strIt(strIt), beginIt(beginIt), endIt(endIt) {};
    GraphemeIterator(const GraphemeIterator<T> &other) : strIt(other.strIt), beginIt(other.beginIt), endIt(other.endIt) {}
    template<class U> GraphemeIterator(const GraphemeIterator<U> &other) : strIt(other.strIt) {} //Convert from const_iterator to iterator
    GraphemeIterator<T>& operator=(const GraphemeIterator<T> &other) { strIt = other.strIt; beginIt = other.beginIt; endIt = other.endIt; return *this; }

    gd::String operator*()
    {
        T it = strIt;
        String::value_type previousCodepoint = *it;

        ++it;
        String::value_type codepoint = *it;

        while( !utf8proc_grapheme_break(previousCodepoint, codepoint) )
        {
            previousCodepoint = codepoint;
            ++it;
            codepoint = *it;
        }

        std::string grapheme(strIt.base(), it.base());

        return gd::String(grapheme.c_str());
    }

    GraphemeIterator<T>& operator++()
    {
        //Find next grapheme
        T it = strIt;
        T previousIt = strIt;
        String::value_type previousCodepoint = it.ReadCodePointAndIterate();
        if(it == endIt)
        {
            //We already reached the end on the second character
            strIt = it;
            return *this;
        }
        previousIt = it;
        String::value_type codepoint = it.ReadCodePointAndIterate();

        while( !utf8proc_grapheme_break(previousCodepoint, codepoint) )
        {
            previousCodepoint = codepoint;

            if(it == endIt)
                break;
            previousIt = it;
            codepoint = it.ReadCodePointAndIterate();
        }
        strIt = previousIt;

        return *this;
    }
    GraphemeIterator<T> operator++(int) { GraphemeIterator<T> tmp(*this); operator++(); return tmp; }
    GraphemeIterator<T>& operator--()
    {
        //Find previous grapheme
        T it = strIt;

        --it;
        if(it == beginIt) //The previous character is the beginning of the string, so it's the previous grapheme
        {
            strIt = it;
            return *this;
        }
        String::value_type codepoint = *it;

        --it;
        String::value_type previousCodepoint = *it;

        while( !utf8proc_grapheme_break(previousCodepoint, codepoint) )
        {
            codepoint = previousCodepoint;

            if(it == beginIt)
            {
                strIt = it;
                return *this;
            }

            --it;
            previousCodepoint = *it;
        }

        ++it;
        strIt = it;

        return *this;
    }
    GraphemeIterator<T> operator--(int) { GraphemeIterator<T> tmp(*this); operator--(); return tmp; }

    bool operator==(const GraphemeIterator<T> &other) { return (strIt == other.strIt); }
    bool operator!=(const GraphemeIterator<T> &other) { return !operator==(other); }

    bool operator<(const GraphemeIterator<T> &other) { return (strIt < other.strIt); }
    bool operator<=(const GraphemeIterator<T> &other) { return (strIt <= other.strIt); }
    bool operator>(const GraphemeIterator<T> &other) { return (strIt > other.strIt); }
    bool operator>=(const GraphemeIterator<T> &other) { return (strIt >= other.strIt); }

    T base() const {return strIt;}

private:

    T strIt;
    T beginIt;
    T endIt;
};

/**
 * \return a GraphemeIterator (on gd::String::const_iterator) from a string and a gd::String::const_iterator
 */
GraphemeIterator<gd::String::const_iterator> GD_CORE_API GetIterator(const gd::String &str, gd::String::const_iterator it);

/**
 * \return a GraphemeIterator (on gd::String::iterator) from a string and a gd::String::iterator
 */
GraphemeIterator<gd::String::iterator> GD_CORE_API GetIterator(gd::String &str, gd::String::iterator it);

/**
 * \return the position of the first codepoint of the **n**-th grapheme of the gd::String str
 */
gd::String::size_type GD_CORE_API FromGraphemePos( const gd::String &str, size_t n );

/**
 * \return the index of the grapheme (the number of grapheme before it) in the string from the position of its first codepoint
 */
size_t GD_CORE_API FromCodepoint( const gd::String &str, gd::String::size_type codepointPos);

size_t GD_CORE_API size( const gd::String &str );

gd::String GD_CORE_API substr( const gd::String &str, size_t start, size_t length );

}

}

#endif
