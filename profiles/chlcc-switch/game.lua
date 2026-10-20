root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine |
                    GameFeature.Renderer2D |
                    GameFeature.Input |
                    GameFeature.Audio |
                    GameFeature.Video |
                    GameFeature.Subtitles |
                    GameFeature.DebugMenu |
                    GameFeature.Achievements |
                    GameFeature.Overlay;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;HEAD Love Chu☆Chu! (Switch)";
root.WindowIconPath = "resources/chlcc/icondata/icon.png";
root.CursorArrowPath = "resources/chlcc/icondata/cursor_arrow.png";
root.CursorPointerPath = "resources/chlcc/icondata/cursor_pointer.png";

root.CharaIsMvl = true;
root.UseMoviePriority = true;
root.UseWaveEffects = true;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.LCCSwitch,
    RestartMaskUsesThreadAlpha = true,
    UseReturnIds = true,
    UseMsbStrings = true,
    UseSeparateMsbArchive = true,
    StringEncodingType = StringUnitEncoding.Uint32,
    StringIdSize = 4,

    ScrWorkChaStructSize = 24,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 24,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 18,
    ScrWorkMesStructSize = 7,
    MaxLinkedBgBuffers = 2,
};

root.PlatformId = 0x100000;

include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/common/achievementnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/common/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');

include(root.BasePaths.RootProfilesDir .. '/chlcc/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/scriptvars.lua');

root.Sprites = {};
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/gamespecific.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/font.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/font.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/waveeffects.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/commonmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/savemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/tipsnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/optionsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/extramenus.lua');
