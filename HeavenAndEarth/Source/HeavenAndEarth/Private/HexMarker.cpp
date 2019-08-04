#include "HexMarker.h"
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

void AHexMarker::setPos(const FGridIndex& gi)
{
	SetActorLocation(AGame::gridIndexToVector(gi));
}

