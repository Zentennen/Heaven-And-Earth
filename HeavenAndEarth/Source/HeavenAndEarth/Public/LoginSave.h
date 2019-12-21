// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoginSave.generated.h"

/**
 * 
 */
UCLASS()
class HEAVENANDEARTH_API ULoginSave : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString username;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString password;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString ip;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString campaignName;
};
