#pragma once
#include "CoreMinimal.h"
#include "GameFramework\Actor.h"

#define MAX_CONDITION_POWER_MULT 5
#define CRIT_POWER_NEEDED 5
#define CRIT_TALENT_POWER_NEEDED 3
#define GLANCING_POWER_NEEDED 5

UENUM(BlueprintType)
enum class BodyPart : uint8 {
	Head, Torso, Arms, Legs
};

namespace GameLib {
	int32 roll(uint8 num, uint8 max);
}