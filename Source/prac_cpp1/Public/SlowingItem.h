#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class PRAC_CPP1_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float SlowingDelay;

	FTimerHandle SlowingTimerHandle;
	
	virtual void ActivateItem(AActor* Activator) override;
	void RemoveDebuff();
};
