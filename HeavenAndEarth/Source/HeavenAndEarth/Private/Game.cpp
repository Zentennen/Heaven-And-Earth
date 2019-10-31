#include "Game.h"
#include "UnrealNetwork.h"
#include "AStar.h"
#include "Engine.h"
#include "Scenery.h"
#include "GameLib.h"
#include <cmath>
#include "..\Public\Game.h"

AGame* AGame::game;

AGame::AGame()
{
	PrimaryActorTick.bCanEverTick = true;
	tiles.Init(FTileColumn(GameLib::mapY), GameLib::mapX);
}

void AGame::BeginPlay()
{
	if (HasAuthority() || !IsValid(game)) game = this; //needed so multiple game instances running in editor don't reset game
	if (HasAuthority()) { 
		for (TActorIterator<AUnit> it(GetWorld()); it; ++it) addUnit(*it);
		for (TActorIterator<APC> it(GetWorld()); it; ++it) addPC(*it);
		for (TActorIterator<AScenery> it(GetWorld()); it; ++it) it->init();
	}
	Super::BeginPlay();
}

void AGame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (game == this) game = nullptr;
}

void AGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!executing || !HasAuthority()) return;
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

void AGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGame, tiles);
	DOREPLIFETIME(AGame, executing);
}

bool AGame::addUnit(AUnit* unit)
{
	if (!IsValid(game)) return false;
	if (!unit) {
		debugStr("ERROR: AGame::addUnit(): unit is null");
		return false;
	}
	if (game->units.Contains(unit)) return false;
	game->units.Emplace(unit);
	if (!game->tiles[unit->position.x].units[unit->position.y]) game->tiles[unit->position.x].units[unit->position.y] = unit;
	else {
		TArray<FGridIndex> open;
		TArray<FGridIndex> closed;
		open.Empty();
		closed.Empty();
		open.Append(getRingOfGridIndices(unit->position));
		closed.Emplace(unit->position);
		while (true) {
			for (auto i : open) {
				if (!game->tiles[i.x].units[i.y]) {
					unit->position = i;
					game->tiles[i.x].units[i.y] = unit;
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

bool AGame::addPC(APC* pc)
{
	if (!IsValid(game)) return false;
	if (!pc) {
		debugStr("ERROR: AGame::addPC(): pc is null");
		return false;
	}
	if (game->pcs.Contains(pc)) return false;
	game->pcs.Emplace(pc);
	return true;
}

bool AGame::canCompleteOrder(AUnit* unit)
{
	if (!unit) return false;
	if (unit->orders.Num() == 0) return true;
	switch (unit->orders[0]) {
	case Order::Rest: return true;
	case Order::Move: {
		auto gi = movementToGridIndex(unit->direction, unit->position);
		if (!isValidPos(gi)) return false;
		if (game->tiles[gi.x].units[gi.y]) return false;
		return unit->orderProgress * unit->getStats().movementSpeed >= game->tiles[gi.x].costs[gi.y];
	}
	case Order::Rotate: {
		return unit->orderProgress * unit->getStats().rotationSpeed >= GameLib::rotationNeeded;
	}
	case Order::CounterRotate: {
		return unit->orderProgress * unit->getStats().rotationSpeed >= GameLib::rotationNeeded;
	}
	default: return true;
	}
}

void AGame::executeTurn()
{
	if (game->executing) return;
	game->executing = true;
	for (auto i : game->units) i->beginTurn();
	for (auto i : game->pcs) i->beginTurn();
	game->onRepExecuting();
}

bool AGame::isValidPos(const FGridIndex& pos)
{
	if (pos.x < 0 || pos.x >= GameLib::mapX || pos.y < 0 || pos.y >= GameLib::mapY) return false;
	else return true;
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

TArray<FGridIndex> AGame::getPathAsGridIndices(FGridIndex start, FGridIndex goal)
{
	TArray<FGridIndex> ret;
	ret.Empty();
	auto path = AStar::getPath(start, goal, 999);
	if (!path) return ret;
	path = path->parent;
	while (path) {
		ret.Emplace(path->tile.pos);
		path = path->parent;
	}
	return ret;
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

FGridIndex AGame::vectorToGridIndex(const FVector& vec)
{
	float x = vec.Y / GameLib::hexSize;
	float y = vec.X / GameLib::hexY;
	FGridIndex gi(FMath::FloorToInt(x), FMath::FloorToInt(y));
	if (gi.y % 2 == 0) {
		if (y - gi.y > GameLib::third + GameLib::twoThirds * FMath::Abs(x - gi.x - 0.5f)) {
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
		if (y - gi.y > GameLib::twoThirds - GameLib::twoThirds * FMath::Abs(x - gi.x - 0.5f)) {
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
	if (gi.y % 2 == 0) return FVector(gi.y * GameLib::hexY, gi.x * GameLib::hexSize, z);
	else return FVector(gi.y * GameLib::hexY, gi.x * GameLib::hexSize - GameLib::hexHalfSize, z);
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
	if (!unit) return false;
	auto newPos = movementToGridIndex(unit->direction, unit->position);
	if (!isValidPos(newPos)) return false;
	if (game->tiles[newPos.x].units[newPos.y]) return false;
	game->tiles[newPos.x].units[newPos.y] = unit;
	game->tiles[unit->position.x].units[unit->position.y] = nullptr;
	unit->position = newPos;
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
		auto result = i->canLogin(username, password);
		if (result != LoginResult::NotRegistered) return false;
	}
	auto acc = pc->GetWorld()->SpawnActor<AAccount>(FVector::ZeroVector, FRotator::ZeroRotator);
	game->accounts.Emplace(acc);
	acc->username = username;
	acc->password = password;
	login(pc, username, password);
	return true;
}

LoginResult AGame::login(APC* pc, const FString& username, const FString& password)
{
	for (auto i : game->accounts) {
		auto result = i->canLogin(username, password);
		switch (result) {
		case LoginResult::Success:
			pc->account = i;
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
	if (radius == 0) ret.Emplace(gi);
	else for (uint8 j = 0; j < 6; j++) {
		auto a = movementToGridIndex((HexDirection)j, gi, radius);
		if(isValidPos(a)) ret.Emplace(a);
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
