// Fill out your copyright notice in the Description page of Project Settings.


#include "Button_AISwitch.h"
#include "GameState_Patrol.H"

// Sets default values
AButton_AISwitch::AButton_AISwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* BaseScene = CreateDefaultSubobject<USceneComponent>(TEXT("Basic Scene"));
	SetRootComponent(BaseScene);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	Mesh->SetupAttachment(BaseScene);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	GlowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top Mesh"));
	GlowMesh->SetupAttachment(BaseScene);
	GlowMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
}

// Called when the game starts or when spawned
void AButton_AISwitch::BeginPlay()
{
	Super::BeginPlay();
	if(GlowMaterial) MaterialInstance = UMaterialInstanceDynamic::Create(GlowMaterial, this);
	if (MaterialInstance)GlowMesh->SetMaterial(0, MaterialInstance);
	GameState = Cast<AGameState_Patrol>(GetWorld()->GetGameState());
}

// Called every frame
void AButton_AISwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButton_AISwitch::FlipFlopState()
{
	SwitchState = !SwitchState;
	if (MaterialInstance)MaterialInstance->SetVectorParameterValue(FName("Color"),
		SwitchState ? FColor::Green : FColor::Black);
	if (GameState)GameState->SwitchStateOfAI(SwitchState);
	else
		UE_LOG(LogTemp, Warning, TEXT("Button: dont have game state."));
}

