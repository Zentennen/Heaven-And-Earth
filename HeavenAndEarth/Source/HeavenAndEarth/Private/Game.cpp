#include "Game.h"
#include "UnrealNetwork.h"
#include "AStar.h"

AGame* AGame::game;

AGame::AGame()
{
	PrimaryActorTick.bCanEverTick = false;
	tiles.Init(FCostColumn(GameLib::mapY), GameLib::mapX);
}

void AGame::BeginPlay()
{
	Super::BeginPlay();
	if (!game) {
		game = this;
		//for (uint8 i = 0; i < tiles.Num(); i++) for (uint8 j = 0; j < tiles[i].tiles.Num(); j++) {
		//	tiles[i][j].pos = FGridIndex(i, j);
		//}
	}
	else Destroy();
}

void AGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGame, tiles);
}

bool AGame::isValidPos(const FGridIndex& pos)
{
	if (pos.x < 0 || pos.x >= GameLib::mapX || pos.y < 0 || pos.y >= GameLib::mapY) return false;
	else return true;
}

FTile AGame::getTile(const FGridIndex& tilePos)
{
	if (!isValidPos(tilePos)) return FTile();
	return FTile(game->tiles[tilePos.x].costs[tilePos.y], tilePos, getTilePropertiesFromFlags(game->tiles[tilePos.x].flags[tilePos.y]));
}

FGridIndex AGame::directionToGridIndex(const HexDirection& dir)
{
	switch (dir) {
	case HexDirection::Right:
		return FGridIndex(1, 0);
	case HexDirection::DownRight:
		return FGridIndex(1, -1);
	case HexDirection::DownLeft:
		return FGridIndex(0, -1);
	case HexDirection::Left:
		return FGridIndex(-1, 0);
	case HexDirection::UpLeft:
		return FGridIndex(-1, 1);
	case HexDirection::UpRight:
		return FGridIndex(0, 1);
	default:
		return FGridIndex(0, 0);
	}
}

FGridIndex AGame::movementToGridIndex(const HexDirection& dir, FGridIndex pos)
{
	switch (dir) {
	case HexDirection::Right:
		return pos + FGridIndex(1, 0);
	case HexDirection::DownRight:
		return pos + FGridIndex(1, -1);
	case HexDirection::DownLeft:
		return pos + FGridIndex(0, -1);
	case HexDirection::Left:
		return pos + FGridIndex(-1, 0);
	case HexDirection::UpLeft:
		return pos + FGridIndex(-1, 1);
	case HexDirection::UpRight:
		return pos + FGridIndex(0, 1);
	default:
		return pos;
	}
}

TArray<TileProperty> AGame::getTilePropertiesFromFlags(const uint8& flags)
{
	TArray<TileProperty> properties;
	properties.Empty();
	//...
	return properties;
}

TArray<FGridIndex> AGame::getPathAsGridIndices(const FGridIndex& start, const FGridIndex& goal, const int32& maxMove)
{
	TArray<FGridIndex> ret;
	ret.Empty();
	auto path = AStar::getPath(start, goal, maxMove);
	if (path) {
		ret.Emplace(path->tile.pos);
		while (path->parent) {
			ret.Emplace(path->tile.pos);
			path = path->parent;
		}
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