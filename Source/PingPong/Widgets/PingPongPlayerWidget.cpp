#include "PingPongPlayerWidget.h"

#include "Components/TextBlock.h"

void UPingPongPlayerWidget::UpdatePlayerScore(int32 Score)
{
	if (!PlayerScoreText) return;
	PlayerScoreText->SetText(FText::FromString(FString::FromInt(Score)));
}

void UPingPongPlayerWidget::UpdateEnemyScore(int32 Score)
{
	if (!EnemyScoreText) return;
	EnemyScoreText->SetText(FText::FromString(FString::FromInt(Score)));
}

void UPingPongPlayerWidget::SetWaitScreen(bool Enable) {
	IsWaitScreenEnabled = Enable;
}