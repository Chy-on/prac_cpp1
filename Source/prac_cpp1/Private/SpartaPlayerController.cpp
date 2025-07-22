#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SpartaGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SpartaGameInstance.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

ASpartaPlayerController::ASpartaPlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),JumpAction(nullptr),
	LookAction(nullptr),SprintAction(nullptr),
	HudWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (UButton* Button = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("MainMenuButton"))))
		{
			if (bIsRestart)
			{
				Button->SetVisibility(ESlateVisibility::Visible);
			}else
			{
				Button->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		
		if (UButton* Button = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("ExitButton"))))
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Button->Slot))
			{
				FVector2D CurrentPosition = CanvasSlot->GetPosition();
				if (bIsRestart)
				{
					CanvasSlot->SetPosition(FVector2D(CurrentPosition.X,800.0f));
				}else
				{
					CanvasSlot->SetPosition(FVector2D(CurrentPosition.X,650.0f));
				}	
			}
		}
		
		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"),SpartaGameInstance->TotalScore)
					));
				}
			}

			UFunction* PlayAnimFunc2 = MainMenuWidgetInstance->FindFunction(FName("PlayFadeOutAnim"));
			if (PlayAnimFunc2)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc2, nullptr);
			}
		}
		else
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayFadeInAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}
		}
	}
}

void ASpartaPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HudWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HudWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>():nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(),FName("BasicLevel"));
	SetPause(false);
}

void ASpartaPlayerController::ExitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}


