// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Player_Char.generated.h"

UCLASS()
class AI_PATROL_API APlayer_Char : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayer_Char();
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)UCameraComponent* Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Widgets")
		class UScreenWidget* ScreenWidget;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) TSubclassOf<class UScreenWidget> WidgetClass;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) TSubclassOf<class AProjectile> ProjectileClass;
	FActorSpawnParameters ProjectileSpawnParameters;
	float LastTimeWalkNoiseMaden{ 0 };
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) class UPawnNoiseEmitterComponent* NoiseComp;
	void Interact();
	void MakeNoise();
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) class USoundCue* SoundNoise;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float TurnRate = 55;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float LookUpRate = 55;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void Turn(float Axis);
	void LookUp(float Axis);
	void MakeShot();

};
