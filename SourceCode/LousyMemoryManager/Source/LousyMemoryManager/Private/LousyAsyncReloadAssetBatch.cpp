// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncReloadAssetBatch.h"
#include "Engine/AssetManager.h"

ULousyAsyncReloadAssetBatch* ULousyAsyncReloadAssetBatch::AsyncReloadAssetBatch(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToReload)
{
    ULousyAsyncReloadAssetBatch* Node = NewObject<ULousyAsyncReloadAssetBatch>();
    Node->WorldContext = WorldContextObject;
    Node->AssetsToReload = AssetsToReload;
    return Node;
}

void ULousyAsyncReloadAssetBatch::Activate()
{
    if (AssetsToReload.Num() == 0)
    {
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    ValidAssetPaths.Empty();
    PendingLoadCount = 0;

    // Validate each asset and prepare unload/reload
    for (const TSoftObjectPtr<UObject>& Asset : AssetsToReload)
    {
        FSoftObjectPath AssetPath = Asset.ToSoftObjectPath();

        if (!AssetPath.IsValid())
        {
            continue; // Skip invalid
        }

        // Unload first
        StreamableManager->Unload(AssetPath);
        ValidAssetPaths.Add(AssetPath);
    }

    if (ValidAssetPaths.Num() == 0)
    {
        OnFailed.Broadcast();
        return;
    }

    // Fire OnStarted
    OnStarted.Broadcast();

    PendingLoadCount = ValidAssetPaths.Num();

    // Async load each valid asset
    for (const FSoftObjectPath& Path : ValidAssetPaths)
    {
        StreamableManager->RequestAsyncLoad(
            Path,
            FStreamableDelegate::CreateUObject(this, &ULousyAsyncReloadAssetBatch::HandleSingleAssetReloaded),
            FStreamableManager::AsyncLoadHighPriority
        );
    }
}

void ULousyAsyncReloadAssetBatch::HandleSingleAssetReloaded()
{
    PendingLoadCount--;

    if (PendingLoadCount <= 0)
    {
        OnCompleted.Broadcast();
    }
}