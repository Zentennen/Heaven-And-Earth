// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameLib.h"
#include "GameFramework/Actor.h"
#include "HexMarker.generated.h"

UCLASS()
class HEAVENANDEARTH_API AHexMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void setPos(const FGridIndex& gi);
};
