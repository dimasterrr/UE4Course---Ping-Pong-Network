#include "PakGameInstance.h"

#include "DLCLoader.h"

void UPakGameInstance::Init()
{
	Super::Init();

	DLCLoader = NewObject<UDLCLoader>(this, "DLC_Loader");
}

TArray<FGameObjectInfo> UPakGameInstance::GetGameObjectsInfo() const
{
	return  DLCLoader->GetGameObjectsInfo();
}
