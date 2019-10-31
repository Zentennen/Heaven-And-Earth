#include "Unit.h"
#include "Game.h"
#include "PC.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "..\Public\Unit.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent->SetNetAddressable();
	RootComponent->SetIsReplicated(true);
	modelBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Model Base"));
	modelBase->SetupAttachment(RootComponent);
	modelBase->SetNetAddressable();
	modelBase->SetIsReplicated(true);
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	AGame::addUnit(this);
	position = AGame::vectorToGridIndex(GetActorLocation());
	SetActorLocation(AGame::gridIndexToVector(position));
}

void AUnit::onPathChanged()
{
	auto world = GetWorld();
	if (!world) return;
	auto pc = Cast<APC, APlayerController>(world->GetFirstPlayerController());
	if (!pc) return;
	if (pc->unit == this) {
		pc->updatePathMarkers(path);
		pc->updateMoveMarkers();
	}
}

void AUnit::damageSoldiers(const float& dmg, const uint8& lethality)
{
	float aliveTotal = getAliveTotal();
	damageHealthy(dmg * baseStats.healthy / aliveTotal, lethality);
	damageWounded(dmg * baseStats.wounded / aliveTotal, lethality);
	damageCrippled(dmg * baseStats.crippled / aliveTotal, lethality);
}

void AUnit::damageCrippled(const float& dmg, const uint8& lethality)
{
	baseStats.crippled -= dmg;
	switch (lethality) {
	case 0: break;
	default: baseStats.dead += dmg;
	}
}

void AUnit::damageWounded(const float& dmg, const uint8& lethality)
{
	baseStats.wounded -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: baseStats.crippled += dmg;
	default: baseStats.dead += dmg;
	}
}

void AUnit::damageHealthy(const float& dmg, const uint8& lethality)
{
	baseStats.healthy -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: baseStats.wounded += dmg;
	case 2: baseStats.crippled += dmg;
	default: baseStats.dead += dmg;
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
	if (!HasAuthority()) return;
	ETeleportType tele = ETeleportType::TeleportPhysics;
	SetActorLocation(AGame::gridIndexToVector(position), false, nullptr, tele);
	modelBase->SetRelativeRotation(AGame::hexDirectionToRotation(direction), false, nullptr, tele);
}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AUnit, name);
	DOREPLIFETIME(AUnit, team);
	DOREPLIFETIME(AUnit, position);
	DOREPLIFETIME(AUnit, direction);
	DOREPLIFETIME(AUnit, baseStats);
	DOREPLIFETIME(AUnit, headArmor);
	DOREPLIFETIME(AUnit, torsoArmor);
	DOREPLIFETIME(AUnit, armArmor);
	DOREPLIFETIME(AUnit, legArmor);
	DOREPLIFETIME(AUnit, perks);
	DOREPLIFETIME(AUnit, orders);
	DOREPLIFETIME(AUnit, path);
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
	orderProgress++;
	if (!AGame::canCompleteOrder(this)) return;
	if (orders.Num() == 0) orders.Emplace(Order::Rest);
	auto stats = getStats();
	switch (orders[0]) {
	case Order::Rest: {
		baseStats.formationStrength += stats.formationStrengthRecovery * baseStats.maxFormationStrength;
		if (baseStats.formationStrength > baseStats.maxFormationStrength) baseStats.formationStrength = baseStats.maxFormationStrength;
		baseStats.morale += baseStats.maxMorale * stats.moraleRecovery;
		if (baseStats.morale > baseStats.maxMorale) baseStats.morale = baseStats.maxMorale;
		baseStats.staminaLimit += baseStats.maxStamina * stats.staminaLimitRecovery;
		if (baseStats.staminaLimit > baseStats.maxStamina) baseStats.staminaLimit = baseStats.maxStamina;
		baseStats.stamina += baseStats.staminaLimit * baseStats.staminaRecovery;
		if (baseStats.stamina > baseStats.staminaLimit) baseStats.stamina = baseStats.staminaLimit;
		break;
	}
	case Order::Move: {
		if (!AGame::moveUnit(this)) debugStr("ERROR: AUnit::executeOrder(): Couldn't move");
		if (path.Num() > 0) {
			path.RemoveAt(0);
			onPathChanged();
		}
		else debugStr("AUnit::executeOrder(): Moved without path");
		break;
	}
	case Order::Rotate: {
		int8 a = (uint8)direction;
		a++;
		a = GameLib::posMod<int8>(a, 6);
		direction = (HexDirection)a;
		break;
	}
	case Order::CounterRotate: {
		int8 a = (uint8)direction;
		a--;
		a = GameLib::posMod<int8>(a, 6);
		direction = (HexDirection)a;
		break;
	}
	default: break;
	}
	orderProgress = 0;
	orders.RemoveAt(0);
}

void AUnit::queueMove(const FGridIndex& destination)
{
	if (!AGame::isValidPos(destination)) {
		return;
	}
	HexDirection dir;
	if (!AGame::gridIndicesToHexDirection(getFinalPosition(), destination, dir)) {
		return;
	}
	if (path.Num() == 0) lastDirection = direction;
	auto o = AGame::getRotationOrdersTo(lastDirection, dir);
	orders.Append(o);
	orders.Emplace(Order::Move);
	path.Emplace(destination);
	lastDirection = dir;
	onPathChanged();
	onOrdersChanged();
}

//void AUnit::moveAlongPath(const TArray<FGridIndex>& p, const bool& replaceCurrentOrders)
//{
//	if (p.Num() == 0) return;
//	FGridIndex pos = position;
//	HexDirection dir = direction;
//	HexDirection temp;
//	TArray<Order> newOrders;
//	for (uint8 i = 0; i < p.Num(); i++) {
//		if (AGame::gridIndicesToHexDirection(pos, p[i], temp)) {
//			auto o = getRotationOrdersTo(dir, temp);
//			newOrders.Append(o);
//			newOrders.Emplace(Order::Move);
//			pos = p[i];
//			dir = temp;
//		}
//		else {
//			FString fstr(" received invalid path to move along.");
//			debugFstr(name + fstr);
//			return;
//		}
//	}
//	if (replaceCurrentOrders) replaceOrders(newOrders);
//	else addOrders(newOrders);
//}

FUnitStats AUnit::getStats() const
{
	FUnitStats ret = baseStats;
	float val = perks.Contains(Perk::Swift) ? 1.2f : 1.0f;
	ret.formationStrengthRecovery *= val;
	val = perks.Contains(Perk::Resolute) ? 1.3f : 1.0f;
	ret.moraleRecovery *= val;
	val = perks.Contains(Perk::Tough) ? 1.2f : 1.0f;
	ret.staminaRecovery *= val;
	val = perks.Contains(Perk::Tough) ? 1.4f : 1.0f;
	ret.staminaLimitRecovery *= val;
	val = perks.Contains(Perk::Swift) ? 1.1f : 1.0f;
	ret.movementSpeed *= val;
	val = perks.Contains(Perk::Swift) ? 1.5f : 1.0f;
	ret.rotationSpeed *= val;
	return ret;
}

int32 AUnit::getAliveTotal() const
{
	return baseStats.crippled + baseStats.wounded + baseStats.healthy;
}

int32 AUnit::getPersonTotal() const
{
	return baseStats.healthy + baseStats.wounded + baseStats.crippled + baseStats.dead;
}

int32 AUnit::getMaxMovementPerTurn() const
{
	return getStats().movementSpeed * AGame::getActionsPerTurn();
}

FGridIndex AUnit::getFinalPosition() const
{
	if (path.Num() == 0) return position;
	else return path.Last();
}

TArray<FGridIndex> AUnit::getPossibleMoves() const
{
	return AGame::getRingOfGridIndices(getFinalPosition());
}

