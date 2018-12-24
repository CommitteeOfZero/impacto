root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.Vm = {
    StartScript: 0,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.CHLCC,
	UseReturnIds: false
};

include('chlcc/vfs.js');
include('chlcc/sprites.js');
include('common/animation.js');
include('chlcc/charset.js');
include('chlcc/font.js');
include('chlcc/dialogue.js');
include('chlcc/hud/saveicon.js');
include('chlcc/hud/loadingdisplay.js');
include('chlcc/hud/datedisplay.js');
//include('chlcc/hud/titlemenu.js');
//include('chlcc/hud/mainmenu.js');
include('chlcc/hud/sysmesboxdisplay.js');