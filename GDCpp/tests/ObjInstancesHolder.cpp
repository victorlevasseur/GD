/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
/**
 * @file Tests covering ObjInstancesHolder class.
 */
#include "catch.hpp"
#include "GDCore/CommonTools.h"
#include "GDCore/Project/Object.h"
#include "GDCore/Project/ClassWithObjects.h"
#include "GDCore/Project/Layout.h"
#include "GDCpp/RuntimeObject.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/ObjInstancesHolder.h"
#include "GDCpp/RuntimeGame.h"

TEST_CASE( "ObjInstancesHolder", "[common]" ) {
	SECTION("Basics") {
		gd::Object obj1("1");
		gd::Object obj2("2");

		RuntimeGame game;
		RuntimeScene scene(NULL, &game);

		std::shared_ptr<RuntimeObject> obj1A(new RuntimeObject(scene, obj1));
		std::shared_ptr<RuntimeObject> obj1B(new RuntimeObject(scene, obj1));
		std::shared_ptr<RuntimeObject> obj1C(new RuntimeObject(scene, obj1));

		std::shared_ptr<RuntimeObject> obj2A(new RuntimeObject(scene, obj2));
		std::shared_ptr<RuntimeObject> obj2B(new RuntimeObject(scene, obj2));
		std::shared_ptr<RuntimeObject> obj2C(new RuntimeObject(scene, obj2));

		//Adding objects
		ObjInstancesHolder container;
		container.AddObject(obj1A);
		container.AddObject(obj1B);
		container.AddObject(obj1C);
		container.AddObject(obj2A);
		container.AddObject(obj2B);
		container.AddObject(obj2C);

		REQUIRE(container.GetAllObjects().size() == 6);
		REQUIRE(container.GetObjects("1").size() == 3);
		REQUIRE(container.GetObjects("2").size() == 3);
		REQUIRE(container.GetObjectsRawPointers("2").size() == 3);

		ObjInstancesHolder copy = container;

		//Removing objects
		container.RemoveObject(obj2A);
		REQUIRE(container.GetObjects("2").size() == 2);

		container.RemoveObjects("2");
		REQUIRE(container.GetObjects("2").size() == 0);

		//Container copy (and objects inside it).
		container = copy;
		REQUIRE(container.GetAllObjects().size() == 6);
		REQUIRE(container.GetObjects("1").size() == 3);
		REQUIRE(container.GetObjects("2").size() == 3);
		REQUIRE(container.GetObjectsRawPointers("2").size() == 3);
	}
}
