// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncReloadSingleAsset.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncReloadSingleAsset : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Reload Single Asset", ToolTip = "Asynchronously reloads a single soft-referenced asset at runtime, allowing asset refresh without blocking the game thread."), Category = "Lousy Memory Manager")
    static ULousyAsyncReloadSingleAsset* AsyncReloadSingleAsset(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToReload);

    // Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnReloadStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnReloadCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnReloadFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TSoftObjectPtr<UObject> AssetToReloadRef;
    TSharedPtr<FStreamableManager> StreamableManager;

    void HandleAssetReloaded();
};