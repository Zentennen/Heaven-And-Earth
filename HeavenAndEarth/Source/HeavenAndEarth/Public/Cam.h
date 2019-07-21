// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameLib.h"
#include "GameFramework/Pawn.h"
#include "Cam.generated.h"

UCLASS()
class HEAVENANDEARTH_API ACam : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float movementSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float zoomSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float maxZoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float minZoom;
	ACam();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable) void move(const float& deltaTime, const float& forward, const float& back, const float& left, const float& right);
	UFUNCTION(BlueprintCallable) static int32 roll(uint8 num, uint8 max);
	UFUNCTION(BlueprintCallable) static FGridIndex vectorToGridIndex(const FVector& vec);
	UFUNCTION(BlueprintCallable) static FGridIndex vector2DToGridIndex(const FVector2D& vec);
	UFUNCTION(BlueprintCallable) static FVector gridIndexToVector(const FGridIndex& gi, float z = 0.0f);
	UFUNCTION(BlueprintCallable) static FVector2D gridIndexToVector2D(const FGridIndex& gi);
};
