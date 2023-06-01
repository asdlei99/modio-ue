/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Core/ModioModInfoUI.h"
#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "Types/ModioImageWrapper.h"
#include "UI/BaseWidgets/ModioImage.h"
#include "UI/BaseWidgets/ModioRichTextBlock.h"
#include "UI/BaseWidgets/ModioRoundedBorder.h"
#include "UI/BaseWidgets/ModioTextBlock.h"
#include "UI/BaseWidgets/ModioUserWidgetBase.h"
#include "UI/CommonComponents/ModioRichTextButton.h"
#include "UI/EventHandlers/IModioUIAuthenticationChangedReceiver.h"
#include "UI/EventHandlers/IModioUIMediaDownloadCompletedReceiver.h"
#include "UI/Interfaces/IModioUIAsyncOperationWidget.h"
#include "UI/Styles/ModioModTileStyle.h"

#include "ModioModTileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModTileSubscribeClicked, UModioModInfoUI*, ModInfo);

/**
 *
 */
UCLASS(Abstract)
class MODIOUI_API UModioModTileBase : public UModioUserWidgetBase,
									  public IUserObjectListEntry,
									  public IModioUIMediaDownloadCompletedReceiver,
									  public IModioUIAuthenticationChangedReceiver,
									  public IModioUIAsyncOperationWidget
{
	GENERATED_BODY()

	bool bRoutedOnSetExpandedState = false;

protected:
	virtual void NativeOnInitialized() override;
	TWeakObjectPtr<UWidget> WeakExpandedWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Transient, Category = "Widgets")
	bool bCurrentExpandedState = false;
	virtual void NativeConstruct() override;

	virtual void NativeOnSetDataSource() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	virtual void NativeOnModLogoDownloadCompleted(FModioModID ModID, FModioErrorCode ec,
												  TOptional<FModioImageWrapper> Image) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void SynchronizeProperties() override;
	virtual void NativeOnSetExpandedState(bool bExpanded);
	UFUNCTION()
	virtual void HandleModLogoOperationStateChanged(EModioUIAsyncOperationWidgetState NewState);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTileClicked();

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
							  const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
							  const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets")
	FModioUIMaterialRef LoadingMaterial = FModioUIMaterialRef {"Loading"};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets")
	FModioUIMaterialRef ErrorMaterial = FModioUIMaterialRef {"ImageError"};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidget)) 
	UModioImage* Thumbnail;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidgetOptional))
	UModioUserWidgetBase* SubscriptionIndicator;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidget))
	UModioRichTextBlock* ModName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FocusTransition;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidgetOptional))
	UModioRoundedBorder* TileBorder;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidgetOptional))
	UModioImage* TileFrame;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets", meta = (BindWidget))
	UModioRichTextButton* SubscribeButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (StyleClass = "ModioModTileStyle"), Category = "Widgets")
	FModioUIStyleRef Style = FModioUIStyleRef {"DefaultModTileStyle"};

	UPROPERTY()
	class UModioUISubsystem* UISubsystem;

	FSlateSound HoveredSound;
	FSlateSound PressedSound;

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetExpandedState(bool bExpanded);
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent);
	virtual void NativeOnAuthenticationChanged(TOptional<FModioUser> User);

	FModioModID CurrentModId;
	FTimerHandle SetFocusTimerHandle;
	bool bIsUserAuthenticated;
	bool GetSubscriptionState();
	UFUNCTION()
	void EnableSubscribeButton();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Localization")
	FText SubscribeLabel;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Localization")
	FText UnsubscribeLabel;

	virtual void BuildCommandList(TSharedRef<FUICommandList> CommandList) override;

	virtual UModioModInfoUI* GetAsModInfoUIObject();

public:
	// These may need to provide a reference to the object that was clicked as well
	UPROPERTY(BlueprintAssignable)
	FOnModTileSubscribeClicked OnSubscribeClicked;

public:
	UFUNCTION(BlueprintCallable, Category = "ModioModTileBase")
	void SetExpandedState(bool bExpanded);

	UFUNCTION()
	virtual void OnModSubscriptionStatusChanged(FModioModID ID, bool Subscribed);

	UFUNCTION()
	void NativeSubscribeClicked();
};
