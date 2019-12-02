// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HostSave.generated.h"

/**
 * 
 */
UCLASS()
class HEAVENANDEARTH_API UHostSave : public USaveGame
{
	GENERATED_BODY()
public:
	FString lastCampaignName;
};
