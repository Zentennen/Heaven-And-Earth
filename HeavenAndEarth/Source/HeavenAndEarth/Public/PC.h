#pragma once

#include "GameLib.h"
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
	UFUNCTION(BlueprintCallable) void unselect();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation) void moveCommand(AUnit* u, FGridIndex destination, bool replaceCurrentOrders);
	UFUNCTION(BlueprintImplementableEvent) void onSelect(AUnit* selectedUnit);
	UFUNCTION(BlueprintImplementableEvent) void onUnselect();
	UFUNCTION(Client, Unreliable) void updatePath(const TArray<FGridIndex>& path);
public:
	void beginTurn();
	void endTurn();
	UFUNCTION(BlueprintImplementableEvent) void bUpdatePath(const TArray<FGridIndex>& path);
};
