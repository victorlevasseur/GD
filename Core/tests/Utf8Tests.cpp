/*
 * GDevelop Core
 * Copyright 2015 Victor Levasseur (victorlevasseur52@gmail.com)
 * This project is released under the MIT License.
 */
/**
 * @file Tests covering utf8 features from GDevelop Core.
 */

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "GDCore/String.h"
#include "GDCore/GraphemeInterface.h"
#include <SFML/System/String.hpp>

TEST_CASE( "Utf8 String", "[common][utf8]") {
	SECTION("ctor & conversions") {
		gd::String str = u8"UTF8 a été testé !";

		sf::String sfStr = str;
		std::u32string u32str = str.ToUTF32();

		REQUIRE( str == gd::String::FromSfString(sfStr) );
		REQUIRE( str == gd::String::FromUTF32(u32str) );
	}

	SECTION("comparison operators") {
		REQUIRE( gd::String(u8"UTF8") == gd::String(u8"UTF8") );
		REQUIRE( gd::String(u8"UTF8") != gd::String(u8"UTF32") );
	}

	SECTION("size") {
		gd::String str = u8"UTF8 a été testé !";

		REQUIRE( str.size() == 18 );
	}

	SECTION("substr") {
		gd::String str = u8"UTF8 a été testé !";

		REQUIRE( str.substr(5, 7) == u8"a été t" );
		REQUIRE( str.substr(5, gd::String::npos) == u8"a été testé !" );

		REQUIRE_THROWS_AS( str.substr(50, 5), std::out_of_range );
	}

	SECTION("insert") {
		gd::String str = u8"Une fonctionnalité a été testée !";
		str.insert(25, u8"vraiment ");

		REQUIRE( str == u8"Une fonctionnalité a été vraiment testée !" );
		REQUIRE_THROWS_AS( str.insert(150, u8"This gonna fail"), std::out_of_range );
	}

	SECTION("replace") {
		//Testing the interval version of replace
		gd::String str = u8"UTF8 a été testé !";

		REQUIRE( str.replace(11, 5, u8"vérifié") == u8"UTF8 a été vérifié !" );
		REQUIRE( str.replace(11, gd::String::npos, u8"vraiment très testé !")
			== u8"UTF8 a été vraiment très testé !" );

		REQUIRE_THROWS_AS( str.replace(50, 5, u8"Cela va planter."),
			std::out_of_range );

		//Testing the iterator version of replace
		gd::String str2 = u8"UTF8 a été testé !";

		gd::String::iterator i1 = str2.begin();
		std::advance(i1, 11);
		gd::String::iterator i2 = i1;
		std::advance(i2, 5);

		REQUIRE( str2.replace(i1, i2, u8"vérifié") == u8"UTF8 a été vérifié !" );

		gd::String::iterator i3 = str2.begin();
		std::advance(i3, 11);

		REQUIRE( str2.replace(i3, str2.end(), u8"vraiment très testé !")
			== u8"UTF8 a été vraiment très testé !" );
	}

	SECTION("erase") {
		{
			gd::String str = u8"UTF8 a été testé !";
			str.erase(4, 6);
			REQUIRE( str == u8"UTF8 testé !" );
		}
		{
			gd::String str = u8"UTF8 a été testé !";
			str.erase(4, gd::String::npos);
			REQUIRE( str == "UTF8" );
		}
		{
			gd::String str = u8"UTF8 a été testé !";
			REQUIRE_THROWS_AS( str.erase(100, 5), std::out_of_range );
		}
		{
			gd::String str = u8"UTF8 a été testé !";
			gd::String::iterator first = str.begin();
			std::advance(first, 4);
			gd::String::iterator last = first;
			std::advance(last, 6);

			gd::String::iterator it = str.erase(first, last);

			REQUIRE( str == u8"UTF8 testé !" );
			REQUIRE( std::distance(str.begin(), it) == 4 ); //Also check the returned iterator

			gd::String::iterator it2 = str.erase(it);

			REQUIRE( str == u8"UTF8testé !" );
			REQUIRE( std::distance(str.begin(), it2) == 4 );
		}
	}

	SECTION("find") {
		gd::String str = u8"UTF8 a été testé !";

		REQUIRE( str.find( u8"té", 0) == 8);
		REQUIRE( str.find( u8"té", 8) == 8);
		REQUIRE( str.find( u8"té", 9) == 14);
		REQUIRE( str.find( u8"té", 14) == 14);
		REQUIRE( str.find( u8"té", 15) == gd::String::npos);
	}

	SECTION("rfind") {
		gd::String str = u8"UTF8 a été testé !";
		REQUIRE( str.rfind(u8"té !", gd::String::npos) == 14 );
		REQUIRE( str.rfind(u8"té", gd::String::npos) == 14 );
		REQUIRE( str.rfind(u8"té", 14) == 14 );
		REQUIRE( str.rfind(u8"té", 13) == 8 );
		REQUIRE( str.rfind(u8"té", 8) == 8 );
		REQUIRE( str.rfind(u8"té", 7) == std::string::npos );
	}

	SECTION("find_first/last(_not)_of") {
		gd::String str = u8"Arriveras-tu à trouver un caractère sans accent ?";

		REQUIRE( str.find_first_of(u8"àéèù") == 13 );
		REQUIRE( str.find_first_of(u8"àéèù", 13) == 13 );
		REQUIRE( str.find_first_of(u8"àéèù", 14) == 32 );
		REQUIRE( str.find_first_of(u8"àéèù", 32) == 32 );
		REQUIRE( str.find_first_of(u8"àéèù", 33) == gd::String::npos );
		REQUIRE( str.find_first_of(u8"àéèù", gd::String::npos) == gd::String::npos );

		REQUIRE( str.find_last_of(u8"àéèù") == 32 );
		REQUIRE( str.find_last_of(u8"àéèù", 32) == 32 );
		REQUIRE( str.find_last_of(u8"àéèù", 31) == 13 );
		REQUIRE( str.find_last_of(u8"àéèù", 13) == 13 );
		REQUIRE( str.find_last_of(u8"àéèù", 12) == gd::String::npos );
		REQUIRE( str.find_last_of(u8"àéèù", 0) == gd::String::npos );

		REQUIRE( str.find_first_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?") == 13 );
		REQUIRE( str.find_first_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 13) == 13 );
		REQUIRE( str.find_first_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 14) == 32 );
		REQUIRE( str.find_first_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 32) == 32 );
		REQUIRE( str.find_first_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 33) == gd::String::npos );

		REQUIRE( str.find_last_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?") == 32 );
		REQUIRE( str.find_last_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 32) == 32 );
		REQUIRE( str.find_last_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 31) == 13 );
		REQUIRE( str.find_last_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 13) == 13 );
		REQUIRE( str.find_last_not_of(u8"Aabcdefghijklmnopqrstuvwxyz- ?", 12) == gd::String::npos );
	}

	SECTION("Split") {
		//Use a "special" character as separator to test the worst case
		gd::String str = u8"Premier élémentйDeuxième élémentйTroisième élémentйDernier élément";

		std::vector<gd::String> splitted = str.Split(U'й');

		REQUIRE( splitted.size() == 4 );
		REQUIRE( splitted[0] == "Premier élément" );
		REQUIRE( splitted[1] == "Deuxième élément" );
		REQUIRE( splitted[2] == "Troisième élément" );
		REQUIRE( splitted[3] == "Dernier élément" );
	}

	SECTION("conversions from/to numbers") {
		REQUIRE( gd::String::From(-15) == "-15" );
		REQUIRE( gd::String::From<unsigned int>(15) == "15" );
		REQUIRE( gd::String::From(15.6f) == "15.6" );
		REQUIRE( gd::String::From(15.6) == "15.6" );

		REQUIRE( gd::String("-15").To<int>() == -15 );
		REQUIRE( gd::String("15").To<unsigned int>() == 15 );
		REQUIRE( gd::String("15.6").To<float>() == 15.6f );
		REQUIRE( gd::String("15.6").To<double>() == 15.6 );
	}

	SECTION("operator+=") {
		gd::String str = u8"Début d'une chaîne";
		gd::String str2 = u8", suite et fin";
		gd::String str3 = str;

		str3 += str2;
		REQUIRE( str3 == u8"Début d'une chaîne, suite et fin" );

		str3 += u8" encore un peu";
		REQUIRE( str3 == u8"Début d'une chaîne, suite et fin encore un peu" );

		str3 += U'.';
		REQUIRE( str3 == u8"Début d'une chaîne, suite et fin encore un peu." );
	}

	SECTION("push_back/pop_back") {
		gd::String str = u8"This is a sentence";

		str.push_back(U'!');
		REQUIRE( str == u8"This is a sentence!" );

		str.pop_back();
		REQUIRE( str == u8"This is a sentence" );
	}

	SECTION("case-insensitive equivalence") {
		gd::String str1 = u8"Ceci est une chaîne";
		gd::String str2 = u8"CECI est UNE CHAÎNE";
		gd::String str3 = u8"CECI est UNE CHAINE";
		gd::String str4 = "\xEF\xAC\x83"; //ffi ligature
		gd::String str5 = u8"ffi"; //ffi normal
		gd::String str6 = u8"²";
		gd::String str7 = u8"2";

		REQUIRE( gd::CaseInsensitiveEquiv(str1, str2) == true );
		REQUIRE( gd::CaseInsensitiveEquiv(str1, str3) == false );
		REQUIRE( gd::CaseInsensitiveEquiv(str4, str5) == true );
		REQUIRE( gd::CaseInsensitiveEquiv(str6, str7) == true );
		REQUIRE( gd::CaseInsensitiveEquiv(str6, str7, false) == false );
	}

	SECTION("case-insensitive find/rfind") {
		gd::String str1 = u8"Ich heiße GDevelop";
		gd::String search1 = u8"HEISSE";
		gd::String search2 = u8"heiße";
		gd::String search3 = u8"gdevelop";

		REQUIRE( str1.FindCaseInsensitive(search1) == 4 );
		REQUIRE( str1.FindCaseInsensitive(search1, 4) == 4 );
		REQUIRE( str1.FindCaseInsensitive(search1, 5) == gd::String::npos );

		REQUIRE( str1.FindCaseInsensitive(search2) == 4 );
		REQUIRE( str1.FindCaseInsensitive(search2, 4) == 4 );
		REQUIRE( str1.FindCaseInsensitive(search2, 5) == gd::String::npos );

		REQUIRE( str1.FindCaseInsensitive(search3) == 10 );
		REQUIRE( str1.FindCaseInsensitive(search3, 10) == 10 );
		REQUIRE( str1.FindCaseInsensitive(search3, 11) == gd::String::npos );

		gd::String str2 = u8"Ich heiße GDevelop ß";
		REQUIRE( str2.FindCaseInsensitive(u8"SS") == 7 );
		REQUIRE( str2.FindCaseInsensitive(u8"SS", 7) == 7 );
		REQUIRE( str2.FindCaseInsensitive(u8"SS", 8) == 19 );
	}
}

TEST_CASE( "Utf8 Grapheme clusters aware methods of gd::String", "[common][utf8][graphemeadapter]") {
	SECTION("Iterators") {
		gd::String str1 = u8"A simple string";
		gd::String str2 = u8"Les graph\x65\xCC\x80mes ont march\x65\xCC\x81 !"; //Use the decomposed graphemes "è" and "é"
		//                            |___|______|            |___|______|
		//                              e + grave accent        e + acute accent

		REQUIRE( str1.size() == 15 );
		REQUIRE( str2.size() == 28 ); //There are 28 characters but e + grave accent and e + acute accent count as 2 characters each

		REQUIRE( str2.Grph().size() == 26 ); //The count is correct with GraphemeIterators

		{
			auto it = str2.Grph().begin();
			std::advance(it, 9);
			REQUIRE( std::distance( str2.begin(), it.base() ) == 9 );
			++it;
			REQUIRE( std::distance( str2.begin(), it.base() ) == 11 );
		}
		{
			auto it = str2.Grph().begin();
			std::advance(it, 26);
			REQUIRE( it == str2.Grph().end() );
			REQUIRE( it.base() == str2.end() );

			std::advance(it, -25);
			REQUIRE( std::distance( str2.begin(), it.base() ) == 1 );
			--it;
			REQUIRE( it == str2.Grph().begin() );
			REQUIRE( it.base() == str2.begin() );
		}
		{
			auto it = str2.Grph().begin();
			REQUIRE( *it == "L" );
			++it;
			REQUIRE( *it == "e" );
			++it;
			REQUIRE( *it == "s" );
			++it;
			REQUIRE( *it == " " );
			++it;
			REQUIRE( *it == "g" );
			++it;
			REQUIRE( *it == "r" );
			++it;
			REQUIRE( *it == "a" );
			++it;
			REQUIRE( *it == "p" );
			++it;
			REQUIRE( *it == "h" );
			++it;
			REQUIRE( *it == "\x65\xCC\x80" );
			++it;
			REQUIRE( *it == "m" );
			++it;
			REQUIRE( *it == "e" );
			++it;
			REQUIRE( *it == "s" );
			++it;
			REQUIRE( *it == " " );
			++it;
			REQUIRE( *it == "o" );
			++it;
			REQUIRE( *it == "n" );
			++it;
			REQUIRE( *it == "t" );
			++it;
			REQUIRE( *it == " " );
			++it;
			REQUIRE( *it == "m" );
			++it;
			REQUIRE( *it == "a" );
			++it;
			REQUIRE( *it == "r" );
			++it;
			REQUIRE( *it == "c" );
			++it;
			REQUIRE( *it == "h" );
			++it;
			REQUIRE( *it == "\x65\xCC\x81" );
			++it;
			REQUIRE( *it == " " );
			++it;
			REQUIRE( *it == "!" );
			++it;
			REQUIRE( it == str2.Grph().end() );
		}

		REQUIRE( str2.Grph().CodepointPosFromGraphemePos(9) == 9 );
		REQUIRE( str2.Grph().GraphemePosFromCodepointPos(9) == 9 );

		REQUIRE( str2.Grph().CodepointPosFromGraphemePos(10) == 11 ); //After the è
		REQUIRE( str2.Grph().GraphemePosFromCodepointPos(11) == 10 );

		REQUIRE( str2.Grph().CodepointPosFromGraphemePos(23) == 24 ); //After the è and just before the é
		REQUIRE( str2.Grph().GraphemePosFromCodepointPos(24) == 23 );

		REQUIRE( str2.Grph().CodepointPosFromGraphemePos(24) == 26 ); //After the è and é
		REQUIRE( str2.Grph().GraphemePosFromCodepointPos(26) == 24 );

		REQUIRE( str2.Grph().CodepointPosFromGraphemePos(40) == (size_t)-1 );
		REQUIRE( str2.Grph().GraphemePosFromCodepointPos(40) == (size_t)-1 );
	}
}
