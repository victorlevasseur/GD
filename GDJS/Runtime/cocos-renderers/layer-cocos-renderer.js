/*
 * GDevelop JS Platform
 * Copyright 2013-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

gdjs.LayerCocosRenderer = function(layer, runtimeSceneRenderer)
{
    var CocosLayer = cc.Layer.extend({
        ctor:function () {
            this._super();
            return true;
        }
    });

    this._layer = layer;
    this.convertYPosition = runtimeSceneRenderer.convertYPosition;

    var effects = this._layer.getEffects();
    if (effects.length === 0) {
        this._cocosLayer = new CocosLayer();
        runtimeSceneRenderer.getCocosScene().addChild(this._cocosLayer);
    } else {
        var gameRenderer = runtimeSceneRenderer._runtimeScene.getGame().getRenderer(); //TODO
        this._renderTexture = new cc.RenderTexture(
            gameRenderer.getCurrentWidth(), gameRenderer.getCurrentHeight());
        this._renderTexture.setPosition(cc.p(
            gameRenderer.getCurrentWidth() / 2, gameRenderer.getCurrentHeight() / 2));
        this._renderTexture.retain();

        this._cocosLayer = new CocosLayer();
        this._cocosLayer.retain();
        runtimeSceneRenderer.getCocosScene().addChild(this._renderTexture);

        this._makeShaders();
    }
}

gdjs.LayerRenderer = gdjs.LayerCocosRenderer; //Register the class to let the engine use it.

gdjs.LayerCocosRenderer.prototype.onSceneUnloaded = function() {
    this._unloaded = true;
    if (this._shaders) {
        for(var i = 0;i < this._shaders.length;++i) {
            this._shaders[i].shader.release();
        }
    }
    if (this._renderTexture) {
        // this._cocosLayer.release(); //TODO
        // this._renderTexture.release(); //TODO
    }
}

gdjs.LayerCocosRenderer.prototype._makeShaders = function() {
    if (this._shaders) {
        console.log('Shaders are already made for this layer');
        return;
    } else if (!this._renderTexture) {
        console.log('You can\'t apply shaders on a layer that is not using a cc.RenderTexture');
        return;
    }

    var effects = this._layer.getEffects();
    if (effects.length === 0) {
        return;
    } else if (effects.length > 1) {
        console.log('Only a single effect by Layer is supported for now by Cocos2d-JS renderer');
    }

    var effect = gdjs.CocosTools.getEffect(effects[0].effectName);
    if (!effect) {
        console.log('Shader \"' + effects[0].name + '\" not found');
        return;
    }

    var theShader = {
        name: effects[0].name,
        shader: effect.makeShader(),
        uniforms: {}
    };
    theShader.shader.retain();
    for (var i = 0;i < effect.uniformNames.length;++i) {
        var name = effect.uniformNames[i];
        theShader.uniforms[name] =
            theShader.shader.getUniformLocationForName(name);
    }

    this._renderTexture.getSprite().setShaderProgram(theShader.shader);
    this._shaders = [theShader];
}

gdjs.LayerCocosRenderer.prototype.render = function() {
    if (this._unloaded) {
        console.log('Tried to render unloaded layer ' + this._layer._name);
        return;
    }

    if (this._renderTexture) {
        this._renderTexture.beginWithClear(0,0,0,0);
        this._cocosLayer.visit();
        this._renderTexture.end();
    }
}

gdjs.LayerCocosRenderer.prototype.updatePosition = function() {
    var angle = gdjs.toRad(this._layer.getCameraRotation());
    var zoomFactor = this._layer.getCameraZoom();

	this._cocosLayer.setRotation(-this._layer.getCameraRotation());
	this._cocosLayer.setScale(zoomFactor, zoomFactor);

	var centerX = (this._layer.getCameraX()-this._layer.getWidth()/2)*Math.cos(-angle)
        - (this._layer.getCameraY()-this._layer.getHeight()/2)*Math.sin(-angle);
	var centerY = (this._layer.getCameraX()-this._layer.getWidth()/2)*Math.sin(-angle)
        + (this._layer.getCameraY()-this._layer.getHeight()/2)*Math.cos(-angle);

	this._cocosLayer.setPositionX(-centerX);
	this._cocosLayer.setPositionY(+centerY);
};

gdjs.LayerCocosRenderer.prototype.updateVisibility = function(visible) {
    this._cocosLayer.setVisible(visible);
}

gdjs.LayerCocosRenderer.prototype.addRendererObject = function(child, zOrder) {
    this._cocosLayer.addChild(child, zOrder);
};

gdjs.LayerCocosRenderer.prototype.changeRendererObjectZOrder = function(child, newZOrder) {
    child.setLocalZOrder(newZOrder);
};

gdjs.LayerCocosRenderer.prototype.removeRendererObject = function(child) {
    this._cocosLayer.removeChild(child);
};

gdjs.LayerCocosRenderer.prototype.setEffectParameter = function (name, parameterName, value) {
    for (var i = 0;i < this._shaders.length; ++i) {
        if (this._shaders[i].name === name) {
            var theShader = this._shaders[i];

            if (theShader.uniforms.hasOwnProperty(parameterName)) {
                theShader.shader.use();
                gdjs.CocosTools.setUniformLocationWith1f(this._renderTexture.getSprite(),
                    theShader.shader, theShader.uniforms[parameterName],
                    parameterName, value);
            }
        }
    }
};
