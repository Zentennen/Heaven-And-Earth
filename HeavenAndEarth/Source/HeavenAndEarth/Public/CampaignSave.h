// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Unit.h"
#include "CampaignSave.generated.h"

UCLASS()
class HEAVENANDEARTH_API UCampaignSave : public USaveGame
{
	GENERATED_BODY()
public: 
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 numAccounts;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 numUnits;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FUnitData> unitData;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FOrderList> unitOrders;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FPath> unitPaths;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FString> accountUsernames;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FString> accountPasswords;
};
