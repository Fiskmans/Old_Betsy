#pragma once

namespace SvnIntegration
{
	bool CheckForUpdates(char* aBuffer, int aSize);

	void LogMessages(char* aBuffer, int aSize);

	void Update();

	bool IsSupported();
}