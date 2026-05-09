root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "ROBOTICS;NOTES DaSH";
root.WindowIconPath = "games/dash/icondata/icon.png";
root.CursorArrowPath = "games/dash/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/dash/icondata/cursor_pointer.png";

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 4,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.Dash,
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

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/charset.lua');
--include(root.BaseConfig.RootProfilesDir .. '/dash/font.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/font-lb.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/scene3d/scene3d.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/hud/tipsnotification.lua');
include(root.BaseConfig.RootProfilesDir .. '/dash/gamespecific.lua');