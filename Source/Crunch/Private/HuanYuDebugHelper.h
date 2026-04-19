#pragma once

namespace Debug
{
	static void Print(const FString& Msg, int32 InKey = -1, const FColor& InColor= FColor::MakeRandomColor())
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, InColor, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
#endif
	}
}