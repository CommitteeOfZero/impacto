root.TipsNotification = {
	Type = TipsNotificationType.CCLCC,
	TextTableId = 2,
	NotificationAlertMessageId = 0,
	NotificationTextPart1MessageId = 1,
	NotificationTextPart2MessageId = 2,
	FadeInDuration = 0.5,
	FadeOutDuration = 0.5,
	NotificationBackground = "TipNotificationBackground",
	BackgroundPosition = { X = 425, Y = 39 },
	InitialNotificationPosition = { X = 1045, Y = 39 },
	NotificationRenderingBounds = { X = 323, Y = 23, Width = 690, Height = 60 },
	TimerDuration = 5,
	MoveAnimationDuration = 1,
	TipNameColorIndex = 2,
	FontSize = 26
};

root.Sprites["TipNotificationBackground"] = {
    Sheet = "Data",
    Bounds = { X = 1475, Y = 0, Width = 780, Height = 115 },
};
