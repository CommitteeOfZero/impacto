root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "Memories Off Yubikiri no Kioku";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript: 0,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.MO7,
	UseReturnIds: true,
	ScrWorkChaStructSize: 40,
	ScrWorkBgStructSize: 40,
    MaxLinkedBgBuffers: 2
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('mo7/scriptvars.js');
include('mo7/savedata.js');
include('mo7/vfs.js');
include('mo7/sprites.js');
include('common/animation.js');
include('mo7/charset.js');
include('mo7/font.js');
include('mo7/dialogue.js');
include('mo7/hud/saveicon.js');
include('mo7/hud/loadingdisplay.js');
include('mo7/hud/datedisplay.js');
//include('mo7/hud/titlemenu.js');
//include('mo7/hud/systemmenu.js');
include('mo7/hud/sysmesboxdisplay.js');
include('mo7/hud/selectiondisplay.js');