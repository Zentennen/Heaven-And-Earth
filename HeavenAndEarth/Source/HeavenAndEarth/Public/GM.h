#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM.generated.h"

UCLASS()
class HEAVENANDEARTH_API AGM : public AGameModeBase
{
	GENERATED_BODY()
	virtual void PostLogin(APlayerController* NewPlayer) override;
};