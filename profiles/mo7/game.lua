root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "Memories Off Yubikiri no Kioku";
root.WindowIconPath = "games/mo7/icondata/icon.png";
root.CursorArrowPath = "games/mo7/icondata/cursor_arrow.png";
root.CursorPointerPath = "games/mo7/icondata/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 0,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.MO7,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,

    MaxLinkedBgBuffers = 2
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('mo7/config.lua');
include('mo7/scriptvars.lua');
include('mo7/savedata.lua');
include('mo7/tipssystem.lua');
include('mo7/vfs.lua');
include('mo7/sprites.lua');
include('common/animation.lua');
include('mo7/charset.lua');
include('mo7/font.lua');
include('mo7/dialogue.lua');
include('mo7/hud/saveicon.lua');
include('mo7/hud/loadingdisplay.lua');
include('mo7/hud/datedisplay.lua');
--include('mo7/hud/titlemenu.lua');
--include('mo7/hud/systemmenu.lua');
include('mo7/hud/backlogmenu.lua');
include('mo7/hud/sysmesboxdisplay.lua');
include('mo7/hud/selectiondisplay.lua');
include('mo7/hud/tipsmenu.lua');
include('mo7/hud/tipsnotification.lua');