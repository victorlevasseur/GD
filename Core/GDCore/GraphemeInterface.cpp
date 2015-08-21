#include "GDCore/GraphemeInterface.h"

namespace gd
{

ConstGraphemeInterface::ConstGraphemeInterface(const String &str) : str(str)
{

}

GraphemeInterface::GraphemeInterface(String &str) : ConstGraphemeInterface(str), str(str)
{

}

}
