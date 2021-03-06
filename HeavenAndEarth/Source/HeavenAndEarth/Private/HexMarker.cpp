#include "HexMarker.h"
#include "HAE.h"
#include "Game.h"

AHexMarker::AHexMarker()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AHexMarker::BeginPlay()
{
	Super::BeginPlay();

}

void AHexMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHexMarker::setProperties(const FGridIndex& gi, MarkerType type)
{
	setPos(gi);
	setType(type);
}

void AHexMarker::setPos(const FGridIndex& gi)
{
	ETeleportType tele = ETeleportType::TeleportPhysics;
	float f = markerType == MarkerType::Cursor ? 0.02f : 0.01f;
	SetActorLocation(AGame::gridIndexToVector(gi, f), false, nullptr, tele);
}
