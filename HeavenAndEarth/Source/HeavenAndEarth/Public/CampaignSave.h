// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CampaignSave.generated.h"

UCLASS()
class HEAVENANDEARTH_API UCampaignSave : public USaveGame
{
	GENERATED_BODY()
public: 
	uint32 numAccounts;
	uint32 numUnits;
};
