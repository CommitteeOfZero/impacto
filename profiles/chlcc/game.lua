root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | 
    GameFeature.Audio | GameFeature.Video |  GameFeature.Subtitles | GameFeature.DebugMenu;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "CHAOS;HEAD Love Chu☆Chu!";
root.WindowIconPath = "games/chlcc/icondata/icon.png";
root.CursorArrowPath = "games/chlcc/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/chlcc/icondata/cursor_pointer.png";

root.CharaIsMvl = false;
root.UseMoviePriority = true;
root.UseWaveEffects = true;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 0,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CHLCC,
    RestartMaskUsesThreadAlpha = true,
    UseReturnIds = false,

    ScrWorkChaStructSize = 20,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 20,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 2,
};

include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/achievementsystem.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/common/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/font.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/commonmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/savemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/tipsnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/optionsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/extramenus.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/hud/trophymenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/gamespecific.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc/waveeffects.lua');
