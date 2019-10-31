#pragma once
#include "Armor.h"
#include "Unit.generated.h"

class AAccount;

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
};

UCLASS()
class HEAVENANDEARTH_API AUnit : public AActor
{
	GENERATED_BODY()
private:

protected:
	UPROPERTY(EditAnywhere, Replicated) FUnitStats baseStats;
	virtual void BeginPlay() override;
	UFUNCTION() void onPathChanged();
	UFUNCTION() void damageSoldiers(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageCrippled(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageWounded(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageHealthy(const float& dmg, const uint8& lethality);
	UFUNCTION() void addOrders(TArray<Order> newOrders);
	UFUNCTION() void replaceOrders(TArray<Order> newOrders);
	UFUNCTION(BlueprintImplementableEvent) void onOrdersChanged();
public:	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) uint8 orderProgress;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) HexDirection lastDirection;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) AAccount* account;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) USceneComponent* modelBase;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor headArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor torsoArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor armArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor legArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) TArray<Perk> perks;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) HexDirection direction;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated) bool team;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated) FGridIndex position;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = onOrdersChanged) TArray<Order> orders;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = onPathChanged) TArray<FGridIndex> path;
	AUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void beginTurn();
	void endTurn();
	void attack(const AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio);
	void executeOrder();
	void queueMove(const FGridIndex& destination);
	//UFUNCTION() void moveAlongPath(const TArray<FGridIndex>& p, const bool& replaceCurrentOrders);
	UFUNCTION(BlueprintPure) FUnitStats getStats() const;
	UFUNCTION(BlueprintPure) int32 getAliveTotal() const;
	UFUNCTION(BlueprintPure) int32 getPersonTotal() const;
	UFUNCTION(BlueprintPure) int32 getMaxMovementPerTurn() const;
	UFUNCTION(BlueprintPure) FGridIndex getFinalPosition() const;
	UFUNCTION(BlueprintCallable) TArray<FGridIndex> getPossibleMoves() const;
	UFUNCTION(BlueprintImplementableEvent) void select();
	UFUNCTION(BlueprintImplementableEvent) void unselect();
};
