root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;HEAD NOAH";
root.WindowIconPath = "games/chn/icondata/icon.png";
root.CursorArrowPath = "games/chn/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/chn/icondata/cursor_pointer.png";

root.CharaIsMvl = true;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CHN,
    UseReturnIds = true,
    UseMsbStrings = true,
    UseSeparateMsbArchive = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 20,

    MaxLinkedBgBuffers = 2,
};

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/charset.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/font.lua');
--include(root.BaseConfig.RootProfilesDir .. '/chn/font-lb.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/titlemenu.lua');
--include(root.BaseConfig.RootProfilesDir .. '/chn/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chn/hud/tipsnotification.lua');