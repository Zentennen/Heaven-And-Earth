// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AccountSave.generated.h"

/**
 * 
 */
UCLASS()
class HEAVENANDEARTH_API UAccountSave : public USaveGame
{
	GENERATED_BODY()
public:
	FString username;
	FString password;
};
