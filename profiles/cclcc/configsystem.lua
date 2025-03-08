root.ConfigSystem = {
    ShowTipsNotification = true;
    AdvanceTextOnDirectionalInput = false;
    DirectionalInputForTrigger = false;
    TriggerStopSkip = true;

    TextSpeed = 768 / 60;
    TextSpeedBounds = { X = 256 / 60, Y = 4096 / 60 };
    AutoSpeed = 768 / 60;
    AutoSpeedBounds = { X = 2048 / 60, Y = 256 / 60 };

    SkipRead = true;
    SyncVoice = true;
    SkipVoice = false;

    VoiceCount = 13;
    VoiceMuted = {};
    VoiceVolume = {};
};

for i = 1, root.ConfigSystem.VoiceCount do
    root.ConfigSystem.VoiceMuted[i] = false;
    root.ConfigSystem.VoiceVolume[i] = 1;
end