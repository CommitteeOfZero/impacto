root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio |
GameFeature.Video | GameFeature.DebugMenu;
-- NOTE: 1080p messes up rendering of "loading system data..." message,
-- but is probably correct because otherwise logo slideshow has wrong size.
-- Probably a scaling factor somewhere needs to be adjusted for the message.
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "STEINS;GATE";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;
root.UseScreenCapEffects = false;

root.Vm = {
    StartScript = 2, -- _STARTUP_WIN.SCX
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.SGHD,
    UseReturnIds = true,
    ScrWorkChaStructSize = 40,
    ScrWorkBgStructSize = 40
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('sghd/config.lua');
include('sghd/scriptvars.lua');
include('sghd/savedata.lua');
include('sghd/tipssystem.lua');
include('sghd/vfs.lua');
include('sghd/sprites.lua');
include('common/animation.lua');
include('sghd/charset.lua');
include('sghd/font.lua');
include('sghd/dialogue.lua');
include('sghd/nametag.lua');
include('sghd/hud/saveicon.lua');
include('sghd/hud/loadingdisplay.lua');
include('sghd/hud/datedisplay.lua');
include('sghd/hud/titlemenu.lua');
include('sghd/hud/systemmenu.lua');
include('sghd/hud/backlogmenu.lua');
include('sghd/hud/sysmesboxdisplay.lua');
include('sghd/hud/selectiondisplay.lua');
include('sghd/hud/tipsmenu.lua');
include('sghd/hud/tipsnotification.lua');
