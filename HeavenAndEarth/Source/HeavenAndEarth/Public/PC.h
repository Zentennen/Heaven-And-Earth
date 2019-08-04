// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC.generated.h"

class AUnit;

UENUM(BlueprintType)
enum class ActionMode : uint8 {
	CreatingUnit, SelectingUnit
};

UCLASS()
class HEAVENANDEARTH_API APC : public APlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AUnit* unit;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ActionMode mode;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable) void action();
	UFUNCTION(BlueprintCallable) void createUnit(const FVector& location);
	UFUNCTION(BlueprintCallable) void selectUnit(AUnit* u);
	UFUNCTION(BlueprintImplementableEvent) void onSelect(AUnit* selectedUnit);
};
