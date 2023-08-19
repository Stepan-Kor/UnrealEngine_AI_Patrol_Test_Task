// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Char.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Player_Char.h"
#include "GameState_Patrol.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AAI_Char::AAI_Char()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	PerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	Billboard = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Health Billboard"));
	Billboard->SetupAttachment(RootComponent);
	Billboard->SetRelativeLocation(FVector(0, 0, 260));
	if (BillboardMaterial) {
		Billboard->AddElement(BillboardMaterial, nullptr, false, 150, 150, nullptr);
	}
	SightConfig = CreateDefaultSubobject <UAISenseConfig_Sight>(TEXT("Sight Configureation"));
	SightConfig->SightRadius = 3000;
	SightConfig->LoseSightRadius = 3500;
	SightConfig->PeripheralVisionAngleDegrees = 60;
	SightConfig->SetMaxAge(0.2);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	PerComp->ConfigureSense(*SightConfig);
	PerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	PerComp->OnPerceptionUpdated.AddDynamic(this, &AAI_Char::PerceptionUpdated);

	GetCharacterMovement()->MaxAcceleration=500;
	GetCharacterMovement()->MaxWalkSpeed = 150;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->HearingThreshold = 5000;
	PawnSensingComp->SightRadius = 0;
	//PawnSensingComp->SightRadius
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAI_Char::BeginPlay()
{
	Super::BeginPlay();
	StartLoc = GetActorLocation();
	AIContrlr = Cast<AAIController>(GetController());
	NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavSystem || !AIContrlr || !PerComp)return;
	AIContrlr->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AAI_Char::OnMoveCompleted);
	//AIContrlr->ReceiveMoveCompleted.AddDynamic(this, &AAI_Char::OnMoveCompleted);
	if (PawnSensingComp)	{
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AAI_Char::OnHearNoise);
		//PawnSensingComp->OnSeePawn.AddDynamic(this, &AAI_Char::SeenSomeone);
	}
	if (BillboardMaterial) {
		MaterialInstance = UMaterialInstanceDynamic::Create(BillboardMaterial, this);
		Billboard->Elements.Empty();
		Billboard->AddElement(MaterialInstance, nullptr, false, 15, 15, nullptr);

		MaterialInstance->SetVectorParameterValue(FName("Color"),FVector(GetStateColor()));
	}
	auto GS = Cast<AGameState_Patrol>(GetWorld()->GetGameState());
	if(GS)GS->AddAIChar(this);
	if (PatrolState == EPatrolType::Patrol) { StartPatrol(); }
	
}

void AAI_Char::PerceptionUpdated(const TArray<AActor*>& Actors)
{
	if (PatrolState == EPatrolType::DoNothing)return;
	if (SeenTarget) {
		if (Actors.Contains(SeenTarget)) { 
			LastTargetLoc = SeenTarget->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("AI Char: player still visible: %f"),
				FVector::Distance(LastTargetLoc, GetActorLocation()));
			CheckLastSeenLoc();
			return; }
		UE_LOG(LogTemp, Warning, TEXT("AI Char: loosed player."));
		SeenTarget = nullptr;
		ChangePatrolState(EPatrolType::Search);
		CheckLastSeenLoc();
		return;
	}
	APlayer_Char* PC{ nullptr };
	for (AActor* TempActor : Actors)
	{
		//UE_LOG(LogTemp,Warning,TEXT("AI Char: detect some one - %s"),*TempActor->GetName());
		PC = Cast<APlayer_Char>(TempActor);
		if (!PC)continue;
		UE_LOG(LogTemp, Warning, TEXT("AI Char: player visualy found: %f"),
			FVector::Distance(PC->GetActorLocation(),GetActorLocation()));

		if (ChangePatrolState(EPatrolType::Chase)) {
			SeenTarget = PC;
			LastTargetLoc = SeenTarget->GetActorLocation();
			CheckLastSeenLoc();
		}
		break;
	}
}

void AAI_Char::SwitchPower(bool NewState)
{
	ChangePatrolState( NewState ? EPatrolType::Patrol : EPatrolType::DoNothing);
	if (NewState) {
		PerComp->Activate();
		PawnSensingComp->Activate();
		HealthCurrent = HealthStart;
		if (MaterialInstance)MaterialInstance->SetScalarParameterValue(FName("Health"), 1);
		StartPatrol();
		return;
	}
	PerComp->Deactivate();
	PawnSensingComp->Deactivate();	
	AIContrlr->StopMovement();
}

float AAI_Char::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (PatrolState == EPatrolType::DoNothing)goto GoSkip;
	UE_LOG(LogTemp, Warning, TEXT("AI Char: damage received."));
	HealthCurrent -= DamageAmount;
	if (HealthCurrent < 0){SwitchPower(false); goto GoSkip;}
	if (MaterialInstance)MaterialInstance->SetScalarParameterValue(FName("Health"), HealthCurrent / HealthStart);
	if (!DamageCauser) goto GoSkip;
	auto Player = Cast<APlayer_Char>(DamageCauser);
	if (!Player)goto GoSkip;
	ChangePatrolState(EPatrolType::Chase);
	AIContrlr->MoveToLocation(Player->GetActorLocation());
	
	GoSkip:
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAI_Char::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	//UE_LOG(LogTemp, Warning, TEXT("AI Char: move to done."));
	if (!NavSystem || !AIContrlr || PatrolState==EPatrolType::DoNothing)return;
	if (Result.IsInterrupted())return;
	if (Result.IsFailure())return;
	if (LastTimeMoveFinished == UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld())) {
		FTimerHandle EmptyTimeHandle;
		GetWorldTimerManager().SetTimer(EmptyTimeHandle, this, &AAI_Char::DelayedMove, 1.0f, false);
		return;
	}
	LastTimeMoveFinished = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	FNavLocation TargetLoc;
	switch (PatrolState)
	{
	case EPatrolType::Search:
		ChangePatrolState(EPatrolType::Patrol);
	case EPatrolType::Patrol:
		StartPatrol();
		break;
	case EPatrolType::Chase:
		//UE_LOG(LogTemp, Warning, TEXT("AI Char: player visible %d."),PerComp->HasAnyActiveStimulus(*SeenTarget));
		if (IsValid(SeenTarget)) {
			if (PerComp->HasAnyActiveStimulus(*SeenTarget)) {
				LastTargetLoc = SeenTarget->GetActorLocation();
				if (FVector::Distance(LastTargetLoc, GetActorLocation()) > 35) {
					CheckLastSeenLoc();
					return;
				}
				FTimerHandle EmptyTimeHandle;
				GetWorldTimerManager().SetTimer(EmptyTimeHandle, this, &AAI_Char::DelayedMove, 1.0f, false);
			}
		}
		ChangePatrolState(EPatrolType::Search);
		NavSystem->GetRandomReachablePointInRadius(LastTargetLoc, SearchRadius, TargetLoc);
		AIContrlr->MoveToLocation(TargetLoc);
		break;
	case EPatrolType::DoNothing:
		break;
	default:
		break;
	}

}

void AAI_Char::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if (PatrolState != EPatrolType::Patrol) return;
	APlayer_Char* PC{ nullptr };
	PC=Cast< APlayer_Char > (PawnInstigator);
	if (!IsValid(PC))return;
	UE_LOG(LogTemp, Warning, TEXT("AI Char: heard player."));
	ChangePatrolState(EPatrolType::Search);
	FNavLocation TargetLoc;
	NavSystem->GetRandomReachablePointInRadius(Location, SearchRadius, TargetLoc);
	AIContrlr->MoveToLocation(TargetLoc);
}

FColor AAI_Char::GetStateColor()
{
	switch (PatrolState)
	{
	case EPatrolType::Patrol:
		return FColor::Green;
	case EPatrolType::Search:
		return FColor::Yellow;
	case EPatrolType::Chase:
		return FColor::Red;
	case EPatrolType::DoNothing:
		return FColor::Black;
	}
	return FColor(0,0,0);
}

bool AAI_Char::ChangePatrolState(EPatrolType NewState)
{
	PatrolState = NewState;
	if (PatrolState != EPatrolType::Chase)SeenTarget = nullptr;
	float Speed{ 150 };
	switch (PatrolState)
	{
	case EPatrolType::Patrol:
		Speed = 150;
		break;
	case EPatrolType::Search:
		Speed = 200;
		break;
	case EPatrolType::Chase:
		Speed = 300;
		break;
	case EPatrolType::DoNothing:
		break;
	default:
		break;
	}
	GetCharacterMovement()->MaxAcceleration = Speed*2;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	if (!MaterialInstance)return true;
	MaterialInstance->SetVectorParameterValue(FName("Color"), FVector(GetStateColor()));
	return true;
}

void AAI_Char::CheckLastSeenLoc()
{
	if (PatrolState == EPatrolType::DoNothing)return;
	AIContrlr->MoveToLocation(LastTargetLoc);
}

void AAI_Char::SeenSomeone(APawn* Observable)
{
	UE_LOG(LogTemp, Warning, TEXT("AI Char: seen some %s."), *Observable->GetName());
}

void AAI_Char::DelayedMove()
{
	UE_LOG(LogTemp, Warning, TEXT("AI Char: delayed chase fired."));
	if (PatrolState == EPatrolType::DoNothing)return;
	if (PatrolState == EPatrolType::Chase) {
		if (!SeenTarget)StartPatrol();
		if (PerComp->HasAnyActiveStimulus(*SeenTarget)) {
			LastTargetLoc = SeenTarget->GetActorLocation();
			if (FVector::Distance(LastTargetLoc, GetActorLocation()) > 45) {
				CheckLastSeenLoc();
				return;
			}
			FTimerHandle EmptyTimeHandle;
			GetWorldTimerManager().SetTimer(EmptyTimeHandle, this, &AAI_Char::DelayedMove, 1.0f, false);
		}
		return;
	}
	StartPatrol();	
}

void AAI_Char::StartPatrol(){
	if(PatrolState!= EPatrolType::Patrol)ChangePatrolState(EPatrolType::Patrol);
	FNavLocation TargetLoc;
	NavSystem->GetRandomReachablePointInRadius(StartLoc, PatrolRadius, TargetLoc);
	AIContrlr->MoveToLocation(TargetLoc);
}

// Called every frame
void AAI_Char::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAI_Char::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

