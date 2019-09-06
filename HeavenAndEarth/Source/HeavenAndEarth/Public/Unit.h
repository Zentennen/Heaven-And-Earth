#pragma once

#include "PC.h"
#include "Armor.h"
#include "Unit.generated.h"

UENUM(BlueprintType)
enum class Order : uint8 {
	Move, Rotate, CounterRotate, Rest
};

UCLASS()
class HEAVENANDEARTH_API AUnit : public AActor
{
	GENERATED_BODY()
private:

protected:
	//formation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 maxFormationStrength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 formationStrength;
	//health
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 personTotal;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 dead;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 crippled;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 wounded;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 healthy;
	//morale
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 maxMorale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 morale;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool wavering;
	//stamina
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 maxStamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 stamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 staminaLimit;
	//armor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor headArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor torsoArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor armArmor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FArmor legArmor;
	//stats
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 baseDamage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 basePower;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) TArray<Perk> perks;
	//functions
	virtual void BeginPlay() override;
	UFUNCTION() void damageSoldiers(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageCrippled(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageWounded(const float& dmg, const uint8& lethality);
	UFUNCTION() void damageHealthy(const float& dmg, const uint8& lethality);
	UFUNCTION() void addOrders(TArray<Order> newOrders);
	UFUNCTION() void replaceOrders(TArray<Order> newOrders);
	UFUNCTION(BlueprintImplementableEvent) void onOrdersChanged();
public:	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) FString name;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) APC* controller;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) bool team;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) FGridIndex position;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) HexDirection direction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 movementSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = onOrdersChanged) TArray<Order> orders;
	AUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void beginTurn();
	void endTurn();
	UFUNCTION() void attack(const AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio);
	UFUNCTION() void executeOrder();
	UFUNCTION() void moveAlongPath( const TArray<FGridIndex>& path, const bool& replaceCurrentOrders);
	UFUNCTION(BlueprintCallable) static TArray<Order> getRotationOrdersTo(const HexDirection& start, const HexDirection& goal);
	UFUNCTION(BlueprintImplementableEvent) void select();
	UFUNCTION(BlueprintImplementableEvent) void unselect();
};
