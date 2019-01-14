// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "SHealth.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCanEverAffectNavigation(false);
    MeshComp->SetSimulatePhysics(true);
    RootComponent = MeshComp;

    HealthComp = CreateDefaultSubobject<USHealth>(TEXT("HealthComp"));
    HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

    bAccelChange = true;
    MovementForce = 1000.0f;
    RequiedDistanceToTarget = 100.0f;
    ExplosionRadius = 100.0f;
    ExplosionDamage = 100.0f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
    NextPathPoint = GetNextPathPoint();
}

FVector ASTrackerBot::GetNextPathPoint()
{
    ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

    if (PlayerPawn)
    {
        UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

        if (NavPath->PathPoints.Num() > 1)
        {
            return NavPath->PathPoints[1];
        }
    }

    return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if ((GetActorLocation() - NextPathPoint).Size() < RequiedDistanceToTarget)
    {
        NextPathPoint = GetNextPathPoint();
    }
    else
    {
        FVector ForceDirection = NextPathPoint - GetActorLocation();
        ForceDirection.Normalize();

        FVector Force = ForceDirection * MovementForce;

        MeshComp->AddForce(Force, NAME_None, bAccelChange);
    }
}

void ASTrackerBot::OnHealthChanged(USHealth* OwningHealthComp, float Health, float HelathDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

    if (MatInst == nullptr)
    {
        MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
    }

    if (MatInst)
    {
        MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
    }

    if (Health <= 0.0f)
    {
        SelfDestruct();
    }

}

void ASTrackerBot::SelfDestruct()
{
    if (bExploded)
    {
        return;
    }

    bExploded = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);
    UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true);

    DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, true, 10.0f);

    Destroy();
}

