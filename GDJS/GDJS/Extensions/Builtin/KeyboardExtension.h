/*
 * GDevelop JS Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#ifndef KEYBOARDEXTENSION_H
#define KEYBOARDEXTENSION_H
#include "GDCore/Extensions/PlatformExtension.h"

namespace gdjs
{

/**
 * \brief Built-in extension providing functions related the keyboard
 *
 * \ingroup BuiltinExtensions
 */
class KeyboardExtension : public gd::PlatformExtension
{
public :

    KeyboardExtension();
    virtual ~KeyboardExtension() {};
};

}
#endif // KEYBOARDEXTENSION_H
