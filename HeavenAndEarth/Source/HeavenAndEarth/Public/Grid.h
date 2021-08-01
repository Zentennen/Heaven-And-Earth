// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AUnit;

USTRUCT(BlueprintType)
struct FTileColumn {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<uint8> costs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<uint64> flags;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AUnit*> units;
	FTileColumn() {
		costs.Reserve(1000);
		flags.Reserve(1000);
		units.Reserve(1000);
	}
	FTileColumn(const int32& rows) {
		costs.Init(60, rows);
		flags.Init(0, rows);
		units.Init(nullptr, rows);
	}
	FTileColumn(const int32& rows, const uint8& cost) {
		costs.Init(cost, rows);
		flags.Init(0, rows);
		units.Init(nullptr, rows);
	}
};

class HEAVENANDEARTH_API Grid
{
public:
	Grid();
	~Grid();
};
