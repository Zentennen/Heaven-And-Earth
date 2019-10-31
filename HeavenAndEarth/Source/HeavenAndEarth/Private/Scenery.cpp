#include "Scenery.h"
#include "Game.h"
#include "..\Public\Scenery.h"

AScenery::AScenery()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AScenery::BeginPlay()
{
	Super::BeginPlay();
}

void AScenery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScenery::setPosition(const FGridIndex& gi)
{

}

void AScenery::init()
{
	if (initialized) return;
	center = AGame::vectorToGridIndex(GetActorLocation());
	SetActorLocation(AGame::gridIndexToVector(center));
	for (uint8 i = 0; i < tiles.Num(); i++) {
		tiles[i].pos += center;
		AGame::setTile(tiles[i]);
	}
	initialized = true;
}

