#include "stdafx.h"
#include "assert.h"
#include <application/resources/resource.h>
#include <stdarg.h>


xtest::common::_internal::AssertContext::AssertContext(const wchar_t* expr, const wchar_t* date, const wchar_t* time, const wchar_t* file, const wchar_t* func, int line, const wchar_t* format...)
	:m_expression(expr), m_date(date), m_time(time), m_file(file), m_line(line), m_function(func)
{
	if (format)
	{
		va_list args;
		va_start(args, format);

		size_t requiredSize = _vsnwprintf(nullptr, 0, format, args) + 1;

		m_message.reset(new wchar_t[requiredSize]);
		_vsnwprintf(m_message.get(), requiredSize, format, args);

		va_end(args);
	}
}


xtest::common::_internal::AssertContext::AssertContext(const wchar_t* expr, const wchar_t* date, const wchar_t* time, const wchar_t* file, const wchar_t* func, int line)
	:m_expression(expr), m_date(date), m_time(time), m_file(file), m_line(line), m_function(func), m_message(nullptr)
{}



namespace {

	INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
			// Windows Unicode function accept UTF16 stored in wchar_t type
			SetDlgItemTextW(hwnd, IDASSERTMESSAGE, reinterpret_cast<wchar_t*>(lParam));
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDDEBUG:
				EndDialog(hwnd, IDDEBUG);
				return TRUE;

			case IDABORT:
				EndDialog(hwnd, IDABORT);
				return TRUE;

			case IDIGNORE:
				EndDialog(hwnd, IDIGNORE);
				return TRUE;

			case IDIGNOREALWAYS:
				EndDialog(hwnd, IDIGNOREALWAYS);
				return TRUE;

			default:
				return FALSE;
			}

		default:
			return FALSE;
		}
	}
} // unnamed namespace


xtest::common::_internal::UserAction xtest::common::_internal::ProcessAssert(const AssertContext& context)
{
	INT_PTR ret = DialogBoxParamW(nullptr, MAKEINTRESOURCE(ID_ASSERTDIALOG), nullptr, DialogProc, reinterpret_cast<LPARAM>(context.Message()));

	switch (ret)
	{
	case IDDEBUG:
		return UserAction::action_debug;
	case IDABORT:
		return UserAction::action_abort;
	case IDIGNORE:
		return UserAction::action_ignore;
	case IDIGNOREALWAYS:
		return UserAction::action_ignore_always;
	default:
		return UserAction::action_abort;
	}
}


void xtest::common::_internal::ProcessVerify(const AssertContext& context)
{
	MessageBoxW(nullptr, context.Message(), L"Fatal Error", MB_ICONERROR | MB_OK);
}

