root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video | GameFeature.DebugMenu;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "Memories Off -Innocent Fille-";
root.WindowIconPath = "games/mo8/icondata/icon.png";
root.CursorArrowPath = "games/mo8/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/mo8/icondata/cursor_pointer.png";

root.CharaIsMvl = true;
root.UseMoviePriority = true;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 7,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.MO8,
    UseReturnIds = true,
    UseMsbStrings = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,

    MaxLinkedBgBuffers = 2
};

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/charset.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/font.lua');
--include(root.BaseConfig.RootProfilesDir .. '/mo8/font-lb.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/optionsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/savemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo8/hud/tipsnotification.lua');