// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "UnrealNetwork.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{
    PowerupInternal = 0.0f;
    TotalNrOfTicks = 0.0f;

    SetReplicates(true);

    bIsPowerupActive = false;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerupActor::OnRep_PowerupActive()
{
    OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::OnTickPowerup()
{
    TicksProcessed++;

    OnPowerupTicked();

    if (TicksProcessed > TotalNrOfTicks)
    {
        OnExpired();

        bIsPowerupActive = false;
        OnRep_PowerupActive();

        GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
    }
}

void ASPowerupActor::ActivatePowerup()
{
    OnActivated();

    bIsPowerupActive = true;
    OnRep_PowerupActive();

    if (PowerupInternal > 0.0f)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInternal, true);
    }
    else
    {
        OnTickPowerup();
    }
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}

