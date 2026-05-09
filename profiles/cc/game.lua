root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD";
root.WindowIconPath = "games/cc/icondata/icon.png";
root.CursorArrowPath = "games/cc/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/cc/icondata/cursor_pointer.png";
root.UseMoviePriority = true;
root.UseWaveEffects = true;

root.CharaIsMvl = false;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;

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

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/charset.lua');
--include(root.BaseConfig.RootProfilesDir .. '/cc/font.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/font-lb.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/waveeffects.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/titlemenu.lua');
--include(root.BaseConfig.RootProfilesDir .. '/cc/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/cc/hud/tipsnotification.lua');
