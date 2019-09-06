#include "Game.h"
#include "UnrealNetwork.h"
#include "AStar.h"
#include "Engine.h"
#include "..\Public\Game.h"

AGame* AGame::game;

AGame::AGame()
{
	PrimaryActorTick.bCanEverTick = false;
	tiles.Init(FTileColumn(GameLib::mapY), GameLib::mapX);
}

void AGame::BeginPlay()
{
	Super::BeginPlay();
	game = this;
	debugStr("AGame::game set");
}

void AGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGame, tiles);
}

bool AGame::addUnit(AUnit* unit)
{
	if (!game) {
		debugStr("ERROR: AGame::addUnit(): no game pointer");
		return false;
	}
	if (!unit) {
		debugStr("ERROR: AGame::addUnit(): no unit pointer");
		return false;
	}
	game->units.Emplace(unit);
	return true;
}

bool AGame::addPC(APC* pc)
{
	if (!game) {
		debugStr("ERROR: AGame::addPC(): no game pointer");
		return false;
	}
	if (!pc) {
		debugStr("ERROR: AGame::addPC(): no pc pointer");
		return false;
	}
	game->pcs.Emplace(pc);
	return true;
}

void AGame::executeTurn()
{
	if (!HasAuthority()) return;
	for (auto i : units) i->beginTurn();
	for (auto i : pcs) i->beginTurn();
	//TODO: ...
	for (auto i : units) i->endTurn();
	for (auto i : pcs) i->endTurn();
}

bool AGame::isValidPos(const FGridIndex& pos)
{
	if (pos.x < 0 || pos.x >= GameLib::mapX || pos.y < 0 || pos.y >= GameLib::mapY) return false;
	else return true;
}

FTile AGame::getTile(const FGridIndex& tilePos)
{
	if (!isValidPos(tilePos)) return FTile();
	uint8 cost = game->tiles[tilePos.x].costs[tilePos.y];
	auto props = getTilePropertiesFromFlags(game->tiles[tilePos.x].flags[tilePos.y]);
	return FTile(cost, tilePos, props);
}

FGridIndex AGame::movementToGridIndex(const HexDirection& dir, FGridIndex pos)
{
	switch (dir) {
	case HexDirection::UpLeft:
		if (pos.y % 2 == 0) return pos + FGridIndex(0, 1);
		else return pos + FGridIndex(-1, 1);
	case HexDirection::UpRight:
		if (pos.y % 2 == 0) return pos + FGridIndex(1, 1);
		else return pos + FGridIndex(0, 1);
	case HexDirection::Left:
		return pos + FGridIndex(-1, 0);
	case HexDirection::Right:
		return pos + FGridIndex(1, 0);
	case HexDirection::DownLeft:
		if (pos.y % 2 == 0) return pos + FGridIndex(0, -1);
		else return pos + FGridIndex(-1, -1);
	case HexDirection::DownRight:
		if (pos.y % 2 == 0) return pos + FGridIndex(1, -1);
		else return pos + FGridIndex(0, -1);
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
	game->tiles[t.pos.x].costs[t.pos.y] = t.cost;
	game->tiles[t.pos.x].units[t.pos.y] = t.unit;
	game->tiles[t.pos.x].flags[t.pos.y] = getFlagsFromTileProperties(t.properties);
	return true;
}

TArray<FGridIndex> AGame::getPathAsGridIndices(const FGridIndex& start, const FGridIndex& goal, const int32& maxMove)
{
	TArray<FGridIndex> ret;
	ret.Empty();
	auto path = AStar::getPath(start, goal, maxMove);
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
		if (other - base == movementToGridIndex((HexDirection)i, base)) {
			dir = (HexDirection)i;
			return true;
		}
	}
	return false;
}

FString AGame::gridIndexToString(const FGridIndex& gridIndex)
{
	return gridIndex;
}
