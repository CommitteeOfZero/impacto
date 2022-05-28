root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD Love Chu☆Chu!!";

root.CharaIsMvl = true;

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
include('cclcc/scriptvars.js');
include('cclcc/savedata.js');
include('cclcc/tipssystem.js');
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
include('cclcc/hud/backlogmenu.js');
include('cclcc/hud/sysmesboxdisplay.js');
include('cclcc/hud/selectiondisplay.js');
include('cclcc/hud/optionsmenu.js');
include('cclcc/hud/tipsmenu.js');
include('cclcc/hud/extramenus.js');
include('cclcc/hud/tipsnotification.js');