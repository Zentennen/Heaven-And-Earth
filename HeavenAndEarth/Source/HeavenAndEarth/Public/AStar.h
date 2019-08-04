#pragma once

#include "CoreMinimal.h"
#include "GameLib.h"
#include <memory>
#include <vector>

class HEAVENANDEARTH_API AStar
{
	static bool shouldSkip(const FTile& t, std::vector<std::shared_ptr<AStar>>& open, std::vector<std::shared_ptr<AStar>>& closed, const int32& maxMove);
	static std::shared_ptr<AStar> findInOpen(const FTile& t, const std::vector<std::shared_ptr<AStar>>& open);
public:
	int32 f;
	int32 g;
	int32 h;
	std::shared_ptr<AStar> parent;
	FTile tile;
	AStar(const FTile& t, const FGridIndex& goal, const std::shared_ptr<AStar>& p);
	~AStar();
	static std::shared_ptr<AStar> getPath(const FGridIndex& start, const FGridIndex& goal, const int32& maxMove);
};
