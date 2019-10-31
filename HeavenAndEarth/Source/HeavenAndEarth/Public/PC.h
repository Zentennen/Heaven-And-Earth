#pragma once

#include "HexMarker.h"
#include "Account.h"
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool isGM;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<AHexMarker> markerClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) ActionMode mode;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AHexMarker*> moveMarkers;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AHexMarker*> pathMarkers;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AHexMarker* cursorMarker;
	virtual void BeginPlay() override;
	bool canCommandUnit(AUnit* u) const;
	UFUNCTION(BlueprintCallable) void createUnit(const FVector& location);
	UFUNCTION(BlueprintCallable) void selectUnit(AUnit* u);
	UFUNCTION(BlueprintCallable) void unselect();
	UFUNCTION(BlueprintCallable) void setMoveMarkers(const TArray<FGridIndex>& positions);
	UFUNCTION(BlueprintCallable) void hideMoveMarkers();
	UFUNCTION(BlueprintCallable) void setCursor(const FGridIndex& pos);
	UFUNCTION(Client, Reliable) void returnLoginResult(LoginResult result);
	UFUNCTION(Server, Reliable, WithValidation) void tryLogin(const FString& username, const FString& password);
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation) void moveCommand(AUnit* u, FGridIndex destination);
	UFUNCTION(BlueprintImplementableEvent) void onSelect(AUnit* selectedUnit);
	UFUNCTION(BlueprintImplementableEvent) void onUnselect();
	UFUNCTION(BlueprintImplementableEvent) void onLoginAttempt(const LoginResult& result);
	UFUNCTION(BlueprintImplementableEvent) void onInit();
public:
	void beginTurn();
	void endTurn();
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AUnit* unit;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AAccount* account;
	UFUNCTION(BlueprintCallable) void updatePathMarkers(const TArray<FGridIndex>& newPath);
	UFUNCTION(BlueprintCallable) void updateMoveMarkers();
	UFUNCTION(BlueprintCallable) void initialize();
	UFUNCTION(Client, Reliable) void requestLogin();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation) void requestAccount(const FString& username, const FString& password);
};
