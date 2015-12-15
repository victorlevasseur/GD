#ifndef GDCORE_STRING_GRAPHEME_TOOLS_H
#define GDCORE_STRING_GRAPHEME_TOOLS_H

#include "GDCore/String.h"
#include "GDCore/Utf8/utf8proc.h"

namespace gd
{
class GraphemeTools
{
public:

    //Typedefs
    using value_type = String;
    using const_reference = const String&;
    using const_pointer = const String*;
    using reference = String&;
    using pointer = String*;

    using size_type = String::size_type;
    using difference_type = String::difference_type;

    //Include the GraphemeIterator
    #include "GraphemeIterator.inl"

    //Iterators typedef
    using iterator = GraphemeIterator<String::iterator>;
    using const_iterator = GraphemeIterator<String::const_iterator>;

    static const_iterator begin( const String &str );

    static const_iterator end( const String &str );

    static iterator begin( String &str );

    static iterator end( String &str );

    static String::size_type size( const String &str );

    /**
     * \return the position of the first codepoint of the **n**-th grapheme of the String str
     */
    static String::size_type CodepointPosFromGraphemePos( const String &str, size_type graphemePos );

    /**
     * \return the index of the grapheme (the number of grapheme before it) in the string from the position of its first codepoint
     */
    static String::size_type GraphemePosFromCodepointPos( const String &str, String::size_type codepointPos);

    static value_type GetGraphemeAt( const String &str, const size_type position);

    /**
     * \brief Replace the portion of the String between **i1** and **i2** (**i2** not
     * included) by the String **replacement**.
     * \return *this
     *
     * **Iterators :** All iterators may be invalidated.
     */
    static String& replace( String &str, iterator i1, iterator i2, const String &replacement );

    /**
     * \brief Replace the portion of the String between the graphemes at **pos** and **pos** + **len**
     * (the grapheme at **pos** + **len** is not included)
     * \return *this
     *
     * **Iterators :** All iterators may be invalidated.
     */
    static String& replace( String &str, size_type pos, size_type len, const String &replacement );
};
}

#endif
