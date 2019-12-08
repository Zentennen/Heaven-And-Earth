// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "GameFramework/SaveGame.h"
#include "UnitSave.generated.h"

UCLASS()
class HEAVENANDEARTH_API UUnitSave : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString accountUsername;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FUnitStats baseStats;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FGridIndex position;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) HexDirection direction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<Order> orders;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FGridIndex> path;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) uint8 orderProgress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) HexDirection lastDirection;
};
