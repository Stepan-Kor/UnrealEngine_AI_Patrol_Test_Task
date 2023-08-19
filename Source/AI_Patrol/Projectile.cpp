// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Player_Char.h"
#include "AI_Char.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollisionR = CreateDefaultSubobject <USphereComponent>("Sphere Component");
	SetRootComponent(SphereCollisionR);
	SphereCollisionR->InitSphereRadius(20);
	SphereCollisionR->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollisionR->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollisionR->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	SphereCollisionR->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	SphereCollisionR->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SphereCollisionR->SetNotifyRigidBodyCollision(true);
	SphereCollisionR->SetSimulatePhysics(true);
	Mesh = CreateDefaultSubobject <UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollisionR);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectComp = CreateDefaultSubobject <UProjectileMovementComponent>("Projectile Movement");
	ProjectComp->bShouldBounce = true;
	ProjectComp->InitialSpeed = 1500;
	ProjectComp->MaxSpeed = 6000;
	InitialLifeSpan = 10;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereCollisionR->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* Component, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	if (bHited || !IsValid(Creator))return;
	AAI_Char* AIChar = Cast<AAI_Char>(OtherActor);
	if (!IsValid(AIChar))return;
	bHited = true;
	UE_LOG(LogTemp, Warning, TEXT("Ball: hit done."));
	UGameplayStatics::ApplyDamage(AIChar,10,Creator->GetController(),Creator,UDamageType::StaticClass());
	//UE_LOG(LogTemp, Warning, TEXT("Project: ball hit done."));
}

