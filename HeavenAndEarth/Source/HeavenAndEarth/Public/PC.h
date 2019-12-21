#pragma once

#include "HexMarker.h"
#include "Unit.h"
#include "GameFramework/PlayerController.h"
#include "PC.generated.h"

class AAccount;

UCLASS()
class HEAVENANDEARTH_API APC : public APlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool isGM;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TSubclassOf<AHexMarker> markerClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AHexMarker*> moveMarkers;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<AHexMarker*> pathMarkers;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AHexMarker* cursorMarker;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable) void selectUnit(AUnit* u);
	UFUNCTION(BlueprintCallable) void unselect();
	UFUNCTION(BlueprintCallable) void setMoveMarkers(const TArray<FGridIndex>& positions);
	UFUNCTION(BlueprintCallable) void hideMoveMarkers();
	UFUNCTION(BlueprintCallable) void setCursor(const FGridIndex& pos);
	UFUNCTION(Client, Reliable) void returnLoginResult(LoginResult result);
	UFUNCTION(Server, Reliable, WithValidation) void tryLogin(const FString& username, const FString& password);
	UFUNCTION(Server, Reliable, WithValidation) void orderUnit(AUnit* u, const TArray<Order>& orders);
	UFUNCTION(Server, Reliable, WithValidation) void resetOrders(AUnit* u);
	UFUNCTION(BlueprintImplementableEvent) void onSelect(AUnit* selectedUnit);
	UFUNCTION(BlueprintImplementableEvent) void onUnselect();
	UFUNCTION(BlueprintImplementableEvent) void onLoginAttempt(const LoginResult& result);
	UFUNCTION(BlueprintImplementableEvent) void onPlayerInit();
	UFUNCTION(BlueprintImplementableEvent) void onHostInit();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AUnit* selected;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) AAccount* account;
	void hostInit();
	void beginTurn();
	void endTurn();
	bool canCommand(const AUnit* unit) const;
	bool canOrder(const AUnit* unit) const;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	UFUNCTION(BlueprintPure) bool getIsGM() const;
	UFUNCTION(BlueprintCallable) void playerInit();
	UFUNCTION(BlueprintCallable) void updatePathMarkers(const TArray<FGridIndex>& newPath);
	UFUNCTION(BlueprintCallable) void updateMoveMarkers();
	UFUNCTION(BlueprintCallable) bool tryMoveSelected(const FGridIndex& destination);
	UFUNCTION(BlueprintCallable) bool tryResetOrders();
	UFUNCTION(Client, Reliable) void requestLogin();
	UFUNCTION(Client, Reliable) void printMsg(const FString& msg);
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation) void requestAccount(const FString& username, const FString& password);
	UFUNCTION(BlueprintImplementableEvent) void show();
	UFUNCTION(BlueprintImplementableEvent) void hide();
};
