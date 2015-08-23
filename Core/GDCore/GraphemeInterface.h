#ifndef GDCORE_UTF8_GRAPHEME_INTERFACE_H
#define GDCORE_UTF8_GRAPHEME_INTERFACE_H

#include "GDCore/String.h"
#include "GDCore/Utf8/utf8proc.h"

namespace gd
{

class GD_CORE_API ConstGraphemeInterface
{
    friend class String;

public:

    //Typedefs
    using value_type = gd::String;
    using const_reference = const gd::String&;
    using const_pointer = const gd::String*;

    using size_type = String::size_type;
    using difference_type = String::difference_type;

    //Define the grapheme iterator
    #include "GraphemeIterator.inl"
    using const_iterator = GraphemeIterator<String::const_iterator>;

    //Default virtual dtor
    virtual ~ConstGraphemeInterface() = default;

    const_iterator begin() const;

    const_iterator end() const;

    String::size_type size() const;

    /**
     * \return the position of the first codepoint of the **n**-th grapheme of the gd::String str
     */
    String::size_type CodepointPosFromGraphemePos( size_type graphemePos ) const;

    /**
     * \return the index of the grapheme (the number of grapheme before it) in the string from the position of its first codepoint
     */
    size_type GraphemePosFromCodepointPos( String::size_type codepointPos) const;

    value_type operator[](const size_type position) const;

protected:
    ConstGraphemeInterface(const String &str);

private:
    const String &m_str;
};

class GD_CORE_API GraphemeInterface : public ConstGraphemeInterface
{
    friend class String;

public:

    //Typedefs
    using reference = gd::String&;
    using pointer = gd::String*;

    using iterator = GraphemeIterator<gd::String::iterator>;

    //Default virtual dtor
    virtual ~GraphemeInterface() = default;

    iterator begin();

    iterator end();

    /**
     * \brief Replace the portion of the String between **i1** and **i2** (**i2** not
     * included) by the String **str**.
     * \return *this
     *
     * **Iterators :** All iterators may be invalidated.
     */
    String& replace( iterator i1, iterator i2, const String &str );

    /**
     * \brief Replace the portion of the String between the graphemes at **pos** and **pos** + **len**
     * (the grapheme at **pos** + **len** is not included)
     * \return *this
     *
     * **Iterators :** All iterators may be invalidated.
     */
    String& replace( size_type pos, size_type len, const String &str );

protected:
    GraphemeInterface(String &str);

private:
    String &m_str;
};

}

#endif
