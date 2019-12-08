#include "Unit.h"
#include "Game.h"
#include "UnitSave.h"
#include "Engine.h"
#include "UnrealNetwork.h"
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
	position = AGame::vectorToGridIndex(GetActorLocation());
	SetActorLocation(AGame::gridIndexToVector(position));
}

void AUnit::onPathChanged()
{
	auto world = GetWorld();
	if (!world) return;
	auto pc = Cast<APC, APlayerController>(world->GetFirstPlayerController());
	if (!pc) return;
	if (pc->selected == this) {
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

void AUnit::addOrders(const TArray<Order>& newOrders)
{
	if (!canAddOrders(newOrders)) return;
	orders.Append(newOrders);
	onOrdersChanged();
	bool changedPath = false;
	for (auto i : newOrders) {
		switch (i) {
		case Order::Rotate:
			AGame::rotateClockwise(lastDirection);
			break;
		case Order::CounterRotate:
			AGame::rotateCounterClockwise(lastDirection);
			break;
		case Order::Move:
			path.Emplace(AGame::movementToGridIndex(getFinalDirection(), getFinalPosition()));
			changedPath = true;
			break;
		default:
			break;
		}
	}
	if (changedPath) onPathChanged();
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
	DOREPLIFETIME(AUnit, account);
	DOREPLIFETIME(AUnit, team);
	DOREPLIFETIME(AUnit, position);
	DOREPLIFETIME(AUnit, direction);
	DOREPLIFETIME(AUnit, baseStats);
	DOREPLIFETIME(AUnit, orders);
	DOREPLIFETIME(AUnit, path);
	DOREPLIFETIME(AUnit, lastDirection);
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
	return orders.Num() + newOrders.Num() <= Config::maxOrders;
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
		armor = baseStats.headArmor;
		lethality += 1;
		break;
	case BodyPart::Torso:
		armor = baseStats.torsoArmor;
		power *= Config::torsoPowerMult;
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
			FArmorLayer::damage(layer, d * (p + Config::conditionDamagePowerBonus));
		}
		int32 critThreshold = attacker->getStats().perks.Contains(Perk::Ruthless) ? Config::strongHitTalentPowerNeeded : Config::strongHitPowerNeeded;
		if (p >= critThreshold) {
			l += 1;
		}
		if (p >= 0.0f) damageSoldiers(d, l);
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
		a = Config::posMod<int8>(a, 6);
		direction = (HexDirection)a;
		break;
	}
	case Order::CounterRotate: {
		int8 a = (uint8)direction;
		a--;
		a = Config::posMod<int8>(a, 6);
		direction = (HexDirection)a;
		break;
	}
	default: break;
	}
	orderProgress = 0;
	orders.RemoveAt(0);
}

void AUnit::saveUnit()
{
	if (!save) {
		debugStr("AUnit::saveUnit() no save");
		return;
	}
	save->name = name;
	save->baseStats = baseStats;
	save->position = position;
	save->direction = direction;
	save->orders = orders;
	save->path = path;
	save->orderProgress = orderProgress;
	save->lastDirection = lastDirection;
	UGameplayStatics::AsyncSaveGameToSlot(save, getSaveName(), 0);
}

FUnitStats AUnit::getStats() const
{
	FUnitStats ret = baseStats;
	float val = baseStats.perks.Contains(Perk::Swift) ? 1.2f : 1.0f;
	ret.formationStrengthRecovery *= val;
	val = baseStats.perks.Contains(Perk::Resolute) ? 1.3f : 1.0f;
	ret.moraleRecovery *= val;
	val = baseStats.perks.Contains(Perk::Tough) ? 1.2f : 1.0f;
	ret.staminaRecovery *= val;
	val = baseStats.perks.Contains(Perk::Tough) ? 1.4f : 1.0f;
	ret.staminaLimitRecovery *= val;
	val = baseStats.perks.Contains(Perk::Swift) ? 1.1f : 1.0f;
	ret.movementSpeed *= val;
	val = baseStats.perks.Contains(Perk::Swift) ? 1.5f : 1.0f;
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

HexDirection AUnit::getFinalDirection() const
{
	if (orders.Num() == 0) return direction;
	else return lastDirection;
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
	orders.Empty();
	onOrdersChanged();
	path.Empty();
	onPathChanged();
}

TArray<FGridIndex> AUnit::getPossibleMoves() const
{
	auto ring = AGame::getRingOfGridIndices(getFinalPosition());
	return ring;
}

void AUnit::init(const FString& pName, const FString& accountUsername, const FUnitStats& pStats, const FGridIndex& pPosition, const HexDirection& pDirection)
{
	init(pName, AGame::getAccount(accountUsername), pStats, pPosition, pDirection);
}

void AUnit::init(const FString& pName, AAccount* pAccount, const FUnitStats& pStats, const FGridIndex& pPosition, const HexDirection& pDirection)
{
	if (id != -1) {
		debugStr("AUnit::init(): unit was already initialized");
		return;
	}
	id = AGame::addUnit(this);
	if (id == -1) {
		debugStr("AUnit::init(): could not add unit");
		AGame::removeUnit(this);
		return;
	}
	if (UGameplayStatics::LoadGameFromSlot(getSaveName(), 0)) debugStr("AUnit::init(): save game already exists");
	name = pName;
	save = Cast<UUnitSave>(UGameplayStatics::CreateSaveGameObject(UUnitSave::StaticClass()));
	account = pAccount;
	baseStats = pStats;
	position = pPosition;
	direction = pDirection;
	lastDirection = direction;
	AGame::addUnitToMap(this, position);
	ETeleportType tele = ETeleportType::TeleportPhysics;
	modelBase->SetRelativeRotation(AGame::hexDirectionToRotation(direction), false, nullptr, tele);
}

void AUnit::load(const int32& pId)
{
	id = pId;
	save = Cast<UUnitSave>(UGameplayStatics::LoadGameFromSlot(getSaveName(), 0));
	id = AGame::addUnit(this); //TEMP: if a unit is added during loading (should not be possible) need to make sure the correct unit is still loaded
	name = save->name;
	baseStats = save->baseStats;
	position = save->position;
	direction = save->direction;
	orders = save->orders;
	orderProgress = save->orderProgress;
	path = save->path;
	lastDirection = save->lastDirection;
	account = AGame::getAccount(save->accountUsername);
	AGame::addUnitToMap(this, position);
	ETeleportType tele = ETeleportType::TeleportPhysics;
	modelBase->SetRelativeRotation(AGame::hexDirectionToRotation(direction), false, nullptr, tele);
}

FString AUnit::getSaveName() const
{
	return FString(TEXT("Unit ")) + FString::FromInt(id) + FString(TEXT(" ")) + AGame::getCampaignName();
}

