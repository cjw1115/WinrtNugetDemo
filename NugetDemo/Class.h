#pragma once

#include "Class.g.h"

namespace winrt::Nuget::implementation
{
	struct Class : ClassT<Class>
	{
		Class() = default;

		hstring Date();
	};
}

namespace winrt::Nuget::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
