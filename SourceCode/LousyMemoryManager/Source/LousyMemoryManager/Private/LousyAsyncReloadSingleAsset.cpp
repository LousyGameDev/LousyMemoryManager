#include "LousyAsyncReloadSingleAsset.h"
#include "Engine/AssetManager.h"

ULousyAsyncReloadSingleAsset* ULousyAsyncReloadSingleAsset::AsyncReloadSingleAsset(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToReload)
{
    ULousyAsyncReloadSingleAsset* Node = NewObject<ULousyAsyncReloadSingleAsset>();
    Node->WorldContext = WorldContextObject;
    Node->AssetToReloadRef = AssetToReload;
    return Node;
}

void ULousyAsyncReloadSingleAsset::Activate()
{
    if (!AssetToReloadRef.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] AsyncReloadSingleAsset: Invalid soft reference."));
        OnFailed.Broadcast();
        return;
    }

    FSoftObjectPath AssetPath = AssetToReloadRef.ToSoftObjectPath();

    if (!AssetPath.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] AsyncReloadSingleAsset: Invalid asset path."));
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    // Fire OnStarted exec pin
    OnStarted.Broadcast();

    // Unload the asset first
    StreamableManager->Unload(AssetPath);
    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] AsyncReloadSingleAsset: Unloaded %s."), *AssetPath.ToString());

    // Wrap asset path in array
    TArray<FSoftObjectPath> AssetPaths;
    AssetPaths.Add(AssetPath);

    // Async load with handler
    StreamableManager->RequestAsyncLoad(
        AssetPaths,
        FStreamableDelegate::CreateUObject(this, &ULousyAsyncReloadSingleAsset::HandleAssetReloaded),
        FStreamableManager::AsyncLoadHighPriority
    );
}

void ULousyAsyncReloadSingleAsset::HandleAssetReloaded()
{
    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] AsyncReloadSingleAsset: Reloaded successfully."));
    OnCompleted.Broadcast();
}