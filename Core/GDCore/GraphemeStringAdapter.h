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

class GraphemeStringAdapter
{
public:

    template<class T>
    class GD_CORE_API GraphemeIterator : public std::iterator<std::bidirectional_iterator_tag, String::value_type, String::difference_type>
    {
        friend class GraphemeStringAdapter;

    public:
        GraphemeIterator() : strIt(), beginIt(), endIt() {};
        GraphemeIterator(const GraphemeIterator<T> &other) : strIt(other.strIt), beginIt(other.beginIt), endIt(other.endIt) {}
        GraphemeIterator<T>& operator=(const GraphemeIterator<T> &other) { strIt = other.strIt; beginIt = other.beginIt; endIt = other.endIt; return *this; }

        gd::String operator*()
        {
            T it = strIt;

            std::u32string grapheme;
            grapheme.push_back(*it);

            ++it;
            char32_t codepoint = *it;

            while( !utf8proc_grapheme_break(grapheme.back(), codepoint) )
            {
                grapheme.push_back(codepoint);
                ++it;
                codepoint = *it;
            }

            return gd::String(grapheme);
        }

        GraphemeIterator<T>& operator++()
        {
            //Find next grapheme
            T it = strIt;
            if(it == endIt)
            {
                return *this;
            }
            char32_t previousCodepoint = *it;

            ++it;
            if(it == endIt)
            {
                //We already reached the end on the second character
                strIt = it;
                return *this;
            }
            char32_t codepoint = *it;

            while( !utf8proc_grapheme_break(previousCodepoint, codepoint) )
            {
                previousCodepoint = codepoint;

                ++it;
                if(it == endIt)
                    break;
                codepoint = *it;
            }
            strIt = it;

            return *this;
        }
        GraphemeIterator<T> operator++(int) { GraphemeIterator<T> tmp(*this); operator++(); return tmp; }
        GraphemeIterator<T>& operator--()
        {
            //Find previous grapheme
            T it = strIt;
            if(it == beginIt)
                return *this;

            --it;
            if(it == beginIt) //The previous character is the beginning of the string, so it's the previous grapheme
            {
                strIt = it;
                return *this;
            }
            char32_t codepoint = *it;

            --it;
            char32_t previousCodepoint = *it;

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
        GraphemeIterator(T strIt, T beginIt, T endIt) : strIt(strIt), beginIt(beginIt), endIt(endIt) {};

        T strIt;
        T beginIt;
        T endIt;
    };

    typedef GraphemeIterator<gd::String::iterator> iterator;
    typedef GraphemeIterator<gd::String::const_iterator> const_iterator;

    GraphemeStringAdapter( String &str );

    GraphemeStringAdapter( const GraphemeStringAdapter & ) = delete;
    GraphemeStringAdapter& operator=( const GraphemeStringAdapter & ) = delete;

    iterator begin();
    iterator end();

private:
    String &str;

};

}

#endif
