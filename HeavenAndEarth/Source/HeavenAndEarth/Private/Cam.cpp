#include "Cam.h"

ACam::ACam()
{
	PrimaryActorTick.bCanEverTick = true;
}

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
	FVector vec(forward - back, right - left, 0.0f);
	vec = vec.GetSafeNormal();
	vec *= deltaTime * movementSpeed;
	SetActorLocation(GetActorLocation() + vec);
}

