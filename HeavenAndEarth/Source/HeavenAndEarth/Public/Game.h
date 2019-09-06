#pragma once
#include "Unit.h"
#include "PC.h"
#include "Armor.h"
#include "Weapon.h"
#include "GameFramework/Actor.h"
#include "Game.generated.h"

USTRUCT(BlueprintType)
struct FTileColumn {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<uint8> costs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<uint8> flags;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AUnit*> units;
	FTileColumn() {

	}
	FTileColumn(const int32& rows) {
		costs.Init(1, rows);
		flags.Init(0, rows);
		units.Init(nullptr, rows);
	}
	FTileColumn(const int32& rows, const uint8& cost) {
		costs.Init(cost, rows);
		flags.Init(0, rows);
		units.Init(nullptr, rows);
	}
};

UCLASS()
class HEAVENANDEARTH_API AGame : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) TArray<FTileColumn> tiles;
	virtual void BeginPlay() override;

public:	
	static AGame* game;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FWeapon> weapons;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray <FArmor> armor;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TArray<AUnit*> units;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TArray<APC*> pcs;
	AGame();
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	static bool addUnit(AUnit* unit);
	static bool addPC(APC* pc);
	UFUNCTION(BlueprintCallable) void executeTurn();
	UFUNCTION(BlueprintPure) static bool isValidPos(const FGridIndex& pos);
	UFUNCTION(BlueprintPure) static int32 manhattanDistance(const FGridIndex& start, const FGridIndex& goal);
	UFUNCTION(BlueprintPure) static FTile getTile(const FGridIndex& tilePos);
	UFUNCTION(BlueprintPure) static FGridIndex movementToGridIndex(const HexDirection& dir, FGridIndex pos);
	UFUNCTION(BlueprintPure) static TArray<TileProperty> getTilePropertiesFromFlags(const uint8& flags);
	UFUNCTION(BlueprintPure) static uint8 getFlagsFromTileProperties(const TArray<TileProperty>& props);
	UFUNCTION(BlueprintCallable) static bool setTile(const FTile& t);
	UFUNCTION(BlueprintCallable) static TArray<FGridIndex> getPathAsGridIndices(const FGridIndex& start, const FGridIndex& goal, const int32& maxMove);
	UFUNCTION(BlueprintCallable) static int32 roll(uint8 num, uint8 max);
	UFUNCTION(BlueprintCallable) static FGridIndex vectorToGridIndex(const FVector& vec);
	UFUNCTION(BlueprintCallable) static FGridIndex vector2DToGridIndex(const FVector2D& vec);
	UFUNCTION(BlueprintCallable) static FVector gridIndexToVector(const FGridIndex& gi, float z = 0.0f);
	UFUNCTION(BlueprintCallable) static FVector2D gridIndexToVector2D(const FGridIndex& gi);
	UFUNCTION(BlueprintCallable) static FRotator hexDirectionToRotation(const HexDirection& dir);
	UFUNCTION(BlueprintCallable) static bool gridIndicesToHexDirection(const FGridIndex& base, const FGridIndex& other, HexDirection& dir);
	UFUNCTION(BlueprintPure) static FString gridIndexToString(const FGridIndex& gridIndex);
};
