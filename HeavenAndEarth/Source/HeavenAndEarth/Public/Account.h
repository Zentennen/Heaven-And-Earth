#pragma once
#include "HAE.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Account.generated.h"

class UCampaignSave;

UCLASS()
class HEAVENANDEARTH_API AAccount : public AActor
{
	GENERATED_BODY()

public:
	AAccount();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 id = -1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString username;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString password;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void init(const FString& pUsername, const FString& pPassword, const int32& i);
	void load(const int32& i, UCampaignSave* saveGame);
	FString getUsername() const;
	LoginResult canLogin(const FString& username, const FString& password);
	void save(UCampaignSave* saveGame);
	UFUNCTION(BlueprintPure) FString getSaveName() const;
};
