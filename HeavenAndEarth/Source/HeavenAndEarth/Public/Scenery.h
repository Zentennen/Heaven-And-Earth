#pragma once
#include "Config.h"
#include "GameFramework/Actor.h"
#include "Scenery.generated.h"

UCLASS()
class HEAVENANDEARTH_API AScenery : public AActor
{
	GENERATED_BODY()

public:
	AScenery();

protected:
	bool initialized;
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly) FGridIndex center;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FTile> tiles;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void setPosition(const FGridIndex& gi);
	UFUNCTION() void init();
};
