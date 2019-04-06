// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealth.h"
#include "Engine/EngineTypes.h"
#include "UnrealNetwork.h"
#include "SGameMode.h"

// Sets default values for this component's properties
USHealth::USHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    DefaultHealth = 100.0f;
    bDead = false;

    TeamNum = 255;

    SetIsReplicated(true);
}


void USHealth::OnRep_Health(float OldHealth)
{
    float Damage = OldHealth - Health;

    OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

// Called when the game starts
void USHealth::BeginPlay()
{
	Super::BeginPlay();

	// ...

    if (GetOwnerRole() == ROLE_Authority)
    {
        AActor* MyOwner = GetOwner();
        if (MyOwner)
        {
            MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealth::HandleTakeAnyDamage);
        }
    }

    Health = DefaultHealth;
}

void USHealth::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || bDead)
    {
        return;
    }

    if (DamagedActor != DamageCauser && isFriendly(DamagedActor, DamageCauser))
    {
        return;
    }

    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

    bDead = Health <= 0.0f;

    UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

    OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

    if (bDead)
    {
        ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
        }
    }
}

// Called every frame
void USHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USHealth, Health);
}

void USHealth::Heal(float HealAmount)
{
    if (HealAmount <= 0.0f || Health <= 0.0f)
    {
        return;
    }

    Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

    UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

    OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

float USHealth::GetHealth() const
{
    return Health;
}

bool USHealth::isFriendly(AActor* ActorA, AActor* ActorB)
{
    if (ActorA == nullptr || ActorB == nullptr)
    {
        return true;
    }

    USHealth* healthA = Cast<USHealth>(ActorA->GetComponentByClass(USHealth::StaticClass()));
    USHealth* healthB = Cast<USHealth>(ActorB->GetComponentByClass(USHealth::StaticClass()));

    if (healthA == nullptr || healthB == nullptr)
    {
        return true;
    }

    return healthA->TeamNum == healthB->TeamNum;

}

