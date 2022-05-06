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
    StartScript: 1,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.CC,
	UseReturnIds: true,
	ScrWorkChaStructSize: 40,
	ScrWorkBgStructSize: 40,
    MaxLinkedBgBuffers: 2
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('cc/scriptvars.js');
include('cc/savedata.js');
include('cc/tipssystem.js');
include('cc/vfs.js');
include('cc/sprites.js');
include('common/animation.js');
include('cc/charset.js');
//include('cc/font.js');
include('cc/font-lb.js');
include('cc/dialogue.js');
include('cc/hud/saveicon.js');
include('cc/hud/loadingdisplay.js');
include('cc/hud/datedisplay.js');
include('cc/hud/titlemenu.js');
//include('cc/hud/systemmenu.js');
include('cc/hud/backlogmenu.js');
include('cc/hud/sysmesboxdisplay.js');
include('cc/hud/selectiondisplay.js');
include('cc/hud/tipsmenu.js');
include('cc/hud/tipsnotification.js');