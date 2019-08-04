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
};
