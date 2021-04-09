root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD LOVE CHU CHU";

root.CharaIsMvl = true;

root.Vm = {
    StartScript: 1,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.CC,
	UseReturnIds: true,
	ScrWorkChaStructSize: 40,
	ScrWorkBgStructSize: 40
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('cclcc/scriptvars.js');
include('cclcc/savedata.js');
include('cclcc/vfs.js');
include('cclcc/sprites.js');
include('common/animation.js');
include('cclcc/charset.js');
//include('cclcc/font.js');
include('cclcc/font-lb.js');
include('cclcc/dialogue.js');
include('cclcc/hud/saveicon.js');
include('cclcc/hud/loadingdisplay.js');
include('cclcc/hud/datedisplay.js');
include('cclcc/hud/titlemenu.js');
//include('cclcc/hud/systemmenu.js');
include('cclcc/hud/sysmesboxdisplay.js');
include('cclcc/hud/selectiondisplay.js');