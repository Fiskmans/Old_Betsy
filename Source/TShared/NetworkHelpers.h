#pragma once
#include <functional>

struct sockaddr;

/// <aAddress> The address to parse </aAddress>
/// <aAddressTarget> The target structure to parse into
/// <aAllowFailure> Whether failure is an accepted state, if set to false function will halt until it returns true
/// <aPrinter> a Function that'll print eventual errors and debugging prints
///		<std::string> The error or message that should be printed
///		<bool> true if this is an error
bool TranslateAddress(const std::string& aAddress, sockaddr* aAddressTarget, bool aAllowFailure = false, std::function<void(std::string,bool)> aPrinter = [](std::string,bool) {/*noop*/});

///<aAddress> A address to make readable</aAddress>
///<return>The address translated into a readable address</return>
std::string ReadableAddress(sockaddr* aAddress);