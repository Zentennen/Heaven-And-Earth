#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Account.generated.h"

class UAccountSave;

UENUM(BlueprintType)
enum class LoginResult : uint8 {
	Success, Fail, NotRegistered
};

UCLASS()
class HEAVENANDEARTH_API AAccount : public AActor
{
	GENERATED_BODY()

public:
	AAccount();

protected:
	UAccountSave* save;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 id = -1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString username;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString password;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void init(const FString& pUsername, const FString& pPassword);
	void load(const int32& pId);
	FString getUsername() const;
	LoginResult canLogin(const FString& username, const FString& password);
	void saveAccount();
	UFUNCTION(BlueprintPure) FString getSaveName() const;
};
