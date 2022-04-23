root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "CHAOS;HEAD Love Chu☆Chu!";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript: 0,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.CHLCC,
    UseReturnIds: false,
    ScrWorkChaStructSize: 20,
    ScrWorkBgStructSize: 20
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('chlcc/scriptvars.js');
include('chlcc/savedata.js');
include('chlcc/tipssystem.js');
include('chlcc/vfs.js');
include('chlcc/sprites.js');
include('common/animation.js');
include('chlcc/charset.js');
//include('chlcc/font.js');
include('chlcc/font-lb.js');
include('chlcc/dialogue.js');
include('chlcc/hud/saveicon.js');
include('chlcc/hud/loadingdisplay.js');
include('chlcc/hud/datedisplay.js');
include('chlcc/hud/titlemenu.js');
include('chlcc/hud/backlogmenu.js');
//include('chlcc/hud/systemmenu.js');
include('chlcc/hud/savemenu.js');
include('chlcc/hud/sysmesboxdisplay.js');
include('chlcc/hud/selectiondisplay.js');
include('chlcc/hud/tipsmenu.js');