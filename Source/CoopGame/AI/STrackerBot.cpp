// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCanEverAffectNavigation(false);
    MeshComp->SetSimulatePhysics(true);
    RootComponent = MeshComp;

    NextPathPoint = GetNextPathPoint();


    bAccelChange = false;
    MovementForce = 1000.0f;
    RequiedDistanceToTarget = 100.0f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ASTrackerBot::GetNextPathPoint()
{
    ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

    UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

    if (NavPath->PathPoints.Num() > 1)
    {
        return NavPath->PathPoints[1];
    }

    return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //if ((GetActorLocation() - NextPathPoint).Size() < RequiedDistanceToTarget)
    //{
    //    FVector ForceDirection = NextPathPoint - GetActorLocation();
    //    ForceDirection.Normalize();

    //    FVector Force = ForceDirection * MovementForce;

    //    MeshComp->AddForce(Force, NAME_None, bAccelChange);
    //}
    //else
    //{
    //    NextPathPoint = GetNextPathPoint();
    //}

}

