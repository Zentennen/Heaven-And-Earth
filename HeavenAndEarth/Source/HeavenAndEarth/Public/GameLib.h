#pragma once
#include "CoreMinimal.h"
#include "GameFramework\Actor.h"

UENUM(BlueprintType)
enum class BodyPart : uint8 {
	Head, Torso, Arms, Legs
};

UENUM(BlueprintType)
enum class Health : uint8 {
	Dead, Crippled, HeavilyWounded, LightlyWounded, Healthy
};

UENUM(BlueprintType)
enum class FormationStrength : uint8 {
	Fine, Damaged, Broken, Shattered
};

namespace GameLib {

}