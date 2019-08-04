#pragma once
#include "CoreMinimal.h"
//#include "GameFramework\Actor.h"
#include "GameLib.generated.h"

#define INVALID_GI FGridIndex(-1, -1)

UENUM(BlueprintType)
enum class BodyPart : uint8 {
	Head, Torso, Arms, Legs
};

UENUM(BlueprintType)
enum class Perk : uint8 {
	Ruthless
};

UENUM(BlueprintType)
enum class HexDirection : uint8 {
	Right, DownRight, DownLeft, Left, UpLeft, UpRight
};

UENUM(BlueprintType)
enum class TileProperty : uint8 {
	Impassable
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
	bool operator==(const FGridIndex& other) const {
		return x == other.x && y == other.y;
	}
	FGridIndex& operator+=(const FGridIndex& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	FGridIndex& operator-=(const FGridIndex& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	friend uint32 GetTypeHash(const FGridIndex& other) {
		FVector2D v(other.x, other.y);
		return GetTypeHash(v);
	}
	friend FGridIndex operator+(const FGridIndex& first, const FGridIndex& second) {
		return FGridIndex(first.x + second.x, first.y + second.y);
	}
	friend FGridIndex operator-(const FGridIndex& first, const FGridIndex& second) {
		return FGridIndex(first.x - second.x, first.y - second.y);
	}
	friend FGridIndex operator*(const FGridIndex& first, const int32& second) {
		return FGridIndex(first.x * second, first.y * second);
	}
};

USTRUCT(BlueprintType)
struct FTile {
	//struct containing data about a tile received from AGame
	//pos (-1, -1) means invalid tile
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) uint8 cost;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FGridIndex pos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<TileProperty> properties;
	FTile() : cost(1), pos(INVALID_GI) {
		properties.Empty();
	}
	FTile(const FGridIndex& gi) : cost(1), pos(gi) {
		properties.Empty();
	}
	FTile(const uint8& c) : cost(c) {
		properties.Empty();
	}
	FTile(const uint8& c, const FGridIndex& gi) : cost(c), pos(gi) {
		properties.Empty();
	}
	FTile(const uint8& c, const TArray<TileProperty>& prop) : cost(c), properties(prop) {

	}
	FTile(const uint8& c, const FGridIndex& gi, const TArray<TileProperty>& prop) : cost(c), pos(gi), properties(prop) {

	}
};

namespace GameLib {
	const int32 strongHitPowerNeeded = 8;
	const int32 strongHitTalentPowerNeeded = 5;
	const int32 weakHitPowerNeeded = 5;
	const int32 mapX = 100;
	const int32 mapY = 100;
	const float conditionDamagePowerBonus = 5.0f;
	const float torsoPowerMult = 1.1f;
	const float hexSize = 100.0f;
	const float hexHalfSize = hexSize / 2.0f;
	const float sin60 = FMath::Sin(PI / 3.0f);
	const float tan30 = FMath::Tan(PI / 6.0f);
	const float hexY = hexSize * sin60;
	const float third = 1.0f / 3.0f;
	const float twoThirds = 2.0f / 3.0f;
}