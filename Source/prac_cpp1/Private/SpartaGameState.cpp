#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"


ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	LevelDuration = 10.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	WaveDuration = {15.0f, 30.0f, 45.0f};
	CurrentWaveIndex = 0;
	MaxWaves = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	
	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
		);
}


int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	CurrentWaveIndex = 0;

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
	
	StartWave();
}


void ASpartaGameState::StartWave()
{
	CurrentWaveIndex++;
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
		FString::Printf(TEXT("Wave %d !!!"),CurrentWaveIndex));

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(PlayerController->GetCharacter()))
		{
			UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(PlayerCharacter->GetMovementComponent());
			UCameraComponent* PlayerCamera = PlayerCharacter->FindComponentByClass<UCameraComponent>();

			if (CurrentWaveIndex ==3)
			{
				if (Movement)
				{
					Movement->GroundFriction = 0.1f;
					Movement->BrakingDecelerationWalking = 200.0f;
				}
				if (PlayerCamera)
				{
					PlayerCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
					PlayerCamera->PostProcessSettings.VignetteIntensity = 5.0f;
				}
			}
			else if (CurrentWaveIndex == 2)
			{
				if (Movement)
				{
					Movement->GroundFriction = 0.1f;
					Movement->BrakingDecelerationWalking = 200.0f;
				}
				if (PlayerCamera)
				{
					PlayerCamera->PostProcessSettings.bOverride_VignetteIntensity = false;
				}
			}
			else
			{
				if (Movement)
				{
					Movement->GroundFriction = 8.0f;
					Movement->BrakingDecelerationWalking = 2048.0f;
				}
				if (PlayerCamera)
				{
					PlayerCamera->PostProcessSettings.bOverride_VignetteIntensity = false;
				}
			}
		}
	}
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	float CurrentWaveDuration = 0.0f;
	if (WaveDuration.IsValidIndex(CurrentWaveIndex -1))
	{
		CurrentWaveDuration = WaveDuration[CurrentWaveIndex -1];
	}

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ASpawnVolume::StaticClass(),FoundVolumes);

	int32 ItemToSpawn = 15*CurrentWaveIndex;
	
	for (int32 i=0; i<ItemToSpawn; i++ )
	{
		if (FoundVolumes.Num()>0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}
	
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		CurrentWaveDuration,
		false
	);
}

void ASpartaGameState::OnWaveTimeUp()
{
	if (CurrentWaveIndex < MaxWaves)
	{
		StartWave();
	}
	else
	{
		EndLevel();	
	}
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected %d/ %d"),
		CollectedCoinCount,
		SpawnedCoinCount);
	if (SpawnedCoinCount > 0 && CollectedCoinCount>=SpawnedCoinCount)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		OnWaveTimeUp();
	}
}

void ASpartaGameState::EndLevel()
{
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"),RemainingTime)));
				}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"),CurrentWaveIndex)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"),SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"),CurrentLevelIndex+1)));
				}
			}
		}
	}
}