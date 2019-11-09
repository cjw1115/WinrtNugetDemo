#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"
#include <ctime>
#include <stdio.h>

namespace winrt::Nuget::implementation
{
	std::wstring CharToWchar(const char* c, size_t m_encode = CP_ACP) 
	{ 
		std::wstring str;	
		int len = MultiByteToWideChar(m_encode, 0, c, strlen(c), NULL, 0);	
		wchar_t* m_wchar = new wchar_t[len + 1];	
		MultiByteToWideChar(m_encode, 0, c, strlen(c), m_wchar, len);	
		m_wchar[len] = '\0';	
		str = m_wchar;	
		delete m_wchar;	
		return str; 
	}
	
	hstring Class::Date()
	{
		time_t now = time(0);
		char buffer[2014];
		memset(buffer, 0, sizeof(buffer));
		ctime_s(buffer, sizeof(buffer), &now);
		auto date = CharToWchar(buffer);
		return hstring(date.c_str());
	}
}
