// Fill out your copyright notice in the Description page of Project Settings.


#include "Cam.h"

// Sets default values
ACam::ACam()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACam::move(const float& deltaTime, const float& forward, const float& back, const float& left, const float& right)
{
	SetActorLocation(GetActorLocation() + FVector(deltaTime * movementSpeed * (forward - back), deltaTime * movementSpeed * (right - left), 0.0f));
}

int32 ACam::roll(uint8 num, uint8 max)
{
	int32 result = 0;
	for (uint8 i = 0; i < num; i++) {
		result += FMath::RandRange(1, max);
	}
	return result;
}

FGridIndex ACam::vectorToGridIndex(const FVector& vec)
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

FGridIndex ACam::vector2DToGridIndex(const FVector2D& vec)
{
	return vectorToGridIndex(FVector(vec, 0.0f));
}

FVector ACam::gridIndexToVector(const FGridIndex& gi, float z)
{
	if (gi.y % 2 == 0) return FVector(gi.y * GameLib::hexY, gi.x * GameLib::hexSize, z);
	else return FVector(gi.y * GameLib::hexY, gi.x * GameLib::hexSize - GameLib::hexHalfSize, z);
}

FVector2D ACam::gridIndexToVector2D(const FGridIndex& gi)
{
	return FVector2D(gridIndexToVector(gi));
}


