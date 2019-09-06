#include "PC.h"
#include "Unit.h"
#include "Game.h"

void APC::BeginPlay() {
	bShowMouseCursor = true;
	Super::BeginPlay();
	if (!HasAuthority()) return;
	AGame::addPC(this);
}

void APC::action()
{

}

void APC::createUnit(const FVector& location)
{
	auto world = GetWorld();
	if (!world) return;
	FActorSpawnParameters param;
	auto loc = location;
	auto rot = AGame::hexDirectionToRotation(HexDirection::Right);
	world->SpawnActor(AUnit::StaticClass(), &loc, &rot, param);
}

void APC::selectUnit(AUnit* u)
{
	if (!u) return;
	if (u == unit) return;
	if (unit) {
		unit->unselect();
		onUnselect();
	}
	unit = u;
	u->select();
	onSelect(u);
}

void APC::unselect()
{
	if (!unit) return;
	unit->unselect();
	onUnselect();
	unit = nullptr;
}

void APC::updatePath_Implementation(const TArray<FGridIndex>& path)
{
	bUpdatePath(path);
}

bool APC::moveCommand_Validate(AUnit* u, FGridIndex destination, bool replaceCurrentOrders)
{
	return true;
}

void APC::moveCommand_Implementation(AUnit* u, FGridIndex destination, bool replaceCurrentOrders)
{
	if (!unit) return;
	if (unit->controller != this) return;
	auto path = AGame::getPathAsGridIndices(unit->position, destination, unit->movementSpeed);
	unit->moveAlongPath(path, replaceCurrentOrders);
	updatePath(path);
}

void APC::beginTurn()
{

}

void APC::endTurn()
{

}
