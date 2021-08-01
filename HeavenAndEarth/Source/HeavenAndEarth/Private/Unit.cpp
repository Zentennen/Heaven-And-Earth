#include "Unit.h"
#include "Game.h"
#include "PC.h"
#include "Engine.h"
#include "CampaignSave.h"
#include "Net/UnrealNetwork.h"
#include "..\Public\Unit.h"

void FArmorLayer::damage(FArmorLayer layer, uint32 dmg)
{
	layer.condition -= dmg;
	if (layer.condition < 0) layer.condition = 0;
	layer.protection = layer.baseProtection * layer.condition / layer.maxCondition;
}

void FArmorLayer::repair(FArmorLayer layer, uint32 rep)
{
	layer.condition += rep;
	if (layer.condition > layer.maxCondition) layer.condition = layer.maxCondition;
	layer.protection = layer.baseProtection * layer.condition / layer.maxCondition;
}

void FArmorLayer::repairPercentage(FArmorLayer layer, uint8 percent)
{
	repair(layer, layer.maxCondition * percent / 100);
}

AUnit::AUnit()
{
	bReplicates = true;
	SetReplicateMovement(true);
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
	//if (!HasAuthority()) return;
	//position = AGame::vectorToGridIndex(GetActorLocation());
	//SetActorLocation(AGame::gridIndexToVector(position));
}

void AUnit::onPathChanged()
{
	auto world = GetWorld();
	if (!world) return;
	auto pc = Cast<APC, APlayerController>(world->GetFirstPlayerController());
	if (!pc) return;
	if (pc->selected == this) {
		pc->updatePathMarkers(PATH);
		pc->updateMoveMarkers();
	}
}

void AUnit::damageSoldiers(const float& dmg, const uint8& lethality)
{
	float aliveTotal = getAliveTotal();
	damageHealthy(dmg * data.baseStats.healthy / aliveTotal, lethality);
	damageWounded(dmg * data.baseStats.wounded / aliveTotal, lethality);
	damageCrippled(dmg * data.baseStats.crippled / aliveTotal, lethality);
}

void AUnit::damageCrippled(const float& dmg, const uint8& lethality)
{
	data.baseStats.crippled -= dmg;
	switch (lethality) {
	case 0: break;
	default: data.baseStats.dead += dmg;
	}
}

void AUnit::damageWounded(const float& dmg, const uint8& lethality)
{
	data.baseStats.wounded -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: data.baseStats.crippled += dmg;
	default: data.baseStats.dead += dmg;
	}
}

void AUnit::damageHealthy(const float& dmg, const uint8& lethality)
{
	data.baseStats.healthy -= dmg;
	switch (lethality) {
	case 0: break;
	case 1: data.baseStats.wounded += dmg;
	case 2: data.baseStats.crippled += dmg;
	default: data.baseStats.dead += dmg;
	}
}

void AUnit::completeOrder()
{
	if (ORDERS.Num() == 0) return;
	data.orderProgress = 0;
	ORDERS.RemoveAt(0);
	onOrdersChanged();
}

void AUnit::addOrders(const TArray<Order>& newOrders)
{
	if (!canAddOrders(newOrders)) return;
	ORDERS.Append(newOrders);
	bool changedPath = false;
	for (auto i : newOrders) {
		auto dir = data.lastDirection;
		switch (i) {
		case Order::Rotate:
			AGame::rotateClockwise(dir);
			data.lastDirection = dir;
			break;
		case Order::CounterRotate:
			AGame::rotateCounterClockwise(dir);
			data.lastDirection = dir;
			break;
		case Order::Move:
			PATH.Emplace(AGame::movementToGridIndex(getFinalDirection(), getFinalPosition()));
			changedPath = true;
			break;
		default:
			break;
		}
	}
	onOrdersChanged();
	if (changedPath) onPathChanged();
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AUnit, data);
	DOREPLIFETIME(AUnit, account);
	DOREPLIFETIME(AUnit, orders);
	DOREPLIFETIME(AUnit, path);
}

void AUnit::setPosition(const FGridIndex& pos)
{
	data.position = pos;
	SetActorLocation(AGame::gridIndexToVector(pos));
}

TArray<Order> AUnit::getOrdersToMoveTo(const FGridIndex& pos) const
{
	TArray<Order> ret;
	ret.Empty();
	HexDirection dir;
	if (!AGame::gridIndicesToHexDirection(getFinalPosition(), pos, dir)) return ret;
	ret = AGame::getRotationOrdersTo(getFinalDirection(), dir);
	ret.Emplace(Order::Move);
	return ret;
}

bool AUnit::canAddOrders(const TArray<Order>& newOrders) const
{
	return ORDERS.Num() + newOrders.Num() <= HAE::maxOrders;
}

void AUnit::beginTurn()
{

}

void AUnit::endTurn()
{

}

void AUnit::attack(AUnit* attacker, const BodyPart& bodyPart, float dmg, float power, uint8 lethality, const float& critRatio)
{
	if (critRatio > 0.0f) {
		attack(attacker, bodyPart, dmg * critRatio, power, lethality, 0.0f);
	}
	FArmor armor;
	auto baseStats = getStats();
	switch (bodyPart) {
	case BodyPart::Head:
		armor = baseStats.headArmor;
		lethality += 1;
		break;
	case BodyPart::Torso:
		armor = baseStats.torsoArmor;
		power *= HAE::torsoPowerMult;
		break;
	case BodyPart::Arms:
		armor = baseStats.armArmor;
		break;
	case BodyPart::Legs:
		armor = baseStats.legArmor;
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
			FArmorLayer::damage(layer, d * (p + HAE::conditionDamagePowerBonus));
		}
		int32 critThreshold = attacker->getStats().perks.Contains(Perk::Ruthless) ? HAE::strongHitTalentPowerNeeded : HAE::strongHitPowerNeeded;
		if (p >= critThreshold) {
			l += 1;
		}
		if (p >= 0.0f) damageSoldiers(d, l);
	}
}

void AUnit::executeOrder()
{
	if (ORDERS.Num() == 0) ORDERS.Emplace(Order::Rest);
	auto stats = getStats();
	switch (ORDERS[0]) {
	case Order::Rest: {
		data.baseStats.formationStrength += stats.formationStrengthRecovery * stats.maxFormationStrength;
		if (data.baseStats.formationStrength > stats.maxFormationStrength) data.baseStats.formationStrength = stats.maxFormationStrength;
		data.baseStats.morale += stats.maxMorale * stats.moraleRecovery;
		if (data.baseStats.morale > stats.maxMorale) data.baseStats.morale = stats.maxMorale;
		data.baseStats.staminaLimit += stats.maxStamina * stats.staminaLimitRecovery;
		if (data.baseStats.staminaLimit > stats.maxStamina) data.baseStats.staminaLimit = stats.maxStamina;
		data.baseStats.stamina += stats.staminaLimit * stats.staminaRecovery;
		if (data.baseStats.stamina > stats.staminaLimit) data.baseStats.stamina = stats.staminaLimit;
		completeOrder();
		return;
	}
	case Order::Move: {
		data.orderProgress += stats.movementSpeed;
		if (AGame::getTile(AGame::movementToGridIndex(data.direction, data.position)).cost > data.orderProgress) return;
		if (AGame::moveUnit(this)) {
			if (PATH.Num() > 0) {
				PATH.RemoveAt(0);
				onPathChanged();
			}
			else debugStr("AUnit::executeOrder(): Moved without path");
			completeOrder();
			return;
		}
		break;
	}
	case Order::Rotate: {
		data.orderProgress += stats.rotationSpeed;
		if (HAE::rotationNeeded > data.orderProgress) return;
		int8 a = (uint8)data.direction;
		a++;
		a = HAE::posMod<int8>(a, 6);
		data.direction = (HexDirection)a;
		modelBase->SetWorldRotation(AGame::hexDirectionToRotation(data.direction));
		completeOrder();
		return;
	}
	case Order::CounterRotate: {
		data.orderProgress += stats.rotationSpeed;
		if (HAE::rotationNeeded > data.orderProgress) return;
		int8 a = (uint8)data.direction;
		a--;
		a = HAE::posMod<int8>(a, 6);
		data.direction = (HexDirection)a;
		modelBase->SetWorldRotation(AGame::hexDirectionToRotation(data.direction));
		completeOrder();
		break;
	}
	default: return;
	}
}

void AUnit::save(UCampaignSave* saveGame)
{
	saveGame->unitData.Emplace(data);
	saveGame->unitOrders.Emplace(orders);
	saveGame->unitPaths.Emplace(path);
}

FUnitStats AUnit::getStats() const
{
	FUnitStats ret = data.baseStats;
	float val = data.baseStats.perks.Contains(Perk::Swift) ? 1.2f : 1.0f;
	ret.formationStrengthRecovery *= val;
	val = data.baseStats.perks.Contains(Perk::Resolute) ? 1.3f : 1.0f;
	ret.moraleRecovery *= val;
	val = data.baseStats.perks.Contains(Perk::Tough) ? 1.2f : 1.0f;
	ret.staminaRecovery *= val;
	val = data.baseStats.perks.Contains(Perk::Tough) ? 1.4f : 1.0f;
	ret.staminaLimitRecovery *= val;
	val = data.baseStats.perks.Contains(Perk::Swift) ? 1.1f : 1.0f;
	ret.movementSpeed *= val;
	val = data.baseStats.perks.Contains(Perk::Swift) ? 1.5f : 1.0f;
	ret.rotationSpeed *= val;
	return ret;
}

FUnitData AUnit::getData() const
{
	auto d = data;
	d.baseStats = getStats();
	return d;
}

int32 AUnit::getAliveTotal() const
{
	auto baseStats = getStats();
	return baseStats.crippled + baseStats.wounded + baseStats.healthy;
}

int32 AUnit::getPersonTotal() const
{
	auto baseStats = getStats();
	return baseStats.healthy + baseStats.wounded + baseStats.crippled + baseStats.dead;
}

int32 AUnit::getMaxMovementPerTurn() const
{
	return getStats().movementSpeed * AGame::getActionsPerTurn();
}

FGridIndex AUnit::getFinalPosition() const
{
	if (PATH.Num() == 0) return data.position;
	else return PATH.Last();
}

HexDirection AUnit::getFinalDirection() const
{
	if (ORDERS.Num() == 0) return data.direction;
	else return data.lastDirection;
}

bool AUnit::isMyAccount(AAccount* acc) const
{
	if (!acc) return false;
	else if (!account) return false;
	else return acc == account;
}

bool AUnit::isAcceptingCommandsFrom(const APC* pc) const
{
	if (!pc) {
		debugStr("AUnit::isAcceptingCommandsFrom(): pc is null");
		return false;
	}
	if (!AGame::isAcceptingCommands()) return false;
	if (pc->getIsGM()) return true;
	else return isMyAccount(pc->account);
}

bool AUnit::isAcceptingOrdersFrom(const APC* pc) const
{
	if (!pc) {
		debugStr("AUnit::isAcceptingOrdersFrom(): pc is null");
		return false;
	}
	else if (!isAcceptingCommandsFrom(pc)) return false;
	else return true;
}

bool AUnit::acceptsOrders(const TArray<Order>& newOrders, const APC* pc) const
{
	return isAcceptingOrdersFrom(pc) && canAddOrders(newOrders);
}

void AUnit::resetOrders()
{
	ORDERS.Empty();
	ORDERS.Empty();
	data.orderProgress = 0;
	onOrdersChanged();
	onPathChanged();
}

TArray<FGridIndex> AUnit::getPossibleMoves() const
{
	TArray<FGridIndex> ret;
	ret.Empty();
	auto ring = AGame::getRingOfGridIndices(getFinalPosition());
	for (auto i : ring) {
		if (getOrdersToMoveTo(i).Num() + ORDERS.Num() <= HAE::maxOrders) ret.Emplace(i);
	}
	return ret;
}

void AUnit::init(const FUnitData& d, AAccount* acc, const int32& i)
{
	if (id != -1) {
		debugStr("AUnit::init(): unit was already initialized");
		return;
	}
	id = i;
	data = d;
	account = acc;
	data.lastDirection = data.direction;
	AGame::addUnitToMap(this, data.position);
	ETeleportType tele = ETeleportType::TeleportPhysics;
	modelBase->SetWorldRotation(AGame::hexDirectionToRotation(data.direction), false, nullptr, tele);
	SetActorLocation(AGame::gridIndexToVector(data.position), false, nullptr, tele);
}

void AUnit::load(const int32& i, UCampaignSave* saveGame)
{
	id = i;
	data = saveGame->unitData[i];
	orders = saveGame->unitOrders[i];
	path = saveGame->unitPaths[i];
	account = AGame::getAccount(data.accountUsername);
	AGame::addUnitToMap(this, data.position);
	ETeleportType tele = ETeleportType::TeleportPhysics;
	modelBase->SetWorldRotation(AGame::hexDirectionToRotation(data.direction), false, nullptr, tele);
	SetActorLocation(AGame::gridIndexToVector(data.position), false, nullptr, tele);
	onOrdersChanged();
	onPathChanged();
}

