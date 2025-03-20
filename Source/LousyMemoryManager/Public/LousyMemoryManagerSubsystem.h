// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "LousyMemoryManagerSubsystem.generated.h"

UCLASS()
class LOUSYMEMORYMANAGER_API ULousyMemoryManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    /** Start scheduled garbage collection at specified interval */
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager")
    void StartScheduledGarbageCollection(float IntervalSeconds, bool bFullPurge = false);

    /** Stop scheduled garbage collection */
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager")
    void StopScheduledGarbageCollection();

private:

    FTimerHandle ScheduledGCTimerHandle;
    bool bScheduledFullPurge;
};