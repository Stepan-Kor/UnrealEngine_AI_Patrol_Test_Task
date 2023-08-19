// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Projectile.generated.h"

UCLASS()
class AI_PATROL_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Transient, Category = "Collision") USphereComponent* SphereCollisionR;
	UPROPERTY(EditDefaultsOnly) UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly) UProjectileMovementComponent* ProjectComp;
	bool bHited{ false };
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void OnHit(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse, const FHitResult& HitResult);
	class APlayer_Char* Creator;
};
