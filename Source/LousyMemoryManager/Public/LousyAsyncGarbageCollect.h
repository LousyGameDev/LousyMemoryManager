// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TimerManager.h"
#include "LousyAsyncGarbageCollect.generated.h"

// Delegates for exec pins
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGCStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGCCompleted);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncGarbageCollect : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    /** Blueprint Node Function */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", ToolTip = "Forces a delayed garbage collection, optionally performing a full purge. Useful for cleaning up memory without interrupting gameplay immediately."), Category = "Lousy Memory Manager")
    static ULousyAsyncGarbageCollect* ForceGarbageCollectionWithDelay(UObject* WorldContextObject, float DelaySeconds = 1.0f, bool bFullPurge = false);


    /** Exec outputs */
    UPROPERTY(BlueprintAssignable)
    FOnGCStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnGCCompleted OnCompleted;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    float Delay;
    bool bFullPurgeFlag;

    FTimerHandle TimerHandle;
};