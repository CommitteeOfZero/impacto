root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.Vm = {
    StartScript: 4,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.Dash,
    UseReturnIds: true
};

include('common/scriptvars.js');
include('dash/scriptvars.js');
include('dash/vfs.js');
include('dash/sprites.js');
include('common/animation.js');
include('dash/charset.js');
//include('dash/font.js');
include('dash/font-lb.js');
include('dash/dialogue.js');
include('dash/hud/saveicon.js');
include('dash/hud/loadingdisplay.js');
include('dash/hud/datedisplay.js');
include('dash/hud/titlemenu.js');
include('dash/hud/mainmenu.js');
include('dash/hud/sysmesboxdisplay.js');
include('dash/scene3d/scene3d.js');