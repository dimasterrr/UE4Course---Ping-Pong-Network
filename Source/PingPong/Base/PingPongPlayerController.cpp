#include "PingPongPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "PingPong/Objects/PingPongPlatform.h"
#include "PingPong/Widgets/PingPongPlayerWidget.h"

APingPongPlayerController::APingPongPlayerController()
{
	SetReplicates(true);
}

void APingPongPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveRight", this, &APingPongPlayerController::MoveRight);
}

void APingPongPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APingPongPlayerController, PlayerId, COND_SimulatedOnly);
}

void APingPongPlayerController::SetStartTransform(const FTransform& Transform)
{
	StartPointTransform = Transform;
}

void APingPongPlayerController::SetPlatformSkin(FSoftObjectPath SkinAsset)
{
	Server_SetPlatformSkin(SkinAsset);
}

void APingPongPlayerController::MoveRight(float AxisValue)
{
	Server_MoveRight(AxisValue);
}

void APingPongPlayerController::Server_MoveRight_Implementation(float AxisValue)
{
	if (!PlatformReference)
	{
		UE_LOG(LogTemp, Error, TEXT("Platform doesn't exist"));
		return;
	}

	PlatformReference->Server_MoveRight(AxisValue);
}

bool APingPongPlayerController::Server_MoveRight_Validate(float AxisValue)
{
	return true;
}

void APingPongPlayerController::Server_Initialize_Implementation(int32 NewPlayerId, APingPongGate* NewPlayerGate)
{
	if (PlatformReference) PlatformReference->Destroy();
	Server_SpawnPlatform(PlatformClass);

	PlayerId = NewPlayerId;
	GateReference = NewPlayerGate;
}

bool APingPongPlayerController::Server_Initialize_Validate(int32 NewPlayerId, APingPongGate* NewPlayerGate)
{
	return true;
	return NewPlayerId && NewPlayerGate;
}

void APingPongPlayerController::Server_Finalize_Implementation()
{
	if (PlatformReference) PlatformReference->Destroy();

	PlayerId = INDEX_NONE;
	GateReference = nullptr;
	PlatformReference = nullptr;
}

bool APingPongPlayerController::Server_Finalize_Validate()
{
	return true;
}

void APingPongPlayerController::Server_SpawnPlatform_Implementation(TSubclassOf<APingPongPlatform> Class)
{
	PlatformReference = GetWorld()->SpawnActor<APingPongPlatform>(PlatformClass);
	if (!PlatformReference)
	{
		UE_LOG(LogTemp, Error, TEXT("Platform dont spawned"));
		return;
	}

	PlatformReference->SetActorLocation(StartPointTransform.GetLocation());
	PlatformReference->SetActorRotation(StartPointTransform.GetRotation());
}

bool APingPongPlayerController::Server_SpawnPlatform_Validate(TSubclassOf<APingPongPlatform> Class)
{
	return true;
	return !!PlatformClass;
}

void APingPongPlayerController::Server_SetPlatformSkin_Implementation(FSoftObjectPath SkinAsset)
{
	if (!SkinAsset.IsValid()) return;

	if (const auto SkinMaterial = Cast<UMaterialInterface>(SkinAsset.TryLoad()); SkinMaterial && PlatformReference)
	{
		PlatformReference->SetSkin(SkinMaterial);
	}
}

void APingPongPlayerController::Client_InitializeHud_Implementation()
{
	if (WidgetReference) return;
	WidgetReference = CreateWidget<UPingPongPlayerWidget>(this, WidgetClass);

	if (WidgetReference)
	{
		WidgetReference->UpdatePlayerScore(0);
		WidgetReference->UpdateEnemyScore(0);
		WidgetReference->SetWaitScreen(true);
		WidgetReference->AddToViewport();
	}
}

bool APingPongPlayerController::Client_InitializeHud_Validate()
{
	return true;
	return !!WidgetClass;
}

void APingPongPlayerController::Client_UpdateWidgetScore_Implementation(int32 PlayerScore, int32 EnemyScore)
{
	WidgetReference->UpdatePlayerScore(PlayerScore);
	WidgetReference->UpdateEnemyScore(EnemyScore);
}

void APingPongPlayerController::Client_UpdateWidgetWaitScreen_Implementation(bool Enable)
{
	WidgetReference->SetWaitScreen(Enable);
}

int32 APingPongPlayerController::GetPlayerId() const
{
	return PlayerId;
}
