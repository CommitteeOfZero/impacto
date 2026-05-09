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

include(root.BaseConfig.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/scriptinput.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/scriptvars.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/savedata.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/achievementsystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/tipssystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/vfs.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/sprites.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/animation.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/charset.lua');
include(root.BaseConfig.RootProfilesDir .. '/common/charset.lua');
if root.Language == "English" then
    include(root.BaseConfig.RootProfilesDir .. '/chlcc/font-lb.lua');
else
    include(root.BaseConfig.RootProfilesDir .. '/chlcc/font.lua');
end
include(root.BaseConfig.RootProfilesDir .. '/chlcc/dialogue.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/commonmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/saveicon.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/loadingdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/datedisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/titlemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/backlogmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/systemmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/savemenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/sysmesboxdisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/selectiondisplay.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/tipsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/tipsnotification.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/optionsmenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/extramenus.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/hud/trophymenu.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/gamespecific.lua');
include(root.BaseConfig.RootProfilesDir .. '/chlcc/waveeffects.lua');
-- include(root.BaseConfig.RootProfilesDir .. '/chlcc/subtitle.lua');