// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC.generated.h"

/**
 * 
 */
UCLASS()
class HEAVENANDEARTH_API APC : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
};
