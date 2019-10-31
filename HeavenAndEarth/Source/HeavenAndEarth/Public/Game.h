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

UCLASS()
class HEAVENANDEARTH_API AGame : public AActor
{
	GENERATED_BODY()
protected:
	static AGame* game;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) float timer;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) uint8 counter;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) float actionTime;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = onRepExecuting) bool executing;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 actionsPerTurn;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) TArray<FTileColumn> tiles;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintImplementableEvent) void onRepExecuting();

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FWeapon> weapons;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray <FArmor> armor;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TArray<AUnit*> units;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TArray<AAccount*> accounts;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) TArray<APC*> pcs;
	AGame();
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	static bool addUnit(AUnit* unit);
	static bool addPC(APC* pc);
	static bool canCompleteOrder(AUnit* unit);
	UFUNCTION(BlueprintPure) static bool isValidPos(const FGridIndex& pos);
	UFUNCTION(BlueprintPure) static int32 manhattanDistance(const FGridIndex& start, const FGridIndex& goal);
	UFUNCTION(BlueprintPure) static FTile getTile(const FGridIndex& tilePos);
	UFUNCTION(BlueprintPure) static FGridIndex movementToGridIndex(HexDirection dir, const FGridIndex& pos, int32 distance = 1);
	UFUNCTION(BlueprintPure) static TArray<TileProperty> getTilePropertiesFromFlags(const uint8& flags);
	UFUNCTION(BlueprintPure) static uint8 getFlagsFromTileProperties(const TArray<TileProperty>& props);
	UFUNCTION(BlueprintPure) static FString gridIndexToString(const FGridIndex& gridIndex);
	UFUNCTION(BlueprintPure) static TArray<FGridIndex> getRingOfGridIndices(const FGridIndex& gi, int32 radius = 1);
	UFUNCTION(BlueprintPure) static TArray<FGridIndex> getCircleOfGridIndices(const FGridIndex& gi, int32 radius = 1);
	UFUNCTION(BlueprintPure) static TArray<FTile> getRingOfTiles(const FGridIndex& center, int32 radius = 1);
	UFUNCTION(BlueprintPure) static TArray<FTile> getCircleOfTiles(const FGridIndex& center, int32 radius = 1);
	UFUNCTION(BlueprintPure) static AGame* getGame();
	UFUNCTION(BlueprintPure) static int32 getActionsPerTurn();
	UFUNCTION(BlueprintPure) static bool isExecuting();
	UFUNCTION(BlueprintCallable) static void executeTurn();
	UFUNCTION(BlueprintCallable) static bool setTile(const FTile& t);
	UFUNCTION(BlueprintCallable) static TArray<FGridIndex> getPathAsGridIndices(FGridIndex start, FGridIndex goal);
	UFUNCTION(BlueprintCallable) static int32 roll(uint8 num, uint8 max);
	UFUNCTION(BlueprintCallable) static FGridIndex vectorToGridIndex(const FVector& vec);
	UFUNCTION(BlueprintCallable) static FGridIndex vector2DToGridIndex(const FVector2D& vec);
	UFUNCTION(BlueprintCallable) static FVector gridIndexToVector(const FGridIndex& gi, float z = 0.0f);
	UFUNCTION(BlueprintCallable) static FVector2D gridIndexToVector2D(const FGridIndex& gi);
	UFUNCTION(BlueprintCallable) static FRotator hexDirectionToRotation(const HexDirection& dir);
	UFUNCTION(BlueprintCallable) static bool gridIndicesToHexDirection(const FGridIndex& base, const FGridIndex& other, HexDirection& dir);
	UFUNCTION(BlueprintCallable) static bool isAdjacent(const FGridIndex& base, const FGridIndex& other);
	UFUNCTION(BlueprintCallable) static bool moveUnit(AUnit* unit);
	UFUNCTION(BlueprintCallable) static bool canLogin(APC* pc, const FString& username, const FString& password);
	UFUNCTION(BlueprintCallable) static bool createAccount(APC* pc, const FString& username, const FString& password);
	UFUNCTION(BlueprintCallable) static LoginResult login(APC* pc, const FString& username, const FString& password);
	UFUNCTION(BlueprintCallable) static TArray<Order> getRotationOrdersTo(const HexDirection& start, const HexDirection& goal);
};
