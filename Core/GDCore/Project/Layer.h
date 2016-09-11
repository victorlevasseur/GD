/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#ifndef GDCORE_LAYER_H
#define GDCORE_LAYER_H
#include "GDCore/String.h"
#include <vector>
#include <memory>
namespace gd { class Effect; }
namespace gd { class Camera; }
namespace gd { class SerializerElement; }

namespace gd
{

/**
 * \brief Represents a layer of a layout.
 *
 * \see gd::Layout
 * \ingroup PlatformDefinition
 */
class GD_CORE_API Layer
{
public:
    Layer();
    virtual ~Layer() {};

    /**
     * \brief Change layer name
     */
    void SetName(const gd::String & name_) { name = name_; }

    /**
     * \brief Get layer name
     */
    const gd::String & GetName() const { return name; }

    /**
     * \brief Change if layer is displayed or not
     */
    void SetVisibility(bool isVisible_) { isVisible = isVisible_; }

    /**
     * \brief Return true if layer will be displayed at the layout startup.
     */
    bool GetVisibility() const { return isVisible; }

    /** \name Cameras
     */
    ///@{

    /**
     * \brief Change the number of cameras inside the layer.
     */
    void SetCameraCount(std::size_t n);

    /**
     * \brief Get cameras count.
     */
    inline std::size_t GetCameraCount() const { return cameras.size(); };

    /**
     * \brief Return a reference to a camera
     */
    inline const Camera & GetCamera(std::size_t n) const { if ( n >= GetCameraCount() ) return badCamera; return cameras[n]; }

    /**
     * \brief Return a reference to a camera
     */
    inline Camera & GetCamera(std::size_t n) { if ( n >= GetCameraCount() ) return badCamera; return cameras[n]; }

    /**
     * \brief Delete a specific camera.
     */
    inline void DeleteCamera(std::size_t n) { if ( n >= GetCameraCount() ) return; cameras.erase(cameras.begin()+n); }

    /**
     * \brief Add an already existing camera.
     */
    inline void AddCamera(const Camera & camera) { cameras.push_back(camera); };

    ///@}

    /** \name Effects
     */
    ///@{
	/**
	 * \brief Return true if the effect called "name" exists.
	 */
	bool HasEffectNamed(const gd::String & name) const;

	/**
	 * \brief Return a reference to the effect called "name".
	 */
	Effect & GetEffect(const gd::String & name);

	/**
	 * \brief Return a reference to the effect called "name".
	 */
	const Effect & GetEffect(const gd::String & name) const;

	/**
	 * Return a reference to the effect at position "index" in the effects list
	 */
	Effect & GetEffect(std::size_t index);

	/**
	 * Return a reference to the effect at position "index" in the effects list
	 */
	const Effect & GetEffect (std::size_t index) const;

	/**
	 * Return the position of the effect called "name" in the effects list
	 */
	std::size_t GetEffectPosition(const gd::String & name) const;

	/**
	 * Return the number of effecst.
	 */
	std::size_t GetEffectsCount() const;

	/**
	 * Add a new effect at the specified position in the effects list.
	 */
	gd::Effect & InsertNewEffect(const gd::String & name, std::size_t position);

	/**
	 * \brief Add the a copy of the specified effect in the effects list.
	 * \note No pointer or reference must be kept on the layer passed as parameter.
	 * \param theEffect The effect that must be copied and inserted into the effects list
	 * \param position Insertion position.
	 */
	void InsertEffect(const Effect & theEffect, std::size_t position);

	/**
	 * Remove the specified effect.
	 */
	void RemoveEffect(const gd::String & name);

	/**
	 * Swap the position of two effects.
	 */
	void SwapEffects(std::size_t firstEffectIndex, std::size_t secondEffectIndex);
    ///@}

    #if defined(GD_IDE_ONLY)
    /**
     * \brief Serialize layer.
     */
    void SerializeTo(SerializerElement & element) const;
    #endif

    /**
     * \brief Unserialize the layer.
     */
    void UnserializeFrom(const SerializerElement & element);

private:

    gd::String name; ///< The name of the layer
    bool isVisible; ///< True if the layer is visible
    std::vector<gd::Camera> cameras; ///< The camera displayed by the layer
    std::vector<std::shared_ptr<gd::Effect>> effects; ///< The effects applied to the layer.

    static gd::Camera badCamera;
    static gd::Effect badEffect;
};

/**
 * \brief Functor testing layer name
 *
 * \see gd::Layer
 */
struct LayerHasName : public std::binary_function<gd::Layer, gd::String, bool> {
    bool operator()(const Layer & layer, const gd::String & name) const { return layer.GetName() == name; }
};


/**
 * \brief A camera is used to render a specific area of a layout.
 *
 * \see gd::Layout
 * \ingroup PlatformDefinition
 */
class GD_CORE_API Camera
{
public:
    Camera();
    ~Camera() {};

    /**
     * \brief Change the viewport, i.e the area of the window where the camera will be displayed.
     * \note The coordinates must be between 0 and 1.
     */
    void SetViewport(float x1_, float y1_, float x2_, float y2_) { x1 = x1_; x2 = x2_; y1 = y1_; y2 = y2_; };
    void SetViewportX1(float x1_) { x1 = x1_; };
    void SetViewportY1(float y1_) { y1 = y1_; };
    void SetViewportX2(float x2_) { x2 = x2_; };
    void SetViewportY2(float y2_) { y2 = y2_; };
    float GetViewportX1() const { return x1; };
    float GetViewportY1() const { return y1; };
    float GetViewportX2() const { return x2; };
    float GetViewportY2() const { return y2; };

    /**
     * \brief Change the size of the rendered area of the scene, in pixels.
     */
    void SetSize(float width_, float height_) { width = width_; height = height_; };
    float GetWidth() const { return width; };
    float GetHeight() const { return height; };

    void SetUseDefaultSize(bool useDefaultSize = true) { defaultSize = useDefaultSize; };
    bool UseDefaultSize() const {return defaultSize;}

    void SetUseDefaultViewport(bool useDefaultViewport = true) { defaultViewport = useDefaultViewport; };
    bool UseDefaultViewport() const {return defaultViewport;}

private:

    bool defaultSize; ///< True if the camera use the default window size
    bool defaultViewport; ///< True if the camera use the default viewport size

    float x1;
    float y1;
    float x2;
    float y2;
    float width; ///< The width of the rendered area
    float height; ///< The height of the rendered area
};

}

#endif // GDCORE_LAYER_H
