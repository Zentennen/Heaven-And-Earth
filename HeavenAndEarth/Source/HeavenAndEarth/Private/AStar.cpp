// Fill out your copyright notice in the Description page of Project Settings.


#include "AStar.h"
#include "Game.h"

bool AStar::shouldSkip(const FTile& t, std::vector<std::shared_ptr<AStar>>& open, std::vector<std::shared_ptr<AStar>>& closed, const int32& maxMove)
{
	if(t.pos == INVALID_GI || maxMove < t.cost ) return true;
	for (uint8 i = 0; i < closed.size(); i++) if (closed[i]->tile.pos == t.pos) return true;
	return false;
}

std::shared_ptr<AStar> AStar::findInOpen(const FTile& t, const std::vector<std::shared_ptr<AStar>>& open)
{
	for (uint8 i = 0; i < open.size(); i++) if (open[i]->tile.pos == t.pos) return open[i];
	return nullptr;
}

AStar::AStar(const FTile& t, const FGridIndex& goal, const std::shared_ptr<AStar>& p) : parent(p), tile(t)
{
	if (!parent) g = 0;
	else g = parent->g + t.cost;
	h = AGame::manhattanDistance(t.pos, goal);
	f = g + h;
}

AStar::~AStar()
{
}

std::shared_ptr<AStar> AStar::getPath(const FGridIndex& start, const FGridIndex& goal, const int32& maxMove)
{
	std::vector<std::shared_ptr<AStar>> open;
	std::vector<std::shared_ptr<AStar>> closed;
	open.clear();
	closed.clear();
	auto st = AGame::getTile(start);
	auto gt = AGame::getTile(goal);
	if (st.pos == INVALID_GI || gt.pos == INVALID_GI || gt.cost > maxMove) return nullptr;
	auto curr = std::make_shared<AStar>(st, goal, nullptr);
	open.push_back(curr);
	std::shared_ptr<AStar> g = nullptr;
	while (open.size() != 0) {
		if (curr->tile.pos == goal) {
			g = curr;
			break;
		}
		closed.push_back(curr);
		for (uint8 i = 0; i < open.size(); i++) if (open[i] == curr) {
			std::swap(open[i], open.back());
			open.pop_back();
		}
		for (uint8 i = 0; i < 6; i++) {
			HexDirection dir = (HexDirection)i;
			auto tile = AGame::getTile(AGame::movementToGridIndex(dir, curr->tile.pos));
			if (shouldSkip(tile, open, closed, maxMove)) continue;
			auto astar = findInOpen(tile, open);
			if (astar) {
				if (curr->g + tile.cost < astar->g) {
					astar->parent = curr;
					astar->g = curr->g + tile.cost;
					astar->f = astar->g + astar->h;
				}
			}
			else open.push_back(std::make_shared<AStar>(tile, goal, curr));
		}
		curr = open[0];
		for(uint8 i = 0; i < open.size(); i++) if (open[i]->f <= curr->f) curr = open[i];
	}
	if(!g) return nullptr;
	auto s = g;
	while (s->parent) s = s->parent;
	auto lastParent = s;
	while (lastParent != g) {
		auto it = g;
		while (it->parent != lastParent) it = it->parent;
		lastParent->parent = it;
		lastParent = it;
	}
	lastParent->parent = nullptr;
	return s;
}
