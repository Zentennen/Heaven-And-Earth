#pragma once

#include "GameLib.h"
#include "GameFramework/Actor.h"
#include "Scenery.generated.h"

UCLASS()
class HEAVENANDEARTH_API AScenery : public AActor
{
	GENERATED_BODY()
	
public:	
	AScenery();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadOnly) FGridIndex center;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FTile> tiles;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void setPosition(const FGridIndex& gi);
};
