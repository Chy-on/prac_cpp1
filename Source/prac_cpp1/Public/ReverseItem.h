#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReverseItem.generated.h"

UCLASS()
class PRAC_CPP1_API AReverseItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AReverseItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ReverseDelay;

	FTimerHandle ReverseTimerHandle;
	
	virtual void ActivateItem(AActor* Activator) override;
	void RemoveDebuff();
};
