#include "SlowingItem.h"

#include "SpartaCharacter.h"

ASlowingItem::ASlowingItem()
{
	ItemType = "Slowing";
	SlowingDelay = 7.0f;
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->SetNormalSpeed(0.5f);
		}

		if (StaticMesh)
		{
			StaticMesh->SetVisibility(false);
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			SlowingTimerHandle,
			this,
			&ASlowingItem::RemoveDebuff,
			SlowingDelay,
			false
		);
	}
}

void ASlowingItem::RemoveDebuff()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ACharacter* Character = Cast<ACharacter>(PlayerController->GetCharacter()))
		{
			if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Character))
			{
				PlayerCharacter->SetNormalSpeed(2.0f);
			}
		}
	}

	DestroyItem();
}

