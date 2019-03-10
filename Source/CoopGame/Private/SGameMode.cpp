// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Engine/EngineTypes.h"
#include "SHealth.h"
#include "SGameState.h"

ASGameMode::ASGameMode()
{
    TimeBetweenWaves = 2.0f;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;

    GameStateClass = ASGameState::StaticClass();
}

void ASGameMode::StartWave()
{
    WaveCount++;

    NrOfBotsToSpawn = 2 * WaveCount;

    GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnNewBot, 1.0f, true, 0.0f);

    SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::SpawnBotTimerElapsed()
{
    SpawnNewBot();

    NrOfBotsToSpawn--;

    if (NrOfBotsToSpawn <= 0)
    {
        EndWave();
    }
}

void ASGameMode::EndWave()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

    SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

    SetWaveState(EWaveState::WaitingToStart);
}


void ASGameMode::CheckWaveState()
{
    bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);
    if (bIsPreparingForWave || NrOfBotsToSpawn > 0)
    {
        return;
    }

    bool bIsAnyBotAlive = false;
    for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
    {
        APawn* Pawn = It->Get();
        if (Pawn == nullptr || Pawn->IsPlayerControlled())
        {
            continue;
        }

        USHealth* Health = Cast<USHealth>(Pawn->GetComponentByClass(USHealth::StaticClass()));
        if (Health && Health->GetHealth() > 0)
        {
            bIsAnyBotAlive = true;
            break;
        }
    }

    if (!bIsAnyBotAlive)
    {
        SetWaveState(EWaveState::WaveComplete);
        PrepareForNextWave();
    }
}

void ASGameMode::CheckPlayerAlive()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->GetPawn())
        {
            APawn* MyPawn = PC->GetPawn();
            USHealth* Health = Cast<USHealth>(PC->GetComponentByClass(USHealth::StaticClass()));
            if (ensure(Health) && Health->GetHealth() > 0)
            {
                return;
            }
        }
    }

    GameOver();
}

void ASGameMode::GameOver()
{
    EndWave();

    SetWaveState(EWaveState::GameOver);

    UE_LOG(LogTemp, Log, TEXT("Game Over! Player died!"));
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
    ASGameState* GS = GetGameState<ASGameState>();

    if (ensureAlways(GS))
    {
        GS->SetWaveState(NewState);
    }
}

void ASGameMode::StartPlay()
{
    Super::StartPlay();

    PrepareForNextWave();

    CheckPlayerAlive();
}

void ASGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CheckWaveState();
}
