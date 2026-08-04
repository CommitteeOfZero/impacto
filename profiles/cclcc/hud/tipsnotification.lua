root.TipsNotification = {
    Type = TipsNotificationType.CCLCC,
    TextTableId = 0,
    NotificationTextPart1MessageId = 100,
    NotificationTextPart2MessageId = 101,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.5,
    NotificationBackground = "TipNotificationBackground",
    BackgroundPositionX = 571,
    BackgroundPositionYOffset = 55,
    NotificationPositionX = 600,
    NotificationPositionYOffset = 10,
    TimerDuration = 2,
    MoveAnimationDuration = 1,
    FontSize = 26,
    TipNameColor = { 0x06a4ff, 0x035280 },
    NotificationTextColor = { 0x080808, 0x000000 },
};

root.Sprites["TipNotificationBackground"] = {
    Sheet = "Data",
    Bounds = { X = 1459, Y = 1, Width = 817, Height = 120 },
};
