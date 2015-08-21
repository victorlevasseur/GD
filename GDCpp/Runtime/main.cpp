/*
 * GDevelop C++ Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */

#include <vector>
#include <string>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#if defined(__GNUC__)
#include <unistd.h>
#endif

#include "GDCpp/CommonTools.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/ResourcesLoader.h"
#include "GDCpp/FontManager.h"
#include "GDCpp/SoundManager.h"
#include "GDCpp/SceneNameMangler.h"
#include "GDCpp/Project.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/CodeExecutionEngine.h"
#include "GDCpp/CppPlatform.h"
#include "GDCpp/ExtensionsLoader.h"
#include "GDCpp/Log.h"
#include "GDCpp/SceneStack.h"
#include "GDCpp/Tools/AES.h"
#include "GDCpp/Serialization/Serializer.h"
#include "GDCpp/Serialization/SerializerElement.h"
#include "GDCpp/tinyxml/tinyxml.h"
#include "GDCpp/RuntimeGame.h"
#include "CompilationChecker.h"

#include <stdlib.h>
#include <stdio.h>

using namespace std;

gd::String GetCurrentWorkingDirectory();
int DisplayMessage(const gd::String & message);

#if defined(WINDOWS)
#include <windows.h>
//On Windows computers, tells the Nvidia/AMD driver that GDevelop works better
//with the more powerful discrete GPU (e.g. use the Nvidia card on an Optimus computer)
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main( int argc, char *p_argv[] )
{
    GDLogBanner();

    //Get executable location
    gd::String fullExecutablePath;
    if ( *p_argv[0] != '/' )
    {
        fullExecutablePath += GetCurrentWorkingDirectory();
        fullExecutablePath += "/";
    }
    #ifndef WINDOWS
    fullExecutablePath += p_argv[0];
    #endif
    gd::String executablePath = fullExecutablePath.substr( 0, fullExecutablePath.find_last_of( "/" ) );
    gd::String executableFilename = fullExecutablePath.find_last_of( "/" ) < fullExecutablePath.length() ? fullExecutablePath.substr( fullExecutablePath.find_last_of( "/" ), fullExecutablePath.length() ) : "";
    gd::String executableNameOnly = executableFilename.substr(0, executableFilename.length()-4);

    #ifdef WINDOWS
        gd::String codeFileExtension = "dll";
    #elif defined(LINUX)
        gd::String codeFileExtension = "so";
        chdir( executablePath.c_str() ); //For linux, make the executable dir the current working directory
    #elif defined(MACOS)
        gd::String codeFileExtension = "dylib";
    #else
        #error Please update this part to support your target system.
    #endif

    //Check GDCpp version
    CompilationChecker::EnsureCorrectGDVersion();

    //Load extensions
    gd::ExtensionsLoader::LoadAllExtensions(".", CppPlatform::Get());
    gd::ExtensionsLoader::ExtensionsLoadingDone(".");
    //Load resource file
    gd::ResourcesLoader * resLoader = gd::ResourcesLoader::Get();
    if (!resLoader->SetResourceFile( executablePath+"/"+executableNameOnly+".egd" )
           && !resLoader->SetResourceFile( executableNameOnly+".egd" )
           && !resLoader->SetResourceFile( executablePath+"/gam.egd" )
           && !resLoader->SetResourceFile( "gam.egd" ) )
    {
        return DisplayMessage("Unable to load resources. Aborting.");
    }

    gd::Project game;

    //Load game data
    {
        cout << "Getting src file size..." << endl;
        int fsize = resLoader->GetBinaryFileSize( "src" );

        // round up (ignore pad for here)
        int size = (fsize+15)&(~15);

        cout << "Getting src raw data..." << endl;
        char * ibuffer = resLoader->LoadBinaryFile( "src" );
        char * obuffer = new char[size];

        unsigned char key[] = "-P:j$4t&OHIUVM/Z+u4DeDP.";
        const unsigned char iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

        aes_ks_t keySetting;
        aes_setks_decrypt(key, 192, &keySetting);
        aes_cbc_decrypt(reinterpret_cast<const unsigned char*>(ibuffer), reinterpret_cast<unsigned char*>(obuffer),
            (uint8_t*)iv, size/AES_BLOCK_SIZE, &keySetting);

        std::string uncryptedSrc = std::string(obuffer, size);
        delete [] obuffer;

        cout << "Loading game data..." << endl;
        TiXmlDocument doc;
        if ( !doc.Parse(uncryptedSrc.c_str()) )
        {
            return DisplayMessage("Unable to parse game data. Aborting.");
        }

        TiXmlHandle hdl(&doc);
        gd::SerializerElement rootElement;
        gd::Serializer::FromXML(rootElement, hdl.FirstChildElement().Element());
        game.UnserializeFrom(rootElement);
	}

    if ( game.GetLayoutsCount() == 0 )
        return DisplayMessage("No scene to be loaded. Aborting.");

    //Loading the code
    gd::String codeLibraryName = executablePath+"/"+executableNameOnly+"."+codeFileExtension;
    Handle codeLibrary = gd::OpenLibrary(codeLibraryName.ToLocale().c_str());
    if ( codeLibrary == NULL )
    {
        codeLibraryName = executablePath+"/Code."+codeFileExtension;
        Handle codeLibrary = gd::OpenLibrary(codeLibraryName.ToLocale().c_str());
        if ( codeLibrary == NULL )
        {
            return DisplayMessage("Unable to load the execution engine for game. Aborting.");
        }
    }

    #if defined(WINDOWS)
    //Handle special argument to change working directory
    if ( argc >= 2 && gd::String(p_argv[1]).size() > 5 && gd::String(p_argv[1]).substr(0, 5) == "-cwd=" )
    {
        gd::String newWorkingDir = gd::String(p_argv[1]).substr(5, gd::String::npos);
        cout << "Changing working directory to " << newWorkingDir << endl;
        chdir(newWorkingDir.ToLocale().c_str());
    }
    #endif

    //Initialize image manager and load always loaded images
    game.GetImageManager()->LoadPermanentImages();

    //Create main window
    sf::RenderWindow window;

    RuntimeGame runtimeGame;
    runtimeGame.LoadFromProject(game);

    window.create(sf::VideoMode(game.GetMainWindowDefaultWidth(), game.GetMainWindowDefaultHeight(), 32),
        "", sf::Style::Close);
    window.setActive(true);
    window.setFramerateLimit(game.GetMaximumFPS());
    window.setVerticalSyncEnabled(game.IsVerticalSynchronizationEnabledByDefault());

    //Game main loop
    bool abort = false;
    SceneStack sceneStack(runtimeGame, &window, codeLibraryName);
    sceneStack.OnError([&abort](gd::String error) {
        DisplayMessage(error);
        abort = true;
    });

    sceneStack.Push(game.GetLayout(0).GetName());
    while (sceneStack.Step() && !abort)
        ;

    SoundManager::Get()->DestroySingleton();
    FontManager::Get()->DestroySingleton();

    gd::CloseLibrary(codeLibrary);

    return EXIT_SUCCESS;
}

/**
 * Retrieve current working directory
 */
gd::String GetCurrentWorkingDirectory()
{
    char path[2048];
    getcwd(path, 2048);

    return path;
}

#if defined(WINDOWS)
#include <windows.h>
#include <Commdlg.h>
#endif
int DisplayMessage(const gd::String & message)
{
    std::cout << message;
    #if defined(WINDOWS)
    MessageBoxW(NULL, message.ToWide().c_str(), L"Fatal error", MB_ICONERROR);
    #endif
    return EXIT_FAILURE;
}
