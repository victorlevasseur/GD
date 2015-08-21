#ifndef GDCORE_UTF8_GRAPHEME_INTERFACE_H
#define GDCORE_UTF8_GRAPHEME_INTERFACE_H

#include "GDCore/String.h"
#include "GDCore/Utf8/utf8proc.h"

namespace gd
{

class ConstGraphemeInterface
{
    friend class String;

public:

    //Define the grapheme iterator
    #include "GraphemeIterator.inl"

    //Typedefs
    using const_iterator = GraphemeIterator<gd::String::const_iterator>;

    virtual ~ConstGraphemeInterface() = default;

protected:
    ConstGraphemeInterface(const String &str);

private:
    const String &str;
};

class GraphemeInterface : public ConstGraphemeInterface
{
    friend class String;

public:

    using iterator = GraphemeIterator<gd::String::iterator>;

    virtual ~GraphemeInterface() = default;

protected:
    GraphemeInterface(String &str);

private:
    String &str;
};

}

#endif
