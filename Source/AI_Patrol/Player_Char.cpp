// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Char.h"
#include "Components/CapsuleComponent.h"
#include "Projectile.h"
#include "ScreenWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Button_AISwitch.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
APlayer_Char::APlayer_Char()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(-40, 0, 60));
	Camera->bUsePawnControlRotation = true;
	ProjectileSpawnParameters.SpawnCollisionHandlingOverride 
		= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	NoiseComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Component of Noise for AI"));
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundOFNoise
		(TEXT("SoundCue'/Game/VR_click2_Cue.VR_click2_Cue'"));
	if (SoundOFNoise.Object)SoundNoise = SoundOFNoise.Object;
}

// Called when the game starts or when spawned
void APlayer_Char::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(WidgetClass))ScreenWidget = Cast <UScreenWidget>	(CreateWidget(GetWorld(), WidgetClass, FName("Screen Widget")));
	if (IsValid(ScreenWidget)) ScreenWidget->AddToViewport(0);
}

// Called every frame
void APlayer_Char::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayer_Char::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayer_Char::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayer_Char::StopJumping);
	PlayerInputComponent->BindAction("Shot", IE_Pressed, this, &APlayer_Char::MakeShot);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayer_Char::Interact);
	PlayerInputComponent->BindAction("MakeNoise", IE_Pressed, this, &APlayer_Char::MakeNoise);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayer_Char::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayer_Char::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayer_Char::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayer_Char::LookUp);

}

void APlayer_Char::Interact()
{
	if (!Camera)return;
	FHitResult Hit;
	FVector TraceStart = Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart+ Camera->GetForwardVector()* 500;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic,
		QueryParams);
	if (!Hit.GetActor()) { 
		UE_LOG(LogTemp, Warning, TEXT("Player: line trace didnt get actor."));
		return; }
	AButton_AISwitch* BS;
	BS = Cast<AButton_AISwitch>(Hit.GetActor());
	if (BS)BS->FlipFlopState();
	else {
		UE_LOG(LogTemp, Warning, TEXT("Player: line trace didnt get button."));
	}
}

void APlayer_Char::MakeNoise()
{
	UE_LOG(LogTemp, Warning, TEXT("Player: making noise..."));
	if(SoundNoise)UGameplayStatics::PlaySound2D(this, SoundNoise, 2);
	NoiseComp->MakeNoise(this, 1, GetActorLocation());
}

void APlayer_Char::MoveForward(float Axis)
{
	if (Axis == 0)return;
	if (false && UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - LastTimeWalkNoiseMaden > 2) {
		LastTimeWalkNoiseMaden = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
		UE_LOG(LogTemp,Warning,TEXT("Player: step noise happen."));
		NoiseComp->MakeNoise(this,1,GetActorLocation());
	}
	AddMovementInput(GetActorForwardVector(), Axis);
}

void APlayer_Char::MoveRight(float Axis)
{
	if (Axis != 0)		AddMovementInput(GetActorRightVector(), Axis);
}

void APlayer_Char::Turn(float Axis)
{
	if (Axis != 0) 	AddControllerYawInput(Axis * TurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayer_Char::LookUp(float Axis)
{
	if (Axis != 0) {
		AddControllerPitchInput(Axis * LookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void APlayer_Char::MakeShot()
{
	//UE_LOG(LogTemp,Warning,TEXT("Pl Char: shot clicked."));
	if (IsValid(ProjectileClass) && IsValid(Camera)) {
		FVector SpawnLocation = Camera->GetComponentLocation()+Camera->GetForwardVector()*50;
		AActor* TempActor = GetWorld()->SpawnActor <AProjectile>(ProjectileClass, SpawnLocation,
			Camera->GetComponentRotation(), ProjectileSpawnParameters);
		AProjectile* P = Cast<AProjectile>(TempActor);
		if (!P)return;
		P->Creator = this;
		//NoiseComp->MakeNoise(this,1,GetActorLocation());
	}
}

