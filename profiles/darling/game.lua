root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "STEINS;GATE: My Darling's Embrace";
root.WindowIconPath = "games/darling/icondata/icon.png";
root.CursorArrowPath = "games/darling/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/darling/icondata/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 2,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.Darling,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,
};

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/charset.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/font.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/backlogmenu.lua');
--include(root.BaseConfig.RootProfilesDir .. '/darling/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/darling/hud/tipsnotification.lua');