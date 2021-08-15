root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "Memories Off -Innocent Fille-";

root.CharaIsMvl = true;

root.Vm = {
    StartScript: 7,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.MO8,
    UseReturnIds: true,
    UseMsbStrings: true,
    ScrWorkChaStructSize: 40,
    ScrWorkBgStructSize: 40,
    MaxLinkedBgBuffers: 2
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('mo8/scriptvars.js');
include('mo8/savedata.js');
include('mo8/vfs.js');
include('mo8/sprites.js');
include('common/animation.js');
include('mo8/charset.js');
include('mo8/font.js');
//include('mo8/font-lb.js');
include('mo8/dialogue.js');
include('mo8/hud/saveicon.js');
include('mo8/hud/loadingdisplay.js');
include('mo8/hud/datedisplay.js');
include('mo8/hud/titlemenu.js');
//include('mo8/hud/systemmenu.js');
include('mo8/hud/sysmesboxdisplay.js');
include('mo8/hud/selectiondisplay.js');