root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio |
                        GameFeature.Video | GameFeature.Subtitles | GameFeature.DebugMenu;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD Love Chu☆Chu!!";
root.WindowIconPath = "games/cclcc/icondata/icon.png";
root.CursorArrowPath = "games/cclcc/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/cclcc/icondata/cursor_pointer.png";

root.CharaIsMvl = true;
root.UseMoviePriority = true;
root.UseBgChaEffects = true;
root.UseBgFrameEffects = false;
root.UseWaveEffects = true;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CC,
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

include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/common/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');

include(root.BasePaths.RootProfilesDir .. '/cclcc/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/gamespecific.lua');
--include(root.BasePaths.RootProfilesDir .. '/cclcc/font.lua');
--include(root.BasePaths.RootProfilesDir .. '/cclcc/font-lb.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/font-lb-italic.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/configsystem.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/bgeff.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/waveeffects.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/optionsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/extramenus.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/tipsnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/savemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/cclcc/hud/helpmenu.lua');
-- include(root.BasePaths.RootProfilesDir .. '/cclcc/subtitle.lua');