#include "Components/UltralightViewComponent.h"
#include "UltralightUE/Public/ULUEInstance.h"
#include "Rendering/ULUERenderManagerCPU.h"
#include "Engine/Texture2D.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"
#include <Ultralight/RefPtr.h>

#include "Ultralight/View.h"

class UUltralightViewComponent::Pimpl
{
public:
	ultralight::RefPtr<ultralight::View> View;
};

UUltralightViewComponent::UUltralightViewComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // No tick needed; manager handles rendering
	m_pimpl = MakeUnique<Pimpl>().Get();
}

UUltralightViewComponent::~UUltralightViewComponent()
{
	if (m_pimpl->View.get() != nullptr)
	{
		m_pimpl->View->Release();
		m_pimpl->View.reset();
		m_pimpl = nullptr;
	}
}

void UUltralightViewComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get the render manager (assumed to be stored in GameInstance or similar)
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULUEInstance* LUEInstance = GameInstance->GetSubsystem<ULUEInstance>())
			{
				RenderManager = LUEInstance->GetRenderManager();
			}
		}
	}

	if (RenderManager)
	{
		InitializeUltralightView();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UUltralightViewComponent: RenderManager not found!"));
	}
}

void UUltralightViewComponent::InitializeUltralightView()
{
	// Create the Ultralight View via the manager
	m_pimpl->View = RenderManager->CreateView(Width, Height, bTransparent);
	if (m_pimpl->View)
	{
		m_pimpl->View->LoadURL(TCHAR_TO_UTF8(*URL));

		// Create the dynamic texture
		RenderTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (RenderTexture)
		{
			RenderTexture->AddressX = TA_Clamp;
			RenderTexture->AddressY = TA_Clamp;
			RenderTexture->UpdateResource();

			// Register with the manager
			RenderManager->RegisterViewTexture(m_pimpl->View.get(), RenderTexture);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UUltralightViewComponent: Failed to create RenderTexture!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UUltralightViewComponent: Failed to create Ultralight View!"));
	}
}

void UUltralightViewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RenderManager && m_pimpl
		->
		View
	)
	{
		// Unregister the View from the manager
		RenderManager->UnregisterView(m_pimpl->View.get());
	}
	// Release the View
	m_pimpl->View->Release();
	m_pimpl->View.reset();
	m_pimpl = nullptr;

	RenderTexture = nullptr; // Texture will be garbage collected
	Super::EndPlay(EndPlayReason);
}

void UUltralightViewComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	if (m_pimpl->View.get() != nullptr)
	{
		m_pimpl->View->Release();
		m_pimpl->View.reset();
		m_pimpl = nullptr;
	}
}
