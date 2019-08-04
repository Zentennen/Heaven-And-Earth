#include "PC.h"
#include "Unit.h"
#include "Game.h"

void APC::BeginPlay() {
	bShowMouseCursor = true;
	Super::BeginPlay();
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
	unit = u;
	u->select();
	onSelect(u);
}
