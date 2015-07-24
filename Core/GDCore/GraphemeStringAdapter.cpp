/*
 * GDevelop Core
 * Copyright 2015 Victor Levasseur (victorlevasseur52@gmail.com).
 * This project is released under the MIT License.
 */

#include "GDCore/GraphemeStringAdapter.h"

namespace gd
{

GraphemeStringAdapter::GraphemeStringAdapter( String &str ) : str(str)
{

}

GraphemeStringAdapter::iterator GraphemeStringAdapter::begin()
{
    return iterator(str.begin(), str.begin(), str.end());
}

GraphemeStringAdapter::iterator GraphemeStringAdapter::end()
{
    return iterator(str.end(), str.begin(), str.end());
}

}
