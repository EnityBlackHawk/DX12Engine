#pragma once
#include <Windows.h>
#include <sstream>

class Exception
{
public:
	static void ErrorMessage(HRESULT hResult, int line, const char* file)
	{
		LPTSTR erroMessage = NULL;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&erroMessage, 0, nullptr);

		std::ostringstream os;
		os << "Code: " << hResult << "\n";
		os << "Line: " << line << "\n";
		os << "File: " << file << "\n";
		os << erroMessage;
		LocalFree(erroMessage);

		MessageBox(NULL, os.str().c_str(), "Error", MB_ICONERROR | MB_OK);
	}
	static void ErrorMessage(const char* text, int line, const char* file)
	{
		std::ostringstream os;
		//os << "Code: " << hResult << "\n";
		os << "Line: " << line << "\n";
		os << "File: " << file << "\n";
		os << text;

		MessageBox(NULL, os.str().c_str(), "Error", MB_ICONERROR | MB_OK);
	}

};

#define ERROR_MESSAGE(x) Exception::ErrorMessage(x, __LINE__, __FILE__)
#define ERROR_MESSAGE_AND_RETURN(x) Exception::ErrorMessage(x, __LINE__, __FILE__); return
#define ERROR_MESSAGE_LAST_ERROR() Exception::ErrorMessage(GetLastError(), __LINE__, __FILE__)
#define ERROR_IF_FAILED(hr) if (FAILED(hr)) ERROR_MESSAGE(hr)
#define ERROR_IF_FAILED_AND_RETURN(hr) if (FAILED(hr)){ ERROR_MESSAGE(hr); return; }
#define ERROR_IF_FAILED_AND_RETURN_VALUE(hr, x) if (FAILED(hr)){ ERROR_MESSAGE(hr); return x; }
#define ERROR_IF_FAILED_AND_EXIT(hr) if(FAILED(hr)) {ERROR_MESSAGE(hr); exit(0);}
#define ERROR_IF_NULL_AND_RETURN(value, message) if(!(value)) { Exception::ErrorMessage(message, __LINE__, __FILE__); return;}

#define ERROR_IF_NULL_AND_THROW(value, message) if(!(value)) { Exception::ErrorMessage(message, __LINE__, __FILE__); throw std::exception(message);}
#define ERROR_MESSAGE_AND_THROW(x) Exception::ErrorMessage(x, __LINE__, __FILE__); throw std::exception(x)


