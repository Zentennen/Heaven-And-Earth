// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameLib.h"
#include "Unit.generated.h"

UCLASS()
class HEAVENANDEARTH_API AUnit : public AActor
{
	GENERATED_BODY()
private:
	TMap<Health, int> health;

protected:
	virtual void BeginPlay() override;

public:	
	AUnit();
	virtual void Tick(float DeltaTime) override;

};
