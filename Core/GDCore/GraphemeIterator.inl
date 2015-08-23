template<class T>
class GD_CORE_API GraphemeIterator : public std::iterator<std::bidirectional_iterator_tag, value_type, difference_type>
{
public:
    GraphemeIterator() : strIt(), beginIt(), endIt() {};
    GraphemeIterator(T strIt, T beginIt, T endIt) : strIt(strIt), beginIt(beginIt), endIt(endIt) {};
    GraphemeIterator(const GraphemeIterator<T> &other) : strIt(other.strIt), beginIt(other.beginIt), endIt(other.endIt) {}
    template<class U> GraphemeIterator(const GraphemeIterator<U> &other) : strIt(other.strIt) {} //Convert from const_iterator to iterator
    GraphemeIterator<T>& operator=(const GraphemeIterator<T> &other) { strIt = other.strIt; beginIt = other.beginIt; endIt = other.endIt; return *this; }

    value_type operator*()
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

        return value_type(grapheme.c_str());
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
