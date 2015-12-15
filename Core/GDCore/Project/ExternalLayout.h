/*
 * GDevelop Core
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef GDCORE_EXTERNALLAYOUT_H
#define GDCORE_EXTERNALLAYOUT_H
#include "GDCore/String.h"
#include "GDCore/Project/InitialInstancesContainer.h"
#include <memory>
namespace gd { class SerializerElement; }
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include "GDCore/IDE/Dialogs/LayoutEditorCanvas/LayoutEditorCanvasOptions.h"
#endif

namespace gd
{

/**
 * \brief An external layout allows to create layouts of objects that can be then inserted on a layout.
 */
class GD_CORE_API ExternalLayout
{
public:
    ExternalLayout() {};
    virtual ~ExternalLayout() {};

    /**
     * \brief Return a pointer to a new ExternalLayout constructed from this one.
     */
    ExternalLayout * Clone() const { return new ExternalLayout(*this); };

    /**
     * \brief Return the name of the external layout.
     */
    const gd::String & GetName() const {return name;}

    /**
     * \brief Change the name of the external layout.
     */
    void SetName(const gd::String & name_) {name = name_;}

    /**
     * \brief Return the container storing initial instances.
     */
    const gd::InitialInstancesContainer & GetInitialInstances() const { return instances; }

    /**
     * \brief Return the container storing initial instances.
     */
    gd::InitialInstancesContainer & GetInitialInstances() { return instances; }

    #if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
    /**
     * \brief Get the user settings for the IDE.
     */
    const gd::LayoutEditorCanvasOptions & GetAssociatedSettings() const {return editionSettings;}

    /**
     * \brief Get the user settings for the IDE.
     */
    gd::LayoutEditorCanvasOptions & GetAssociatedSettings() {return editionSettings;}
    #endif

    /** \name Serialization
     */
    ///@{
    #if defined(GD_IDE_ONLY)
    /**
     * \brief Serialize external layout.
     */
    void SerializeTo(SerializerElement & element) const;
    #endif

    /**
     * \brief Unserialize the external layout.
     */
    void UnserializeFrom(const SerializerElement & element);
    ///@}

private:

    gd::String name;
    gd::InitialInstancesContainer instances;
    #if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
    gd::LayoutEditorCanvasOptions editionSettings;
    #endif
};

/**
 * \brief Functor testing ExternalLayout' name
 */
struct ExternalLayoutHasName : public std::binary_function<std::shared_ptr<gd::ExternalLayout>, gd::String, bool> {
    bool operator()(const std::shared_ptr<gd::ExternalLayout> & externalLayout, gd::String name) const { return externalLayout->GetName() == name; }
};

}

#endif // GDCORE_EXTERNALLAYOUT_H
