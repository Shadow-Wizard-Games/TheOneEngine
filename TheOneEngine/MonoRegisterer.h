#pragma once

#include "../mono/include/mono/jit/jit.h"
#include "../mono/include/mono/metadata/assembly.h"
#include "../mono/include/mono/metadata/attrdefs.h"

#include <string>

class MonoRegisterer
{
public:
	static void RegisterFunctions();

	static bool CheckMonoError(MonoError& error);
	static std::string MonoStringToUTF8(MonoString* monoString);
};

