#pragma once
#include "CoreMinimal.h"
#include "HAE.generated.h"

#define INVALID_GI FGridIndex(-1, -1)
#define INVALID_TILE Tile()
#define DEBUG_TEXT_TIME 20
#define debugInt(x) if(GEngine) { FString fstr = FString::FromInt(x); GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, *fstr); }
#define debugStr(x) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, TEXT(x)); }
#define debugFstr(x) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, x); }
#define debugBool(x) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, x? TEXT("true") : TEXT("false")); }
#define debugFloat(x) if(GEngine) { FString fstr = FString::SanitizeFloat(x); GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, *fstr); }
#define debugVec(x) if(GEngine) { FString fstr = x.ToString(); GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, *fstr); }
#define debugGI(a) if(GEngine) { FString fstr = TEXT("(") + FString::FromInt(a.x) + TEXT(", ") + FString::FromInt(a.y) + TEXT(")"); GEngine->AddOnScreenDebugMessage(-1, DEBUG_TEXT_TIME, FColor::Purple, *fstr); }

class AUnit;

UENUM(BlueprintType)
enum class HexDirection : uint8 {
	Right, DownRight, DownLeft, Left, UpLeft, UpRight
};

UENUM(BlueprintType)
enum class TileProperty : uint8 {
	Impassable
};

UENUM(BlueprintType)
enum class LoginResult : uint8 {
	Success, Fail, NotRegistered
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
	FGridIndex& operator*=(const int32& mult) {
		x *= mult;
		y *= mult;
		return *this;
	}
	operator FString& () {
		FString fstr = TEXT("(") + FString::FromInt(x) + TEXT(", ") + FString::FromInt(y) + TEXT(")");
		return fstr;
	}
	operator FString() const {
		return TEXT("(") + FString::FromInt(x) + TEXT(", ") + FString::FromInt(y) + TEXT(")");
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AUnit* unit;
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

namespace HAE {
	const uint8 strongHitPowerNeeded = 8;
	const uint8 strongHitTalentPowerNeeded = 5;
	const uint8 weakHitPowerNeeded = 5;
	const uint8 mapX = 100;
	const uint8 mapY = 100;
	const uint8 rotationNeeded = 100;
	const uint8 maxOrders = 50;
	const float conditionDamagePowerBonus = 5.0f;
	const float torsoPowerMult = 1.1f;
	const float hexSize = 100.0f;
	const float hexHalfSize = hexSize / 2.0f;
	const float sin60 = FMath::Sin(PI / 3.0f);
	const float tan30 = FMath::Tan(PI / 6.0f);
	const float hexY = hexSize * sin60;
	const float third = 1.0f / 3.0f;
	const float twoThirds = 2.0f / 3.0f;
	template<typename To, typename From> To* castStruct(From* base) {
		static_assert(TIsDerivedFrom<To, From>::IsDerived, "To has to be derived from From.");
		return static_cast<To*>(base);
	}
	template<typename T> T posMod(const T& value, const T& mod) {
		return (value % mod + mod) % mod;
	}
}