// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncLoadSingleAssets.h"

ULousyAsyncLoadSingleAssets* ULousyAsyncLoadSingleAssets::AsyncLoadSingleAsset(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToLoad)
{
    ULousyAsyncLoadSingleAssets* Node = NewObject<ULousyAsyncLoadSingleAssets>();
    Node->WorldContext = WorldContextObject;
    Node->AssetToLoadRef = AssetToLoad;
    return Node;
}

void ULousyAsyncLoadSingleAssets::Activate()
{
    if (!AssetToLoadRef.IsValid())
    {
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    FSoftObjectPath AssetPath = AssetToLoadRef.ToSoftObjectPath();

    if (!AssetPath.IsValid())
    {
        OnFailed.Broadcast();
        return;
    }

    // Fire OnStarted
    OnStarted.Broadcast();

    // Request async load
    StreamableManager->RequestAsyncLoad(
        AssetPath,
        FStreamableDelegate::CreateUObject(this, &ULousyAsyncLoadSingleAssets::HandleAssetLoaded),
        FStreamableManager::AsyncLoadHighPriority
    );
}

void ULousyAsyncLoadSingleAssets::HandleAssetLoaded()
{
    // Fire OnCompleted
    OnCompleted.Broadcast();
}