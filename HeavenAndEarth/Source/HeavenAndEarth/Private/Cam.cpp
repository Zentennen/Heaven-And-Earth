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

