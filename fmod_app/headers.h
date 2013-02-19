#ifndef __FMODJUCE_HEADERS_H__
#define __FMODJUCE_HEADERS_H__

// Juce header
#include <juce/juce.h>

#ifdef JUCE_WINDOWS
#include <conio.h>
#define fmodassert jassert
#define _CRT_SECURE_NO_WARNINGS
#define snprintf _snprintf
#endif

#ifndef fmodassert
#include <cassert>
#define fmodassert assert
#endif

// FMOD headers

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR // not working yet for this example but could be made to..
#include <fmodiphone/api/inc/fmod.hpp>
#include <fmodiphone/api/inc/fmod_errors.h>
#include <fmodiphone/fmoddesignerapi/api/inc/fmod_event.hpp>
#else // mac or windows - the default - this is working
#include <fmod/api/inc/fmod.hpp>
#include <fmod/api/inc/fmod_errors.h>
#include <fmod/fmoddesignerapi/api/inc/fmod_event.hpp>
#endif


// set the MEDIAPATH
// - on Mac the .FEV and .FSB files can be copied in to the app bundle
// - on Windows the app just needs to know where the .FEV and .FSB are
// not that an extra slash needs to be added for FMOD to recoginse these paths but
// Juce strips the trailing slash away - so we add it manually later on
#ifdef JUCE_MAC
#include <fmod/examples/common/wincompat.h>
#define MEDIAPATH "../Resources"
#else // windows - this is not ideal - a better way would be to create a folder next to the app and copy the files in
#define MEDIAPATH "./../../../../media"
#endif


// avoid having to prefix all FMOD class names with "FMOD::"
using namespace FMOD;


// common error checking function
static void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        String error;
        error << "FMOD error! (" << result << ") " << FMOD_ErrorString(result);
        Logger::outputDebugString (error);
        fmodassert(false);
    }
}

#define FMOD_MAIN_LISTENER 0
typedef FMOD_VECTOR Vector3;
#define VECTOR3

// Some code to launch the "game" app
// it's important using this code that the game is named carefully
// e.g., if this project is called "test" the game app must be called
// "testgame.app" on Mac and "testgame.exe" on Windows
#ifdef JUCE_MAC
static void launchGame()
{
	String gameExe = String(PROJECT_NAME) + "game";
	String gameApp = gameExe + ".app";
	File gamePath = File::getSpecialLocation(File::currentExecutableFile)
                        .getParentDirectory()
                        .getChildFile(MEDIAPATH)
                        .getChildFile(gameApp);
    gamePath.startAsProcess();
}
#else // windows
static void launchGame()
{
	String gameExe = String(PROJECT_NAME) + "game";
	String gameApp = gameExe + ".exe";
	File gamePath = File::getSpecialLocation(File::currentExecutableFile)
                        .getParentDirectory()
                        .getChildFile("game")
                        .getChildFile("win")
                        .getChildFile(gameApp);
    gamePath.startAsProcess();
}
#endif

static String getResourcesPath()
{
	return File::getSpecialLocation(File::currentExecutableFile)
            .getParentDirectory()
            .getChildFile(MEDIAPATH)    // MEDIAPATH is dependent on platform
            .getFullPathName() + "/";   // the trailing slash is IMPORTANT!
}


#endif // __FMODJUCE_HEADERS_H__