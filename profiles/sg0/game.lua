

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "STEINS;GATE 0";
root.WindowIconPath = "resources/cc/icondata/icon.png";
root.CursorArrowPath = "resources/cc/icondata/cursor_arrow.png";
root.CursorPointerPath = "resources/cc/icondata/cursor_pointer.png";
root.UseMoviePriority = true;
root.UseWaveEffects = true;

root.CharaIsMvl = false;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 6, -- _Startup_win.scx
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.SG0,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 20,

    MaxLinkedBgBuffers = 2
};

root.PlatformId = 131072;

include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/common/achievementnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/font.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/waveeffects.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/tipsnotification.lua');
