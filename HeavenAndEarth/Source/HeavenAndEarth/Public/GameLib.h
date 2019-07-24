#pragma once
#include "CoreMinimal.h"
#include "GameFramework\Actor.h"
#include "GameLib.generated.h"

UENUM(BlueprintType)
enum class BodyPart : uint8 {
	Head, Torso, Arms, Legs
};

UENUM(BlueprintType)
enum class Perk : uint8 {

};

USTRUCT(BlueprintType)
struct FGridIndex {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 x;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 y;
	FGridIndex() {
		x = 0;
		y = 0;
	}
	FGridIndex(const int32& px, const int32& py) {
		x = px;
		y = py;
	}
};

namespace GameLib {
	const float conditionDamagePowerBonus = 5.0f;
	const float maxConditionPowerMult = 20.0f;
	const float conditionDamageExponent = 1.5f;
	const int32 critPowerNeeded = 8;
	const int32 critTalentPowerNeeded = 5;
	const int32 glancingPowerNeeded = 5;
	const float hexSize = 100.0f;
	const float hexHalfSize = hexSize / 2.0f;
	const float sin60 = FMath::Sin(PI / 3.0f);
	const float tan30 = FMath::Tan(PI / 6.0f);
	const float hexY = hexSize * sin60;
	const float third = 1.0f / 3.0f;
	const float twoThirds = 2.0f / 3.0f;
}