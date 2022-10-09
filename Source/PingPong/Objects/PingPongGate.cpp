#include "PingPongGate.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PingPong/Base/PingPongGameModeBase.h"

APingPongGate::APingPongGate()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetGenerateOverlapEvents(true);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(MeshComponent);
	CollisionComponent->SetIsReplicated(true);

	SetReplicates(true);
}

void APingPongGate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APingPongGate::Server_SetGoal_Implementation()
{
	if (const auto World = GetWorld();
		const auto GameMode = Cast<APingPongGameModeBase>(UGameplayStatics::GetGameMode(World)))
	{
		GameMode->AppendPlayerGoal(PlayerId);
	}
}

bool APingPongGate::Server_SetGoal_Validate()
{
	return true;
}

void APingPongGate::Server_SetPlayerId_Implementation(int32 Value)
{
	this->PlayerId = Value;
}

bool APingPongGate::Server_SetPlayerId_Validate(int32 Value)
{
	return true;
	return Value != INDEX_NONE;
}

bool APingPongGate::IsReadyToUse() const
{
	return PlayerId == INDEX_NONE && AttacheToPlayerStart;
}

void APingPongGate::SetGoal()
{
	Server_SetGoal();
}

void APingPongGate::SetPlayerId(int32 Value)
{
	Server_SetPlayerId(Value);
}