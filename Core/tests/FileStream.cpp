/*
 * GDevelop Core
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
/**
 * @file Tests covering FileStream class of GDevelop Core.
 */
#include "catch.hpp"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/FileStream.h"

#include <chrono>
#include <fstream>
#include <memory>

TEST_CASE( "FileStream", "[common][fstream]" ) {

	//Creating the test file
	std::ofstream testFile("FileStreamTest.test");
	REQUIRE(testFile.is_open());

	testFile << "this is the first line of the test file!\n";
	testFile << "the last line!";
	testFile.close();

    SECTION("Input file") {
		gd::FileStream f;
		f.open("FileStreamTest.test", std::ios_base::in);

		REQUIRE(f.is_open() == true);
		REQUIRE(f.tellg() == 0);

		std::string lineContent;
		REQUIRE(f.eof() == false);
		std::getline(f, lineContent);
		REQUIRE(lineContent == "this is the first line of the test file!");

		REQUIRE(f.eof() == false);
		std::getline(f, lineContent);
		REQUIRE(lineContent == "the last line!");

		REQUIRE(f.eof() == true);
		REQUIRE(f.fail() == false);

		f.close();
		REQUIRE(f.is_open() == false);
		REQUIRE(f.fail() == false);
	}

	SECTION("Output/Input file with special characters in filepath") {
		gd::FileStream output("\xEA\x88\xA1.txt", std::ios_base::out);
		REQUIRE(output.is_open() == true);

		output << "TEST";
		output.close();

		gd::FileStream input("\xEA\x88\xA1.txt", std::ios_base::in);
		REQUIRE(input.is_open() == true);

		std::string lineContent;
		REQUIRE(input.eof() == false);
		std::getline(input, lineContent);
		REQUIRE(lineContent == "TEST");

		REQUIRE(input.eof() == true);

		input.close();
		REQUIRE(input.is_open() == false);
		REQUIRE(input.fail() == false);
	}

	SECTION("File opening failure") {
		gd::FileStream f("\xE4\x84\xA2", std::ios_base::in); //A file that doesn't exist

		REQUIRE(f.is_open() == false);
		REQUIRE(f.fail() == true); //As the opening failed, the "fail" flag should be set

		f.close();
		REQUIRE(f.bad() == true); //As no files are opened, this should set the "bad" flag of the stream
	}

	SECTION("std::ios_base::ate") { //As ate is "emulated", needs testing
		gd::FileStream f;
		f.open("FileStreamTest.test", std::ios_base::in|std::ios_base::ate);

		REQUIRE(f.is_open() == true);
		REQUIRE(f.tellg() > 0);
	}

	SECTION("Benchmark") {
		auto stdStart = std::chrono::high_resolution_clock::now();

		std::fstream file;
		for(auto i = 0; i < 10000; ++i)
		{
			file.open("aaa.txt", std::ios_base::in|std::ios_base::binary);
			file.close();
		}

		auto stdEnd = std::chrono::high_resolution_clock::now();

		auto stdDuration = stdEnd - stdStart;

		auto gdStart = std::chrono::high_resolution_clock::now();

		gd::FileStream file2;
		for(auto i = 0; i < 10000; ++i)
		{
			file2.open("aaa.txt", std::ios_base::in|std::ios_base::binary);
			file2.close();
		}

		auto gdEnd = std::chrono::high_resolution_clock::now();

		auto gdDuration = gdEnd - gdStart;

		WARN( "std::fstream opening/closing (10000 times): " << std::chrono::duration_cast<std::chrono::microseconds>(stdDuration).count() << "us" );
		WARN( "gd::FileStream opening/closing (10000 times): " << std::chrono::duration_cast<std::chrono::microseconds>(gdDuration).count() << "us" );
		WARN( "--> Perf comparison: " << ((float)std::chrono::duration_cast<std::chrono::microseconds>(stdDuration).count() / (float)std::chrono::duration_cast<std::chrono::microseconds>(gdDuration).count()) );
	}
}
