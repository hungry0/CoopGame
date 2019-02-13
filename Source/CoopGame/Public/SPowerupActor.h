// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    float PowerupInternal;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    int32 TotalNrOfTicks;

    FTimerHandle TimerHandle_PowerupTick;

    float TicksProcessed;

    UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
    bool bIsPowerupActive;

    UFUNCTION()
    void OnRep_PowerupActive();

    UFUNCTION()
    void OnTickPowerup();

public:

    void ActivatePowerup();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnPowerupStateChanged(bool bNewIsActive);

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnActivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnPowerupTicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnExpired();
	

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
