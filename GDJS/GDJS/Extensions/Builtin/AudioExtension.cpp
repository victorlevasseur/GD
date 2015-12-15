/*
 * GDevelop JS Platform
 * Copyright 2008-2015 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#include "AudioExtension.h"
#include "GDCore/Extensions/Builtin/AllBuiltinExtensions.h"
#include "GDCore/IDE/Project/ArbitraryResourceWorker.h"
#include "GDCore/Events/CodeGeneration/EventsCodeGenerator.h"
#include "GDCore/CommonTools.h"
#include "GDCore/Tools/Localization.h"

namespace gdjs
{

AudioExtension::AudioExtension()
{
    gd::BuiltinExtensionsImplementer::ImplementsAudioExtension(*this);

    SetExtensionInformation("BuiltinAudio",
                          _("Audio"),
                          _("Builtin audio extension"),
                          "Florian Rival",
                          "Open source (MIT License)");

    GetAllActions()["PlaySound"].SetFunctionName("gdjs.evtTools.sound.playSound");
    GetAllActions()["PlaySoundCanal"].SetFunctionName("gdjs.evtTools.sound.playSoundOnChannel");
    GetAllActions()["PlayMusic"].SetFunctionName("gdjs.evtTools.sound.playMusic");
    GetAllActions()["PlayMusicCanal"].SetFunctionName("gdjs.evtTools.sound.playMusicOnChannel");
    GetAllActions()["StopSoundCanal"].SetFunctionName("gdjs.evtTools.sound.stopSoundOnChannel");
    GetAllActions()["PauseSoundCanal"].SetFunctionName("gdjs.evtTools.sound.pauseSoundOnChannel");
    GetAllActions()["RePlaySoundCanal"].SetFunctionName("gdjs.evtTools.sound.continueSoundOnChannel");
    GetAllActions()["StopMusicCanal"].SetFunctionName("gdjs.evtTools.sound.stopMusicOnChannel");
    GetAllActions()["PauseMusicCanal"].SetFunctionName("gdjs.evtTools.sound.pauseMusicOnChannel");
    GetAllActions()["RePlayMusicCanal"].SetFunctionName("gdjs.evtTools.sound.continueMusicOnChannel");
    GetAllConditions()["MusicPlaying"].SetFunctionName("gdjs.evtTools.sound.isMusicOnChannelPlaying");
    GetAllConditions()["MusicPaused"].SetFunctionName("gdjs.evtTools.sound.isMusicOnChannelPaused");
    GetAllConditions()["MusicStopped"].SetFunctionName("gdjs.evtTools.sound.isMusicOnChannelStopped");
    GetAllConditions()["SoundPlaying"].SetFunctionName("gdjs.evtTools.sound.isSoundOnChannelPlaying");
    GetAllConditions()["SoundPaused"].SetFunctionName("gdjs.evtTools.sound.isSoundOnChannelPaused");
    GetAllConditions()["SoundStopped"].SetFunctionName("gdjs.evtTools.sound.isSoundOnChannelStopped");

    GetAllActions()["ModGlobalVolume"].SetFunctionName("gdjs.evtTools.sound.setGlobalVolume").SetGetter("gdjs.evtTools.sound.getGlobalVolume");
    GetAllConditions()["GlobalVolume"].SetFunctionName("gdjs.evtTools.sound.getGlobalVolume");
    GetAllExpressions()["GlobalVolume"].SetFunctionName("gdjs.evtTools.sound.getGlobalVolume");

    GetAllActions()["ModVolumeSoundCanal"].SetFunctionName("gdjs.evtTools.sound.setSoundOnChannelVolume").SetGetter("gdjs.evtTools.sound.getSoundOnChannelVolume");
    GetAllActions()["ModVolumeMusicCanal"].SetFunctionName("gdjs.evtTools.sound.setMusicOnChannelVolume").SetGetter("gdjs.evtTools.sound.getMusicOnChannelVolume");
    GetAllConditions()["SoundCanalVolume"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelVolume");
    GetAllConditions()["MusicCanalVolume"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelVolume");

    GetAllActions()["ModPlayingOffsetSoundChannel"].SetFunctionName("gdjs.evtTools.sound.setSoundOnChannelPlayingOffset").SetGetter("gdjs.evtTools.sound.getSoundOnChannelPlayingOffset");
    GetAllActions()["ModPlayingOffsetMusicChannel"].SetFunctionName("gdjs.evtTools.sound.setMusicOnChannelPlayingOffset").SetGetter("gdjs.evtTools.sound.getMusicOnChannelPlayingOffset");
    GetAllConditions()["SoundChannelPlayingOffset"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelPlayingOffset");
    GetAllConditions()["MusicChannelPlayingOffset"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelPlayingOffset");

    GetAllConditions()["SoundChannelPitch"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelPitch");
    GetAllConditions()["MusicChannelPitch"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelPitch");

    GetAllExpressions()["SoundChannelVolume"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelVolume");
    GetAllExpressions()["MusicChannelVolume"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelVolume");

    GetAllExpressions()["SoundChannelPlayingOffset"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelPlayingOffset");
    GetAllExpressions()["MusicChannelPlayingOffset"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelPlayingOffset");

    GetAllExpressions()["SoundChannelPitch"].SetFunctionName("gdjs.evtTools.sound.getSoundOnChannelPitch");
    GetAllExpressions()["MusicChannelPitch"].SetFunctionName("gdjs.evtTools.sound.getMusicOnChannelPitch");

    StripUnimplementedInstructionsAndExpressions();
}

void AudioExtension::ExposeActionsResources(gd::Instruction & action, gd::ArbitraryResourceWorker & worker)
{
    if ( action.GetType() == "PlaySound" || action.GetType() == "PlaySoundCanal" || action.GetType() == "PlayMusic" || action.GetType() == "PlayMusicCanal" )
    {
        gd::String parameter = action.GetParameter(1).GetPlainString();
        worker.ExposeAudio(parameter);
        action.SetParameter(1, parameter);
    }
}

}
