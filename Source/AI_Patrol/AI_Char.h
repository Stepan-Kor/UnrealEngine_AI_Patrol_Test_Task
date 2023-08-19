// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/MaterialBillboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI_Char.generated.h"


UENUM(BlueprintType) enum class EPatrolType : uint8 {
	Patrol=0 UMETA(DisplayName = "Patrol"), Search UMETA(DisplayName = "Search"),
	Chase UMETA(DisplayName = "Chase"), DoNothing  UMETA(DisplayName = "Donothing")};

UCLASS()
class AI_PATROL_API AAI_Char : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAI_Char();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UAIPerceptionComponent* PerComp;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UAISenseConfig_Sight* SightConfig;
	UFUNCTION()void PerceptionUpdated(const TArray <AActor*>& Actors);
	UPROPERTY(EditAnywhere) EPatrolType PatrolState{ };
	void SwitchPower(bool NewState);
	virtual float TakeDamage	(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FTimerHandle UnusedHandle = FTimerHandle();
	UPROPERTY(EditDefaultsOnly) UMaterialBillboardComponent* Billboard;
	UPROPERTY(EditDefaultsOnly) UMaterial* BillboardMaterial;
	UMaterialInstanceDynamic* MaterialInstance;
	FVector StartLoc;
	class AAIController* AIContrlr;
	class UNavigationSystemV1* NavSystem;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	UPROPERTY(EditAnywhere) float PatrolRadius{ 2000 };
	UPROPERTY(EditAnywhere) float SearchRadius{ 1000 };
	UFUNCTION()
		void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);
	UPROPERTY(VisibleAnywhere)	class UPawnSensingComponent* PawnSensingComp;
	FColor GetStateColor();
	AActor* SeenTarget;
	FVector LastTargetLoc;
	bool ChangePatrolState(EPatrolType NewState);
	void CheckLastSeenLoc();
	float HealthStart{ 100 };
	float HealthCurrent{ 100 };
	UFUNCTION()void SeenSomeone(APawn* Observable);
	UFUNCTION()void DelayedMove();
	void StartPatrol();
	float LastTimeMoveFinished{ 0 };
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
