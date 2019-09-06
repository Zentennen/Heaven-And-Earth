#include "Unit.h"
#include "Game.h"
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
	if (!HasAuthority()) return;
	AGame::addUnit(this);
	position = AGame::vectorToGridIndex(GetActorLocation());
	SetActorLocation(AGame::gridIndexToVector(position));
}

void AUnit::damageSoldiers(const float& dmg, const uint8& lethality)
{
	float aliveTotal = crippled + wounded + healthy;
	damageHealthy(dmg * healthy / aliveTotal, lethality);
	damageWounded(dmg * wounded / aliveTotal, lethality);
	damageCrippled(dmg * crippled / aliveTotal, lethality);
}

void AUnit::damageCrippled(const float& dmg, const uint8& lethality)
{
	crippled -= dmg;
	switch (lethality) {
	case 0: break;
	default: dead += dmg;
	}
}

void AUnit::damageWounded(const float& dmg, const uint8& lethality)
{
	wounded -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: crippled += dmg;
	default: dead += dmg;
	}
}

void AUnit::damageHealthy(const float& dmg, const uint8& lethality)
{
	healthy -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: wounded += dmg;
	case 2: crippled += dmg;
	default: dead += dmg;
	}
}

void AUnit::addOrders(TArray<Order> newOrders)
{
	orders.Append(newOrders);
	onOrdersChanged();
}

void AUnit::replaceOrders(TArray<Order> newOrders)
{
	orders = newOrders;
	onOrdersChanged();
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AUnit, name);
	DOREPLIFETIME(AUnit, team);
	DOREPLIFETIME(AUnit, controller);
	DOREPLIFETIME(AUnit, position);
	DOREPLIFETIME(AUnit, direction);
	DOREPLIFETIME(AUnit, movementSpeed);
	DOREPLIFETIME(AUnit, maxFormationStrength);
	DOREPLIFETIME(AUnit, formationStrength);
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
	DOREPLIFETIME(AUnit, headArmor);
	DOREPLIFETIME(AUnit, torsoArmor);
	DOREPLIFETIME(AUnit, armArmor);
	DOREPLIFETIME(AUnit, legArmor);
	DOREPLIFETIME(AUnit, baseDamage);
	DOREPLIFETIME(AUnit, basePower);
	DOREPLIFETIME(AUnit, perks);
}

void AUnit::beginTurn()
{

}

void AUnit::endTurn()
{

}

void AUnit::attack(const AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio)
{
	if (critRatio > 0.0f) {
		attack(attacker, bodyPart, dmg * critRatio, power, lethality, 0.0f);
	}
	FArmor armor;
	switch (bodyPart) {
	case BodyPart::Head: 
		armor = headArmor;
		lethality += 1;
		break;
	case BodyPart::Torso: 
		armor = torsoArmor;
		power *= GameLib::torsoPowerMult;
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
			FArmorLayer::damage(layer, d * (p + GameLib::conditionDamagePowerBonus));
		}
		int32 critThreshold = attacker->perks.Contains(Perk::Ruthless) ? GameLib::strongHitTalentPowerNeeded : GameLib::strongHitPowerNeeded;
		if (p >= critThreshold) {
			l += 1;
		}
		if(p >= 0.0f) damageSoldiers(d, l);
	}
}

void AUnit::executeOrder()
{
	
}

void AUnit::moveAlongPath(const TArray<FGridIndex>& path, const bool& replaceCurrentOrders)
{
	if (path.Num() == 0) return;
	FGridIndex pos = position;
	HexDirection dir = direction;
	HexDirection temp;
	TArray<Order> newOrders;
	for (auto i : path) {
		if (AGame::gridIndicesToHexDirection(pos, i, temp)) {
			auto o = getRotationOrdersTo(dir, temp);
			newOrders.Append(o);
			newOrders.Emplace(Order::Move);
			pos = i;
			dir = temp;
		}
		else {
			FString fstr(" received invalid path to move along.");
			debugFstr(name + fstr);
			return;
		}
	}
	if (replaceCurrentOrders) replaceOrders(newOrders);
	else addOrders(newOrders);
}

TArray<Order> AUnit::getRotationOrdersTo(const HexDirection& start, const HexDirection& goal)
{
	TArray<Order> ret;
	ret.Empty();
	int32 diff = ((int32)goal - (int32)start) % 6;
	while (diff > 0) {
		ret.Emplace(Order::Rotate);
		diff--;
	}
	while (diff < 0) {
		ret.Emplace(Order::CounterRotate);
		diff++;
	}
	return ret;
}
