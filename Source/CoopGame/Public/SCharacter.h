// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealth;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

    virtual FVector GetPawnViewLocation() const override;


    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void MoveForward(float Value);

    void MoveRight(float Value);

    void BeginCrouth();

    void EndCrouth();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;

    bool bWantsToZoom;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    float ZoomedFOV;

    UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.0, ClampMax = 100.0))
    float ZoomInterpSpeed;

    float DefaultFOV;

    UPROPERTY(Replicated)
    ASWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    TSubclassOf<ASWeapon> StarterWeaponClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    FName SocketName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USHealth* HealthComponent;

protected:

    void BeginZoom();

    void EndZoom();

    void StartFire();

    void StopFire();

    UFUNCTION()
    void TakeHealthChanged(USHealth* HealthComp, float Health, float HelathDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
    bool bDied;
};
