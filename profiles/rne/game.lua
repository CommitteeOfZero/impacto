root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "ROBOTICS;NOTES ELITE";
root.WindowIconPath = "games/rne/icondata/icon.png";
root.CursorArrowPath = "games/rne/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/rne/icondata/cursor_pointer.png";

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 4,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.RNE,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,

    MaxLinkedBgBuffers = 2
};

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua')
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/charset.lua');
--include(root.BaseConfig.RootProfilesDir .. '/rne/font.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/font-lb.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/scene3d/scene3d.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/hud/tipsnotification.lua');
include(root.BaseConfig.RootProfilesDir .. '/rne/gamespecific.lua');