// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState_Patrol.h"
#include "AI_Char.h"

void AGameState_Patrol::AddAIChar(AAI_Char* NewOne)
{
	AI_Chars.Add(NewOne);
}

void AGameState_Patrol::SwitchStateOfAI(bool NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("GameState: new AI state %d."),NewState);
	for (AAI_Char* TempChar : AI_Chars)
	{
		if (!IsValid(TempChar))continue;
		TempChar->SwitchPower(NewState);
	}
}
