root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.Vm = {
    StartScript: 4,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.RNE,
	UseReturnIds: true
};

include('rne/vfs.js');
include('rne/sprites.js');
include('common/animation.js');
include('rne/charset.js');
//include('rne/font.js');
include('rne/font-lb.js');
include('rne/dialogue.js');
include('rne/hud/saveicon.js');
include('rne/hud/loadingdisplay.js');
include('rne/hud/datedisplay.js');
include('rne/hud/titlemenu.js');
include('rne/hud/mainmenu.js');
include('rne/hud/sysmesboxdisplay.js');
include('rne/scene3d/scene3d.js');