// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Button_AISwitch.generated.h"

UCLASS()
class AI_PATROL_API AButton_AISwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButton_AISwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* GlowMesh;
	UPROPERTY(EditDefaultsOnly) UMaterial* GlowMaterial;
	UMaterialInstanceDynamic* MaterialInstance;
	bool SwitchState{ false };
	class AGameState_Patrol* GameState;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FlipFlopState();
};
