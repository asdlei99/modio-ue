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
#include "ModioSubsystem.h"
#include "Tests/Commands/ModioTestCommandBase.h"

#if WITH_DEV_AUTOMATION_TESTS
class FModioGetTermsOfUseAsyncCommand : public FModioTestLatentCommandBaseExpectedResult
{

public:
	FModioGetTermsOfUseAsyncCommand(FAutomationTestBase* AssociatedTest,
									 EModioErrorCondition ExpectedResult)
		: FModioTestLatentCommandBaseExpectedResult(AssociatedTest, ExpectedResult)
	{}

	using FModioTestLatentCommandBase::Update;
	virtual void Start() override
	{
		Modio->GetTermsOfUseAsync(FOnGetTermsOfUseDelegateFast::CreateSP(this, &FModioGetTermsOfUseAsyncCommand::Callback));
	}
	void Callback(FModioErrorCode ec, TOptional<FModioTerms> Terms)
	{
		CurrentTest->TestEqual("GetTermsOfUse completes without errors", CheckExpected(ec), true);
		CurrentTest->TestTrue("When GetTermsOfUse returns without error, Terms should have a value",
							  Terms.IsSet());
		Done();
	}
};


#endif