// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncActionBase.h"

ULousyAsyncActionBase* ULousyAsyncActionBase::LoadAssetsAsync(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad)
{
    ULousyAsyncActionBase* Node = NewObject<ULousyAsyncActionBase>();
    Node->WorldContext = WorldContextObject;
    Node->AssetsToLoad = AssetsToLoad;
    return Node;
}

void ULousyAsyncActionBase::Activate()
{
    if (AssetsToLoad.Num() == 0)
    {
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    ValidLoadedAssetPaths.Empty();
    PendingLoadCount = 0;

    // Check each asset's validity and load if valid
    for (const TSoftObjectPtr<UObject>& Asset : AssetsToLoad)
    {
        FSoftObjectPath AssetPath = Asset.ToSoftObjectPath();

        if (!AssetPath.IsValid())
        {
            // Skip invalid assets
            continue;
        }

        ValidLoadedAssetPaths.Add(AssetPath);
        PendingLoadCount++;

        // Request individual async load per valid asset
        StreamableManager->RequestAsyncLoad(
            AssetPath,
            FStreamableDelegate::CreateUObject(this, &ULousyAsyncActionBase::HandleSingleAssetLoaded),
            FStreamableManager::AsyncLoadHighPriority
        );
    }

    if (PendingLoadCount == 0)
    {
        // No valid assets found
        OnFailed.Broadcast();
        return;
    }

    // Fire OnStarted pin
    OnStarted.Broadcast();
}

void ULousyAsyncActionBase::HandleSingleAssetLoaded()
{
    PendingLoadCount--;

    // Check if all assets finished loading
    if (PendingLoadCount <= 0)
    {
        OnCompleted.Broadcast();
    }
}