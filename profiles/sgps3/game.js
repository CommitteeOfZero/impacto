root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "STEINS;GATE";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;
root.UseScreenCapEffects = false;

root.Vm = {
    StartScript: 2,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.SGPS3,
	UseReturnIds: false,
	ScrWorkChaStructSize: 20,
	ScrWorkBgStructSize: 20
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('sgps3/scriptvars.js');
include('sgps3/savedata.js');
include('sgps3/vfs.js');
include('sgps3/sprites.js');
include('common/animation.js');
include('sgps3/charset.js');
include('sgps3/font.js');
include('sgps3/dialogue.js');
include('sgps3/hud/saveicon.js');
include('sgps3/hud/loadingdisplay.js');
include('sgps3/hud/datedisplay.js');
include('sgps3/hud/titlemenu.js');
//include('sgps3/hud/systemmenu.js');
include('sgps3/hud/backlogmenu.js');
include('sgps3/hud/sysmesboxdisplay.js');
include('sgps3/hud/selectiondisplay.js');