/*
 * GDevelop C++ Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "SceneStack.h"
#include "RuntimeScene.h"
#include "RuntimeGame.h"
#include "CodeExecutionEngine.h"
#include "SceneNameMangler.h"

bool SceneStack::Step()
{
	if (stack.empty()) return false;

	auto & scene = stack.back();
	if (scene->RenderAndStep())
	{
		auto request = scene->GetRequestedChange();
        if (request.change == RuntimeScene::SceneChange::STOP_GAME) {
            return false;
        } else if (request.change == RuntimeScene::SceneChange::POP_SCENE) {
        	Pop();
        } else if (request.change == RuntimeScene::SceneChange::PUSH_SCENE) {
        	Push(request.requestedScene);
        } else if (request.change == RuntimeScene::SceneChange::REPLACE_SCENE) {
            Replace(request.requestedScene);
        } else if (request.change == RuntimeScene::SceneChange::CLEAR_SCENES) {
        	Replace(request.requestedScene, true);
        } else {
        	if (errorCallback) errorCallback("Unrecognized change in scene stack.");
        	return false;
        }
	}

	return true;
}

std::unique_ptr<RuntimeScene> SceneStack::Pop()
{
	if (stack.size() <= 1) return nullptr;

	std::unique_ptr<RuntimeScene> scene = std::move(stack.back());
	stack.pop_back();
	return scene;
}

RuntimeScene * SceneStack::Push(gd::String newSceneName)
{
    if (!game.HasLayoutNamed(newSceneName))
    {
        if (errorCallback) errorCallback("Scene \"" + newSceneName + "\" does not exist.");
        return nullptr;
    }

	std::unique_ptr<RuntimeScene> newScene(new RuntimeScene(window, &game));
    if (!newScene->LoadFromScene(game.GetLayout(newSceneName)))
    {
        if (errorCallback) errorCallback("Unable to load scene \"" + newSceneName + "\".");
        return nullptr;
    }

	if (loadCallback && !loadCallback(*newScene))
	{
		if (errorCallback) errorCallback("Unable to setup execution engine for scene \"" + newScene->GetName() + "\".");
		return nullptr;
	}

    newScene->ChangeRenderWindow(window);
	stack.push_back(std::move(newScene));
	return stack.back().get();
}

RuntimeScene * SceneStack::Replace(gd::String newSceneName, bool clear)
{
    if (clear)
    {
        while (!stack.empty()) stack.pop_back();
    }
    else
    {
        if (!stack.empty()) stack.pop_back();
    }
	return Push(newSceneName);
}
