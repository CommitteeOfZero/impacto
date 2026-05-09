root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video | GameFeature.DebugMenu;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "Memories Off 6 ~T-Wave~";
root.WindowIconPath = "games/mo6tw/icondata/icon.png";
root.CursorArrowPath = "games/mo6tw/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/mo6tw/icondata/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript=0,
    StartScriptBuffer=0,
    GameInstructionSet=InstructionSet.MO6TW,
    UseReturnIds=false,
    RestartMaskUsesThreadAlpha=true,

    ScrWorkChaStructSize = 20,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 20,
    ScrWorkBgOffsetStructSize = 10,
};

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/charset.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/font.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/savemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/optionsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/extramenus.lua');
include(root.BaseConfig.RootProfilesDir .. '/mo6tw/hud/tipsnotification.lua');
