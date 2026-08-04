root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "STEINS;GATE";
root.WindowIconPath = "games/sgps3/icondata/icon.png";
root.CursorArrowPath = "games/sgps3/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/sgps3/icondata/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript = 2,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.SGPS3,
    UseReturnIds = false,

    ScrWorkChaStructSize = 20,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 20,
    ScrWorkBgOffsetStructSize = 10,
};

include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/font.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/titlemenu.lua');
--include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sgps3/hud/tipsnotification.lua');
