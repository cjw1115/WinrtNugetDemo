#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

#include <winrt/Nuget.h>

namespace winrt::Reference_cppwinrtComponent::implementation
{
	hstring Class::ProxyDate()
	{
		Nuget::Class demo;
		auto re = L" Powerred by CppWinRT! " + demo.Date();
		return re;
	}
}
