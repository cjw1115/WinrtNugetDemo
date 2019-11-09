#pragma once

#include "Class.g.h"

namespace winrt::Reference_cppwinrtComponent::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

		hstring ProxyDate();
    };
}

namespace winrt::Reference_cppwinrtComponent::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
