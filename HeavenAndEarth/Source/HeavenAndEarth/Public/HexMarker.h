#pragma once
#include "Config.h"
#include "GameFramework/Actor.h"
#include "HexMarker.generated.h"

UENUM(BlueprintType)
enum class MarkerType : uint8 {
	Move, Path, Cursor, Selected
};

UCLASS()
class HEAVENANDEARTH_API AHexMarker : public AActor
{
	GENERATED_BODY()

public:
	AHexMarker();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) MarkerType markerType;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) void setProperties(const FGridIndex& gi, MarkerType type);
	UFUNCTION(BlueprintCallable) void setPos(const FGridIndex& gi);
	UFUNCTION(BlueprintImplementableEvent) void setType(const MarkerType& type);
	UFUNCTION(BlueprintImplementableEvent) void hide();
	UFUNCTION(BlueprintImplementableEvent) void show();
};
