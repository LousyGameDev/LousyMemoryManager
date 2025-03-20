// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncLoadSingleAssets.generated.h"

// Unique Delegates for LOAD
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadSingleStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadSingleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadSingleFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncLoadSingleAssets : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Load Single Asset", ToolTip = "Asynchronously loads a single soft-referenced asset to reduce load times and prevent frame drops."), Category = "Lousy Memory Manager")
    static ULousyAsyncLoadSingleAssets* AsyncLoadSingleAsset(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToLoad);

    // Unique Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnLoadSingleStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnLoadSingleCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnLoadSingleFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TSoftObjectPtr<UObject> AssetToLoadRef;
    TSharedPtr<FStreamableManager> StreamableManager;

    void HandleAssetLoaded();
};