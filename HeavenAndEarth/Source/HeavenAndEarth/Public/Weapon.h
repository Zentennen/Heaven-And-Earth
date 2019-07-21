// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameLib.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FWeapon {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 condition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 baseCondition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 power;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 basePower;
};
