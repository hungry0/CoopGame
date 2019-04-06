// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class UParticleSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();


    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    void RefreshPath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    FVector GetNextPathPoint();

    FTimerHandle TimerHandle_RefreshPath;

protected:

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    FVector NextPathPoint;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    bool bAccelChange;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float MovementForce;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float RequiedDistanceToTarget;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    USHealth* HealthComp;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    USphereComponent* SphereComp;

    UMaterialInstanceDynamic* MatInst;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UParticleSystem* ExplosionEffect;

    bool bExploded;

    bool bStartedSelfDestruction;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float SelfDamageInternal;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float ExplosionRadius;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float ExplosionDamage;

    FTimerHandle TimerHandle_SelfDamage;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    USoundCue* SelfDestructSound;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    USoundCue* ExplodeSound;

    void DamageSelf();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnHealthChanged(USHealth* OwningHealthComp, float Health, float HelathDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    void SelfDestruct();
};
