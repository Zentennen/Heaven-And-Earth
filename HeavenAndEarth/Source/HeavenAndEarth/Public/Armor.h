#pragma once
#include "GameLib.h"
#include "Armor.generated.h"

USTRUCT(BlueprintType)
struct FArmorLayer {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 protection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 baseProtection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 condition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 maxCondition;
	static void damage(FArmorLayer layer, uint32 dmg);
	static void repair(FArmorLayer layer, uint32 rep);
	static void repairPercentage(FArmorLayer layer, uint8 percent);
};

USTRUCT(BlueprintType)
struct FArmorPiece {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 coverage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArmorLayer> layers;
};

USTRUCT(BlueprintType)
struct FArmor {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArmorPiece> pieces;
};