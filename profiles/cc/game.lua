root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD";

root.CharaIsMvl = false;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CC,
	UseReturnIds = true,
	ScrWorkChaStructSize = 40,
	ScrWorkBgStructSize = 40,
    MaxLinkedBgBuffers = 2
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('cc/scriptvars.lua');
include('cc/savedata.lua');
include('cc/tipssystem.lua');
include('cc/vfs.lua');
include('cc/sprites.lua');
include('common/animation.lua');
include('cc/charset.lua');
--include('cc/font.lua');
include('cc/font-lb.lua');
include('cc/dialogue.lua');
include('cc/hud/saveicon.lua');
include('cc/hud/loadingdisplay.lua');
include('cc/hud/datedisplay.lua');
include('cc/hud/titlemenu.lua');
--include('cc/hud/systemmenu.lua');
include('cc/hud/backlogmenu.lua');
include('cc/hud/sysmesboxdisplay.lua');
include('cc/hud/selectiondisplay.lua');
include('cc/hud/tipsmenu.lua');
include('cc/hud/tipsnotification.lua');