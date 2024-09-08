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
    GameInstructionSet = InstructionSet.SG,
    UseReturnIds = true,
    ScrWorkChaStructSize = 40,
    ScrWorkBgStructSize = 40
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('sg/config.lua');
include('sg/scriptvars.lua');
include('sg/savedata.lua');
include('sg/tipssystem.lua');
include('sg/vfs.lua');
include('sg/sprites.lua');
include('common/animation.lua');
include('sg/charset.lua');
include('sg/font.lua');
include('sg/dialogue.lua');
include('sg/nametag.lua');
include('sg/hud/saveicon.lua');
include('sg/hud/loadingdisplay.lua');
include('sg/hud/datedisplay.lua');
include('sg/hud/titlemenu.lua');
include('sg/hud/systemmenu.lua');
include('sg/hud/backlogmenu.lua');
include('sg/hud/sysmesboxdisplay.lua');
include('sg/hud/selectiondisplay.lua');
include('sg/hud/tipsmenu.lua');
include('sg/hud/tipsnotification.lua');
