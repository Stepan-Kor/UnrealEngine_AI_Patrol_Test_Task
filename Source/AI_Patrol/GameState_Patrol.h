// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameState_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class AI_PATROL_API AGameState_Patrol : public AGameStateBase
{
	GENERATED_BODY()
protected:
	TArray <class AAI_Char*> AI_Chars;
public:
	void AddAIChar(AAI_Char* NewOne);
	void SwitchStateOfAI(bool NewState);
};
