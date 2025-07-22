#include "ReverseItem.h"
#include "SpartaCharacter.h"

AReverseItem::AReverseItem()
{
	ItemType = "Reverse";
	ReverseDelay = 7.0f;
}

void AReverseItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->SetOffsetDirection(-1.0f);
		}

		if (StaticMesh)
		{
			StaticMesh->SetVisibility(false);
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			ReverseTimerHandle,
			this,
			&AReverseItem::RemoveDebuff,
			ReverseDelay,
			false
		);
	}
}

void AReverseItem::RemoveDebuff()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ACharacter* Character = Cast<ACharacter>(PlayerController->GetCharacter()))
		{
			if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Character))
			{
				PlayerCharacter->SetOffsetDirection(1.0f);
			}
		}
	}

	DestroyItem();
}


