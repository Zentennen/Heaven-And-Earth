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

void AUnit::damage(int32 dmg, uint8 lethality)
{
	morale -= dmg * lethality;
	if (morale <= 0) morale = 0;
	float aliveTotal = crippled + wounded + healthy;
	//healthy
	healthy -= dmg * healthy / aliveTotal;
	switch (lethality) {
	case 0: break;
	case 1: wounded += dmg * healthy / aliveTotal; break;
	case 2: crippled += dmg * healthy / aliveTotal; break;
	default: dead += dmg * healthy / aliveTotal; break;
	}
	//wounded
	wounded -= dmg * wounded / aliveTotal;
	switch (lethality) {
	case 0: break;
	case 1: crippled += dmg * wounded / aliveTotal; break;
	default: dead += dmg * wounded / aliveTotal; break;
	}
	//crippled
	crippled -= dmg * crippled / aliveTotal;
	switch (lethality) {
	case 0: break;
	default: dead += dmg * crippled / aliveTotal; break;
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

void AUnit::attack(AUnit* attacker, BodyPart bodyPart, int32 dmg, int32 power, uint8 lethality)
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
		int32 d = dmg * armor.pieces[i].coverage;
		int32 p = power;
		uint8 l = lethality;
		for (uint8 j = 0; j < armor.pieces[i].layers.Num(); j++) {
			FArmorLayer layer = armor.pieces[i].layers[j];
			p -= layer.protection;
			FArmorLayer::damage(layer, FMath::Max(p, MAX_CONDITION_POWER_MULT));
		}
		int32 critThreshold = attacker->critical ? CRIT_TALENT_POWER_NEEDED : CRIT_POWER_NEEDED;
		if (p >= critThreshold) {
			l += 1;
		}
		if(p >= 0) damage(d, l);
	}
}

