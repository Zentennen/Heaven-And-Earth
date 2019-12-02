#pragma once
#include "Config.h"
#include "Unit.generated.h"

class AAccount;
class UUnitSave;

UENUM(BlueprintType)
enum class Order : uint8 {
	Rest, Move, Rotate, CounterRotate
};

UENUM(BlueprintType)
enum class BodyPart : uint8 {
	Head, Torso, Arms, Legs
};

UENUM(BlueprintType)
enum class Perk : uint8 {
	Ruthless, Swift, Resolute, Precise, Steadfast, Stalwart, Tough
};

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

USTRUCT(BlueprintType)
struct FUnitStats {
	GENERATED_BODY()
	//formation
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 maxFormationStrength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 formationStrength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float formationStrengthRecovery;
	//health
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 dead;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 crippled;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 wounded;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 healthy;
	//morale
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 maxMorale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 morale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float moraleRecovery;
	//stamina
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 maxStamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 stamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 staminaLimit;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float staminaRecovery;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float staminaLimitRecovery;
	//stats
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float damageMult;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float powerMult;
	//movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 movementSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 rotationSpeed;
	//armor
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FArmor headArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FArmor torsoArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FArmor armArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FArmor legArmor;
	//perks
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<Perk> perks;
};

UCLASS()
class HEAVENANDEARTH_API AUnit : public AActor
{
	GENERATED_BODY()
private:
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 id = -1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) FUnitStats baseStats;
	virtual void BeginPlay() override;
	void damageSoldiers(const float& dmg, const uint8& lethality);
	void damageCrippled(const float& dmg, const uint8& lethality);
	void damageWounded(const float& dmg, const uint8& lethality);
	void damageHealthy(const float& dmg, const uint8& lethality);
	void addOrders(const TArray<Order>& newOrders);
	UFUNCTION() void onPathChanged();
	UFUNCTION(BlueprintImplementableEvent) void onOrdersChanged();
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) AAccount* account;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) bool team;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) FGridIndex position;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) HexDirection direction;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = onOrdersChanged) TArray<Order> orders;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = onPathChanged) TArray<FGridIndex> path;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) USceneComponent* modelBase;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) uint8 orderProgress;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) HexDirection lastDirection;
	UUnitSave* save;
	AUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void beginTurn();
	void endTurn();
	void attack(const AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio);
	void executeOrder();
	void queueMove(const FGridIndex& destination);
	void saveUnit();
	void init(const FString& pName, const FString& accountUsername, const FUnitStats& pStats, const FGridIndex& pPosition, const HexDirection& pDirection);
	void init(const FString& pName, AAccount* pAccount, const FUnitStats& pStats, const FGridIndex& pPosition, const HexDirection& pDirection);
	void load(const int32& pId);
	//UFUNCTION() void moveAlongPath(const TArray<FGridIndex>& p, const bool& replaceCurrentOrders);
	UFUNCTION(BlueprintPure) FUnitStats getStats() const;
	UFUNCTION(BlueprintPure) FString getSaveName() const;
	UFUNCTION(BlueprintPure) int32 getAliveTotal() const;
	UFUNCTION(BlueprintPure) int32 getPersonTotal() const;
	UFUNCTION(BlueprintPure) int32 getMaxMovementPerTurn() const;
	UFUNCTION(BlueprintPure) FGridIndex getFinalPosition() const;
	UFUNCTION(BlueprintPure) bool isMyAccount(AAccount* acc);
	UFUNCTION(BlueprintCallable) void resetOrders();
	UFUNCTION(BlueprintCallable) TArray<FGridIndex> getPossibleMoves() const;
	UFUNCTION(BlueprintImplementableEvent) void select();
	UFUNCTION(BlueprintImplementableEvent) void unselect();
};
