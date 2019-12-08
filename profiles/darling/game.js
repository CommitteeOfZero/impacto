root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript: 2,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.Darling,
	UseReturnIds: true,
	ScrWorkChaStructSize: 40,
	ScrWorkBgStructSize: 40
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('darling/scriptvars.js');
include('darling/savedata.js');
include('darling/vfs.js');
include('darling/sprites.js');
include('common/animation.js');
include('darling/charset.js');
include('darling/font.js');
include('darling/dialogue.js');
include('darling/hud/saveicon.js');
include('darling/hud/loadingdisplay.js');
include('darling/hud/datedisplay.js');
//include('darling/hud/titlemenu.js');
//include('darling/hud/mainmenu.js');
include('darling/hud/sysmesboxdisplay.js');
include('dash/hud/selectiondisplay.js');