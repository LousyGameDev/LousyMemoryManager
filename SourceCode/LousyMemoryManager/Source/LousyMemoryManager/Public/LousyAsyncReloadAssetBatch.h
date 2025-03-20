// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncReloadAssetBatch.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadBatchStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadBatchCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadBatchFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncReloadAssetBatch : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Reload Asset Batch", ToolTip = "Asynchronously reloads a batch of assets, useful for resetting or refreshing assets at runtime without blocking the game thread."), Category = "Lousy Memory Manager")
    static ULousyAsyncReloadAssetBatch* AsyncReloadAssetBatch(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToReload);

    // Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnReloadBatchStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnReloadBatchCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnReloadBatchFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TArray<TSoftObjectPtr<UObject>> AssetsToReload;
    TSharedPtr<FStreamableManager> StreamableManager;
    TArray<FSoftObjectPath> ValidAssetPaths;
    int32 PendingLoadCount = 0;

    void HandleSingleAssetReloaded();
};