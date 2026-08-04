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

include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/font.lua');
--include(root.BasePaths.RootProfilesDir .. '/chn/font-lb.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/titlemenu.lua');
--include(root.BasePaths.RootProfilesDir .. '/chn/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/chn/hud/tipsnotification.lua');