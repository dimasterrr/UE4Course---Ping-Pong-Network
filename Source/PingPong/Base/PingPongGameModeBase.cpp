#include "PingPongGameModeBase.h"

#include "PingPongPlayerController.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PingPong/Characters/PingPongPlayerPawn.h"
#include "PingPong/Objects/PingPongBall.h"
#include "PingPong/Objects/PingPongGate.h"

APingPongGameModeBase::APingPongGameModeBase()
{
	DefaultPawnClass = APingPongPlayerPawn::StaticClass();
	PlayerControllerClass = APingPongPlayerController::StaticClass();
}

void APingPongGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void APingPongGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (PlayerControllerFirst && PlayerControllerSecond)
	{
		UE_LOG(LogTemp, Error, TEXT("There are already two players in the game. New connections are not possible"));
		return;
	}

	// Refresh Available Gates
	TArray<APingPongGate*> AvailableGates;
	UpdateAvailableGates(AvailableGates);

	if (AvailableGates.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Available start point is empty, New players are not possible"));
		return;
	}

	// Initialize Player
	//// Base
	const auto TargetPlayerController = Cast<APingPongPlayerController>(NewPlayer);
	const auto TargetGate = AvailableGates.Pop();
	const auto TargetStartPoint = TargetGate->AttacheToPlayerStart;
	const auto PlayerId = FMath::Rand(); // TODO: Need Super Unique (?Random)

	//// Pawn
	const auto TargetPlayerPawn = TargetPlayerController->GetPawn()
		                              ? Cast<APingPongPlayerPawn>(TargetPlayerController->GetPawn())
		                              : GetWorld()->SpawnActor<APingPongPlayerPawn>(DefaultPawnClass);
	TargetPlayerPawn->SetActorLocation(TargetStartPoint->GetActorLocation());
	TargetPlayerPawn->SetActorRotation(TargetStartPoint->GetActorRotation());

	//// Controller
	TargetPlayerController->SetPawn(TargetPlayerPawn);
	TargetPlayerController->SetStartTransform(TargetStartPoint->GetActorTransform());
	TargetPlayerController->Server_Initialize(PlayerId, TargetGate);
	TargetPlayerController->Client_InitializeHud();

	//// Gate
	TargetGate->SetPlayerId(PlayerId);

	// Initialize Score
	// PlayersScore.Add(PlayerId, 0);
	if (!PlayerControllerFirst) PlayerControllerFirst = TargetPlayerController;
	else if (!PlayerControllerSecond) PlayerControllerSecond = TargetPlayerController;
	else UE_LOG(LogTemp, Display, TEXT("WTF!!! THIRD CONTROLLER?!?!?!?"));

	// Other
	OnPlayerEntered.Broadcast(TargetPlayerController);
	UE_LOG(LogTemp, Error, TEXT("Player is ready to play"));

	// Start Match When Player Is Ready
	if (PlayerControllerFirst && PlayerControllerSecond) OnAllPlayerIsReady();
}

void APingPongGameModeBase::UpdateAvailableGates(TArray<APingPongGate*>& AvailableGates)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APingPongGate::StaticClass(), FoundActors);

	for (const auto Item : FoundActors)
	{
		const auto TargetGates = Cast<APingPongGate>(Item);
		if (!TargetGates->IsReadyToUse()) continue;

		AvailableGates.Add(TargetGates);
	}
}

void APingPongGameModeBase::Logout(AController* Exiting)
{
	const auto TargetController = Cast<APingPongPlayerController>(Exiting);
	TargetController->Server_Finalize();

	// Remove Player
	// int32 TargetPlayerId = INDEX_NONE;
	// PlayersId.RemoveAndCopyValue(TargetController, TargetPlayerId);

	// Remove Player Score
	// PlayersScore.Remove(TargetPlayerId);

	// 
	if (PlayerControllerFirst == Exiting) PlayerControllerFirst = nullptr;
	else if (PlayerControllerSecond == Exiting) PlayerControllerSecond = nullptr;
	else UE_LOG(LogTemp, Error, TEXT("Controller not found!"));
	// Reset Score
	PlayerScoreFirst = PlayerScoreSecond = 0;

	Super::Logout(Exiting);
}

void APingPongGameModeBase::OnAllPlayerIsReady()
{
	PlayerControllerFirst->Client_UpdateWidgetWaitScreen(false);
	PlayerControllerSecond->Client_UpdateWidgetWaitScreen(false);

	SpawnBall();
}

void APingPongGameModeBase::SpawnBall()
{
	if (const auto World = GetWorld())
	{
		TArray<AActor*> TargetPoints;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, ATargetPoint::StaticClass(), BallStartPointTag,
		                                             TargetPoints);

		if (TargetPoints.IsEmpty()) return;
		const auto TargetPoint = TargetPoints.Top();
		const auto TargetPointTransform = TargetPoint->GetActorTransform();

		GetWorld()->SpawnActor<APingPongBall>(DefaultBallClass, TargetPointTransform);
	}
}

void APingPongGameModeBase::AppendPlayerGoal(int32 PlayerId)
{
	if (PlayerControllerFirst->GetPlayerId() == PlayerId)
	{
		PlayerScoreFirst++;
	}
	else if (PlayerControllerSecond->GetPlayerId() == PlayerId)
	{
		PlayerScoreSecond++;
	}

	PlayerControllerFirst->Client_UpdateWidgetScore(PlayerScoreFirst, PlayerScoreSecond);
	PlayerControllerSecond->Client_UpdateWidgetScore(PlayerScoreFirst, PlayerScoreSecond);
}
