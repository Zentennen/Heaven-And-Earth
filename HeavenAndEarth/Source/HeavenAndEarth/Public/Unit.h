#pragma once
#include "HAE.h"
#include "Unit.generated.h"

#define PATH path.path
#define ORDERS orders.orders

class AAccount;
class APC;
class UCampaignSave;

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 protection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 baseProtection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 condition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 maxCondition;
	//FArmorLayer operator=(const FArmorLayer& other) {
	//	protection = other.protection;
	//	baseProtection = other.baseProtection;
	//	condition = other.condition;
	//	maxCondition = other.maxCondition;
	//	return *this;
	//}
	static void damage(FArmorLayer layer, uint32 dmg);
	static void repair(FArmorLayer layer, uint32 rep);
	static void repairPercentage(FArmorLayer layer, uint8 percent);
};

USTRUCT(BlueprintType)
struct FArmorPiece {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 coverage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArmorLayer> layers;
	//FArmorPiece operator=(const FArmorPiece& other) {
	//	coverage = other.coverage;
	//	layers = other.layers;
	//	return *this;
	//}
};

USTRUCT(BlueprintType)
struct FArmor {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FArmorPiece> pieces;
	//FArmor operator=(const FArmor& other) {
	//	//name = other.name;
	//	pieces = other.pieces;
	//	return *this;
	//}
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

USTRUCT(BlueprintType)
struct FUnitData {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString accountUsername;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float orderProgress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FUnitStats baseStats;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FGridIndex position;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) HexDirection direction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool team;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) HexDirection lastDirection;
};

USTRUCT(BlueprintType)
struct FOrderList {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<Order> orders;
};

USTRUCT(BlueprintType)
struct FPath {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FGridIndex> path;
};

UCLASS()
class HEAVENANDEARTH_API AUnit : public AActor
{
	GENERATED_BODY()
private:
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 id = -1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) FUnitData data;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated) AAccount* account;
	virtual void BeginPlay() override;
	void damageSoldiers(const float& dmg, const uint8& lethality);
	void damageCrippled(const float& dmg, const uint8& lethality);
	void damageWounded(const float& dmg, const uint8& lethality);
	void damageHealthy(const float& dmg, const uint8& lethality);
	void completeOrder();
	UFUNCTION() void onPathChanged();
	UFUNCTION(BlueprintImplementableEvent) void onOrdersChanged();
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = onOrdersChanged) FOrderList orders;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = onPathChanged) FPath path;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) USceneComponent* modelBase;
	AUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void setPosition(const FGridIndex& pos);
	TArray<Order> getOrdersToMoveTo(const FGridIndex& pos) const;
	bool canAddOrders(const TArray<Order>& newOrders) const;
	bool isAcceptingCommandsFrom(const APC* pc) const;
	bool isAcceptingOrdersFrom(const APC* pc) const;
	bool acceptsOrders(const TArray<Order>& newOrders, const APC* pc) const;
	void beginTurn();
	void endTurn();
	void attack(AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio);
	void executeOrder();
	void save(UCampaignSave* saveGame);
	void init(const FUnitData& d, AAccount* acc, const int32& i);
	void load(const int32& i, UCampaignSave* saveGame);
	//UFUNCTION() void moveAlongPath(const TArray<FGridIndex>& p, const bool& replaceCurrentOrders);
	UFUNCTION(BlueprintPure) FUnitStats getStats() const;
	UFUNCTION(BlueprintPure) FUnitData getData() const;
	UFUNCTION(BlueprintPure) int32 getAliveTotal() const;
	UFUNCTION(BlueprintPure) int32 getPersonTotal() const;
	UFUNCTION(BlueprintPure) int32 getMaxMovementPerTurn() const;
	UFUNCTION(BlueprintPure) FGridIndex getFinalPosition() const;
	UFUNCTION(BlueprintPure) HexDirection getFinalDirection() const;
	UFUNCTION(BlueprintPure) bool isMyAccount(AAccount* acc) const;
	UFUNCTION(BlueprintCallable) void resetOrders();
	UFUNCTION(BlueprintCallable) void addOrders(const TArray<Order>& newOrders);
	UFUNCTION(BlueprintCallable) TArray<FGridIndex> getPossibleMoves() const;
	UFUNCTION(BlueprintImplementableEvent) void select();
	UFUNCTION(BlueprintImplementableEvent) void unselect();
};
