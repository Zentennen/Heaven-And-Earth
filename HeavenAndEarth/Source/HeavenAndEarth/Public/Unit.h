// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameLib.h"
#include "Armor.h"
#include "Unit.generated.h"

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
	//tags
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool critical;
	//functions
	virtual void BeginPlay() override;
	UFUNCTION() void damage(int32 dmg, uint8 lethality);
public:	
	AUnit();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	UFUNCTION() void attack(AUnit* attacker, BodyPart bodyPart, int32 dmg, int32 power, uint8 lethality);
};
