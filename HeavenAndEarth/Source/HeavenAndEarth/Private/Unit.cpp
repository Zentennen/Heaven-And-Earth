#include "Unit.h"
#include "UnrealNetwork.h"
#include "..\Public\Unit.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnit::damageSoldiers(float dmg, uint8 lethality)
{
	float aliveTotal = crippled + wounded + healthy;
	damageHealthy(dmg * healthy / aliveTotal, lethality);
	damageWounded(dmg * wounded / aliveTotal, lethality);
	damageCrippled(dmg * crippled / aliveTotal, lethality);
}

void AUnit::damageCrippled(float dmg, uint8 lethality)
{
	crippled -= dmg;
	switch (lethality) {
	case 0: break;
	default: dead += dmg;
	}
}

void AUnit::damageWounded(float dmg, uint8 lethality)
{
	wounded -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: crippled += dmg;
	default: dead += dmg;
	}
}

void AUnit::damageHealthy(float dmg, uint8 lethality)
{
	healthy -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: wounded += dmg;
	case 2: crippled += dmg;
	default: dead += dmg;
	}
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AUnit, dead);
	DOREPLIFETIME(AUnit, crippled);
	DOREPLIFETIME(AUnit, wounded);
	DOREPLIFETIME(AUnit, healthy);
	DOREPLIFETIME(AUnit, personTotal);
	DOREPLIFETIME(AUnit, morale);
	DOREPLIFETIME(AUnit, maxMorale);
	DOREPLIFETIME(AUnit, wavering);
	DOREPLIFETIME(AUnit, stamina);
	DOREPLIFETIME(AUnit, staminaLimit);
	DOREPLIFETIME(AUnit, maxStamina);
}

void AUnit::attack(AUnit* attacker, BodyPart bodyPart, float dmg, float power, uint8 lethality)
{
	FArmor armor;
	switch (bodyPart) {
	case BodyPart::Head: 
		armor = headArmor;
		lethality += 1;
		break;
	case BodyPart::Torso: 
		armor = torsoArmor;
		power += 1;
		break;
	case BodyPart::Arms:
		armor = armArmor;
		break;
	case BodyPart::Legs:
		armor = legArmor;
		break;
	default:
		return;
	}
	for (uint8 i = 0; i < armor.pieces.Num(); i++) {
		float d = dmg * armor.pieces[i].coverage;
		float p = power;
		uint8 l = lethality;
		for (uint8 j = 0; j < armor.pieces[i].layers.Num(); j++) {
			FArmorLayer layer = armor.pieces[i].layers[j];
			p -= layer.protection;
			FArmorLayer::damage(layer, 
				FMath::Max(d * FMath::Pow(p + GameLib::conditionDamagePowerBonus, GameLib::conditionDamageExponent), GameLib::maxConditionPowerMult));
		}
		int32 critThreshold = attacker->critical ? GameLib::critTalentPowerNeeded : GameLib::critPowerNeeded;
		if (p >= critThreshold) {
			l += 1;
		}
		if(p >= 0.0f) damageSoldiers(d, l);
	}
}

