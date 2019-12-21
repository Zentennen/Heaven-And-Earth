#include "Game.h"
#include "UnrealNetwork.h"
#include "Engine.h"
#include "Scenery.h"
#include "UnitSave.h"
#include "CampaignSave.h"
#include "Engine.h"
#include "LoginSave.h"
#include <cmath>

AGame* AGame::game;

AGame::AGame()
{
	bReplicates = true;
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	PrimaryActorTick.bCanEverTick = true;
	tiles.Init(FTileColumn(Config::mapY), Config::mapX);
}

void AGame::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() || !game) {
		game = this; //needed so multiple game instances running in editor don't reset game
	}
	else {
		//debugStr("Game could not be set");
	}
	if (!HasAuthority()) return;
	for (TActorIterator<APC> it(GetWorld()); it; ++it) addPC(*it);
	auto loginSave = Cast<ULoginSave>(UGameplayStatics::LoadGameFromSlot("Login", 0));
	if(loginSave) campaignName = loginSave->campaignName;
	if (campaignName == "") {
		finishedLoading = true;
		onRepFinishedLoading();
		return;
	}
	auto save = LOAD_GAME;
	if (!save) {
		debugStr("Creating new save for this campaign");
		finishedLoading = true;
		onRepFinishedLoading();
	}
	else {
		numAccounts = save->numAccounts;
		numUnits = save->numUnits;
	}
}

void AGame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (game == this) game = nullptr;
}

void AGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority()) return;
	if (executing) {
		timer += DeltaTime;
		if (timer < actionTime) return;
		timer = 0.0f;
		counter++;
		for (auto i : game->units) i->executeOrder();
		if (counter >= actionsPerTurn) {
			counter = 0;
			executing = false;
			for (auto i : game->units) i->endTurn();
			for (auto i : game->pcs) i->endTurn();
			onRepExecuting();
		}
	}
	else if (!finishedLoading) {
		auto world = GetWorld();
		auto save = LOAD_GAME;
		if (accountCounter < numAccounts) {
			auto acc = world->SpawnActor<AAccount>();
			acc->load(addAccount(acc), save);
			accountCounter++;
		}
		else if (unitCounter < numUnits) {
			AUnit* u = nullptr;
			spawnUnit(u);
			u->load(addUnit(u), save);
			unitCounter++;
		}
		else {
			finishedLoading = true;
			onRepFinishedLoading();
		}
	}
}

void AGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGame, tiles);
	DOREPLIFETIME(AGame, executing);
	DOREPLIFETIME(AGame, finishedLoading);
}

FString AGame::getSaveName() const
{
	return FString(TEXT("Campaign ")) + campaignName;
}

int32 AGame::addUnit(AUnit* unit)
{
	if (!IsValid(game)) return -1;
	if (!unit) {
		debugStr("AGame::addUnit(): unit is null");
		return -1;
	}
	if (game->units.Contains(unit)) return -1;
	return game->units.Emplace(unit);
}

int32 AGame::addAccount(AAccount* account)
{
	if (!IsValid(game)) return -1;
	if (!account) {
		debugStr("AGame::addAccount(): account was null");
		return -1;
	}
	if (game->accounts.Contains(account)) return -1;
	return game->accounts.Emplace(account);
}

void AGame::removeUnit(AUnit* unit)
{
	if (!unit) {
		debugStr("AGame::removeUnit(): unit was null");
		return;
	}
	if (!game->units.Contains(unit)) return;
	game->units.Remove(unit);
	UGameplayStatics::DeleteGameInSlot(unit->getSaveName(), 0);
	unit->Destroy();
}

void AGame::removeAccount(AAccount* account)
{
	if (!account) {
		debugStr("AGame::removeAccount(): account was null");
		return;
	}
	game->accounts.Remove(account);
	UGameplayStatics::DeleteGameInSlot(account->getSaveName(), 0);
	account->Destroy();
}

bool AGame::addPC(APC* pc)
{
	if (!IsValid(game)) return false;
	if (!pc) {
		debugStr("ERROR: AGame::addPC(): pc is null");
		return false;
	}
	if (game->pcs.Contains(pc)) return false;
	game->pcs.Emplace(pc);
	pc->hostInit();
	return true;
}

//bool AGame::canCompleteOrder(AUnit* unit)
//{
//	if (!unit) return false;
//	if (unit->orders.Num() == 0) return true;
//	switch (unit->orders[0]) {
//	case Order::Rest: return true;
//	case Order::Move: {
//		auto gi = movementToGridIndex(unit->direction, unit->position);
//		if (!isValidPos(gi)) return false;
//		if (game->tiles[gi.x].units[gi.y]) return false;
//		return unit->orderProgress * unit->getStats().movementSpeed >= game->tiles[gi.x].costs[gi.y];
//	}
//	case Order::Rotate: {
//		return unit->orderProgress * unit->getStats().rotationSpeed >= Config::rotationNeeded;
//	}
//	case Order::CounterRotate: {
//		return unit->orderProgress * unit->getStats().rotationSpeed >= Config::rotationNeeded;
//	}
//	default: return true;
//	}
//}

FString AGame::getCampaignName()
{
	if (!game) return FString();
	return game->campaignName;
}

void AGame::rotateClockwise(HexDirection& dir, uint8 steps)
{
	int8 a = (uint8)dir;
	a++;
	a = Config::posMod<int8>(a, 6);
	dir = (HexDirection)a;
}

void AGame::rotateCounterClockwise(HexDirection& dir, uint8 steps)
{
	int8 a = (uint8)dir;
	a--;
	a = Config::posMod<int8>(a, 6);
	dir = (HexDirection)a;
}

UCampaignSave* AGame::getSave()
{
	if (!game) {
		debugStr("AGame::getSave(): game is null");
		return nullptr;
	}
	return Cast<UCampaignSave>(UGameplayStatics::LoadGameFromSlot(game->getSaveName(), 0));
}

void AGame::executeTurn()
{
	if (!game->HasAuthority()) return;
	if (game->executing) return;
	game->executing = true;
	for (auto i : game->units) i->beginTurn();
	for (auto i : game->pcs) i->beginTurn();
	game->onRepExecuting();
}

bool AGame::isValidPos(const FGridIndex& pos)
{
	if (pos.x < 0 || pos.x >= Config::mapX || pos.y < 0 || pos.y >= Config::mapY) return false;
	else return true;
}

bool AGame::isOpenPos(const FGridIndex& pos)
{
	if (!isValidPos(pos)) return false;
	return game->tiles[pos.x].units[pos.y] == nullptr;
}

FTile AGame::getTile(const FGridIndex& tilePos)
{
	if (!isValidPos(tilePos)) return FTile();
	if (!IsValid(game)) return FTile();
	if (game->tiles.Num() == 0) return FTile();
	uint8 cost = game->tiles[tilePos.x].costs[tilePos.y];
	auto props = getTilePropertiesFromFlags(game->tiles[tilePos.x].flags[tilePos.y]);
	return FTile(cost, tilePos, props);
}

FGridIndex AGame::movementToGridIndex(HexDirection dir, const FGridIndex& pos, int32 distance)
{
	switch (dir) {
	case HexDirection::UpLeft:
		if (pos.y % 2 == 0) return pos + FGridIndex(0, 1) * distance;
		else return pos + FGridIndex(-1, 1) * distance;
	case HexDirection::UpRight:
		if (pos.y % 2 == 0) return pos + FGridIndex(1, 1) * distance;
		else return pos + FGridIndex(0, 1) * distance;
	case HexDirection::Left:
		return pos + FGridIndex(-1, 0) * distance;
	case HexDirection::Right:
		return pos + FGridIndex(1, 0) * distance;
	case HexDirection::DownLeft:
		if (pos.y % 2 == 0) return pos + FGridIndex(0, -1) * distance;
		else return pos + FGridIndex(-1, -1) * distance;
	case HexDirection::DownRight:
		if (pos.y % 2 == 0) return pos + FGridIndex(1, -1) * distance;
		else return pos + FGridIndex(0, -1) * distance;
	default:
		return pos;
	}
}

TArray<TileProperty> AGame::getTilePropertiesFromFlags(const uint8& flags)
{
	TArray<TileProperty> properties;
	properties.Empty();
	//TODO: implement
	return properties;
}

uint8 AGame::getFlagsFromTileProperties(const TArray<TileProperty>& props)
{
	//TODO: Implement
	return 0;
}

bool AGame::setTile(const FTile& t)
{
	if (!isValidPos(t.pos)) return false;
	if (!IsValid(game)) return false;
	if (game->tiles.Num() == 0) return false;
	game->tiles[t.pos.x].costs[t.pos.y] = t.cost;
	game->tiles[t.pos.x].units[t.pos.y] = t.unit;
	game->tiles[t.pos.x].flags[t.pos.y] = getFlagsFromTileProperties(t.properties);
	return true;
}

int32 AGame::manhattanDistance(const FGridIndex& start, const FGridIndex& goal)
{
	int32 startX = start.x - (start.y + (start.y & 1)) / 2;
	int32 startZ = start.y;
	int32 startY = -startX - startZ;
	int32 goalX = goal.x - (goal.y + (goal.y & 1)) / 2;
	int32 goalZ = goal.y;
	int32 goalY = -goalX - goalZ;
	return FMath::Max3(FMath::Abs<int32>(startX - goalX), FMath::Abs<int32>(startY - goalY), FMath::Abs<int32>(startZ - goalZ));
}

int32 AGame::roll(uint8 num, uint8 max)
{
	int32 result = 0;
	for (uint8 i = 0; i < num; i++) {
		result += FMath::RandRange(1, max);
	}
	return result;
}

APC* AGame::getLocalPC()
{
	if (!IsValid(game)) return nullptr;
	return Cast<APC>(game->GetWorld()->GetFirstPlayerController());
}

FGridIndex AGame::vectorToGridIndex(const FVector& vec)
{
	float x = vec.Y / Config::hexSize;
	float y = vec.X / Config::hexY;
	FGridIndex gi(FMath::FloorToInt(x), FMath::FloorToInt(y));
	if (gi.y % 2 == 0) {
		if (y - gi.y > Config::third + Config::twoThirds * FMath::Abs(x - gi.x - 0.5f)) {
			gi.y += 1;
			gi.x += 1;
			return gi;
		}
		else {
			gi.x = FMath::FloorToInt(x + 0.5f);
			return gi;
		}
	}
	else {
		if (y - gi.y > Config::twoThirds - Config::twoThirds * FMath::Abs(x - gi.x - 0.5f)) {
			gi.y += 1;
			gi.x = FMath::FloorToInt(x + 0.5f);
			return gi;
		}
		else {
			gi.x += 1;
			return gi;
		}
	}
	return gi;
}

FGridIndex AGame::vector2DToGridIndex(const FVector2D& vec)
{
	return vectorToGridIndex(FVector(vec, 0.0f));
}

FVector AGame::gridIndexToVector(const FGridIndex& gi, float z)
{
	if (gi.y % 2 == 0) return FVector(gi.y * Config::hexY, gi.x * Config::hexSize, z);
	else return FVector(gi.y * Config::hexY, gi.x * Config::hexSize - Config::hexHalfSize, z);
}

FVector2D AGame::gridIndexToVector2D(const FGridIndex& gi)
{
	return FVector2D(gridIndexToVector(gi));
}

FRotator AGame::hexDirectionToRotation(const HexDirection& dir)
{
	float yaw = 0.0f;
	switch (dir) {
	case HexDirection::Right:
		yaw = 0.0f;
		break;
	case HexDirection::DownRight:
		yaw = 60.0f;
		break;
	case HexDirection::DownLeft:
		yaw = 120.0f;
		break;
	case HexDirection::Left:
		yaw = 180.0f;
		break;
	case HexDirection::UpLeft:
		yaw = 240.0f;
		break;
	case HexDirection::UpRight:
		yaw = 300.0f;
		break;
	}
	return FRotator(0.0f, yaw, 0.0f);
}

bool AGame::gridIndicesToHexDirection(const FGridIndex& base, const FGridIndex& other, HexDirection& dir)
{
	for (uint8 i = 0; i < 6; i++) {
		if (other == movementToGridIndex((HexDirection)i, base)) {
			dir = (HexDirection)i;
			return true;
		}
	}
	return false;
}

bool AGame::isAdjacent(const FGridIndex& base, const FGridIndex& other)
{
	for (uint8 i = 0; i < 6; i++) {
		if (movementToGridIndex((HexDirection)i, base) == other) return true;
	}
	return false;
}

bool AGame::moveUnit(AUnit* unit)
{
	if (!unit) {
		debugStr("AGame::moveUnit(): unit was null");
		return false;
	}
	auto position = unit->getData().position;
	auto direction = unit->getData().direction;
	auto newPos = movementToGridIndex(direction, position);
	if (!isValidPos(newPos)) return false;
	if (game->tiles[newPos.x].units[newPos.y]) return false;
	game->tiles[newPos.x].units[newPos.y] = unit;
	if (game->tiles[position.x].units[position.y] == unit) game->tiles[position.x].units[position.y] = nullptr;
	else debugStr("AGame::moveUnit(): unit was not at its position");
	unit->setPosition(newPos);
	return true;
}

bool AGame::teleportUnit(AUnit* unit, const FGridIndex& pos)
{
	if (!unit) {
		debugStr("AGame::teleportUnit(): unit was null");
		return false;
	}
	auto position = unit->getData().position;
	if (game->tiles[pos.x].units[pos.y] != nullptr) return false;
	game->tiles[pos.x].units[pos.y] = unit;
	if (game->tiles[position.x].units[position.y] == unit) game->tiles[position.x].units[position.y] = nullptr;
	else debugStr("AGame::teleportUnit(): unit was not at its position");
	unit->setPosition(pos);
	return true;
}

bool AGame::addUnitToMap(AUnit* unit, const FGridIndex& pos)
{
	if (!unit) {
		debugStr("AGame::addUnitToMap(): unit was null");
		return false;
	}
	if (!game->tiles[pos.x].units[pos.y]) {
		game->tiles[pos.x].units[pos.y] = unit;
		unit->setPosition(pos);
	}
	else {
		TArray<FGridIndex> open;
		TArray<FGridIndex> closed;
		open.Empty();
		closed.Empty();
		open.Append(getRingOfGridIndices(pos));
		closed.Emplace(pos);
		while (true) {
			for (auto i : open) {
				if (!game->tiles[i.x].units[i.y]) {
					game->tiles[i.x].units[i.y] = unit;
					unit->setPosition(i);
					return true;
				}
				else closed.Emplace(i);
			}
			for (auto i : open) {
				TArray<FGridIndex> arr = getRingOfGridIndices(i);
				for (auto j : arr) if (!closed.Contains(j)) open.AddUnique(j);
			}
		}
	}
	return true;
}

bool AGame::canLogin(APC* pc, const FString& username, const FString& password)
{
	for (auto i : game->accounts) {
		auto result = i->canLogin(username, password);
		switch (result) {
		case LoginResult::Success:
			return true;
		case LoginResult::Fail:
			return false;
		default:
			break;
		}
	}
	return true;
}

bool AGame::createAccount(APC* pc, const FString& username, const FString& password)
{
	if (!pc) return false;
	for (auto i : game->accounts) {
		if (i->canLogin(username, password) != LoginResult::NotRegistered) return false;
	}
	auto acc = pc->GetWorld()->SpawnActor<AAccount>(FVector::ZeroVector, FRotator::ZeroRotator);
	acc->init(username, password, addAccount(acc));
	login(pc, username, password);
	return true;
}

AAccount* AGame::getAccount(const FString& username)
{
	if (!IsValid(game)) {
		debugStr("AGame::getAccount(): game is invalid");
		return nullptr;
	}
	for (auto i : game->accounts) if (i->getUsername() == username) return i;
	return nullptr;
}
TArray<FString> AGame::getUsernames()
{
	TArray<FString> ret;
	ret.Empty();
	for (auto i : game->accounts) ret.Emplace(i->getUsername());
	return ret;
}

TArray<AAccount*> AGame::getAccounts()
{
	return game->accounts;
}

FUnitStats AGame::getUnitStats(FString name)
{
	return game->defaultStats[name];
}

bool AGame::isAcceptingCommands()
{
	if (!game) {
		debugStr("AGame::isAcceptingCommands(): game is null");
		return false;
	}
	return !game->executing && game->finishedLoading;
}

AUnit* AGame::createAndInitializeUnit(AAccount* acc, const FUnitData& data)
{
	if (!isAcceptingCommands() || !game->HasAuthority() || !AGame::isValidPos(data.position)) return nullptr;
	AUnit* u = nullptr;
	game->spawnUnit(u);
	u->init(data, acc, addUnit(u));
	return u;
}

LoginResult AGame::login(APC* pc, const FString& username, const FString& password)
{
	for (auto i : game->accounts) {
		auto result = i->canLogin(username, password);
		switch (result) {
		case LoginResult::Success:
			pc->account = i;
			debugFstr(username + FString(TEXT(" logged in")));
			return LoginResult::Success;
		case LoginResult::Fail:
			return LoginResult::Fail;
		default:
			break;
		}
	}
	return LoginResult::NotRegistered;
}

TArray<Order> AGame::getRotationOrdersTo(const HexDirection& start, const HexDirection& goal)
{
	TArray<Order> ret;
	ret.Empty();
	float diff = std::remainderf((int8)goal - (int8)start, 6.0f);
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

FString AGame::gridIndexToString(const FGridIndex& gridIndex)
{
	return gridIndex;
}

TArray<FGridIndex> AGame::getRingOfGridIndices(const FGridIndex& gi, int32 radius)
{
	TArray<FGridIndex> ret;
	ret.Empty();
	if (radius < 0) return ret;
	else if (radius == 0) ret.Emplace(gi);
	else for (uint8 j = 0; j < 6; j++) {
		auto a = movementToGridIndex((HexDirection)j, gi, radius);
		if (isValidPos(a)) ret.Emplace(a);
	}
	return ret;
}

TArray<FGridIndex> AGame::getCircleOfGridIndices(const FGridIndex& gi, int32 radius)
{
	TArray<FGridIndex> ret;
	ret.Empty();
	for (uint8 i = 0; i <= radius; i++) {
		ret.Append(getCircleOfGridIndices(gi, i));
	}
	return ret;
}

TArray<FTile> AGame::getRingOfTiles(const FGridIndex& center, int32 radius)
{
	TArray<FTile> ret;
	ret.Empty();
	TArray<FGridIndex> arr = getRingOfGridIndices(center, radius);
	for (auto i : arr) ret.Emplace(getTile(i));
	return ret;
}

TArray<FTile> AGame::getCircleOfTiles(const FGridIndex& center, int32 radius)
{
	TArray<FTile> ret;
	ret.Empty();
	TArray<FGridIndex> arr = getCircleOfGridIndices(center, radius);
	for (auto i : arr) ret.Emplace(getTile(i));
	return ret;
}

AGame* AGame::getGame()
{
	return game;
}

int32 AGame::getActionsPerTurn()
{
	return game->actionsPerTurn;
}

bool AGame::isExecuting()
{
	return game->executing;
}

bool AGame::hasGameStarted()
{
	return game->finishedLoading && IsValid(game);
}

void AGame::saveGame()
{
	if (!IsValid(game)) {
		debugStr("AGame::saveGame(): game is invalid");
		return;
	}
	if (game->campaignName == "") {
		debugStr("There is no campaign name. The game will not be saved");
		return;
	}
	if (!isAcceptingCommands()) {
		debugStr("Game was not accepting commands. Could not save game");
		return;
	}
	auto save = CREATE_GAME_SAVE;
	save->numAccounts = game->accounts.Num();
	save->numUnits = game->units.Num();
	for (auto i : game->accounts) {
		i->save(save);
	}
	for (auto i : game->units) {
		i->save(save);
	}
	UGameplayStatics::AsyncSaveGameToSlot(save, game->getSaveName(), 0);
}
