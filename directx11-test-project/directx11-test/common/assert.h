#pragma once

/**
Compile-time assert.
static_assert is already good enough
*/
#define XTEST_STATIC_ASSERT(bool_constexpr, message) static_assert(bool_constexpr, message)


/**
Enables or disable all asserts.
Define this macro with value 1 if you want to enable the custom assert mechanism, 0 otherwise.
If not defined, this macro assume the same value of NDEBUG.
*/
#ifndef XTEST_ENABLE_ASSERT
#	ifndef NDEBUG
#		define XTEST_ENABLE_ASSERT 1 // turn on asserts
#	else
#		define XTEST_ENABLE_ASSERT 0 // turn off asserts
#	endif
#endif // ifndef XTEST_ENABLE_ASSERT



#if XTEST_ENABLE_ASSERT
/**
Use this macro tho assert a bool expression.
If the assert fails, the program paused and the user is able to decide between ignore
and continue, abort, or break into the code for debugging.
All XTEST_ASSERTs can be disabled/enabled via XTEST_ENABLE_ASSERT macro.
Example of usage:
int a = ...;
XTEST_ASSERT(a > 0, u8"Unexpected value:%d", a);
@param expr	 The expression to test.
@param message  The error message or format to display (optional).
@param vars	 The variables to insert into the message format (optional).
*/
#	define XTEST_ASSERT(expr, ...) \
		do \
		{ \
			static bool sIgnoreThisAssert = false; \
			if (!(expr) && !sIgnoreThisAssert) \
			{ \
				/* ##__VA_ARGS__ is a GCC extension to remove the trailing comma in case of zero arguments, MSVC do it automatically.*/ \
				xtest::common::_internal::AssertContext _assertContext(L#expr, WDATE, WTIME, WFILE, WFUNC, __LINE__, ##__VA_ARGS__); \
				switch ( xtest::common::_internal::ProcessAssert(_assertContext) ) \
				{ \
					case xtest::common::_internal::UserAction::action_ignore: \
						break; \
					case xtest::common::_internal::UserAction::action_ignore_always: \
						sIgnoreThisAssert = true; \
						break; \
					case xtest::common::_internal::UserAction::action_abort: \
						abort(); \
						break; \
					case xtest::common::_internal::UserAction::action_debug: \
						__debugbreak(); \
				} \
			} \
		} while (false)

#else
#	define XTEST_ASSERT(expr, ...) __noop
#endif // if XTEST_ENABLE_ASSERT


/**
Use this macro to verify a bool expression.
XTEST_VERIFY similar to XTEST_ASSERT but is always enabled.
If the test fails, the user is notified and the program aborts.
Example of usage:
int a = ...;
XTEST_VERIFY(a > 0, u8"Unexpected value:%d", a);
@param expr	 The expression to test.
@param message  The error message or format to display (optional).
@param vars	 The variables to insert into the message format (optional).
*/
#define XTEST_VERIFY(expr, ...) \
	do \
	{ \
		if (!(expr)) \
		{ \
			xtest::common::_internal::AssertContext context(L#expr, WDATE, WTIME, WFILE, WFUNC, __LINE__, ##__VA_ARGS__); \
			xtest::common::_internal::ProcessVerify(context); \
			abort(); \
		} \
	} while (false)


namespace xtest {
namespace common {
namespace _internal {

	//internal use only, don't use this
	enum class UserAction : char
	{
		action_ignore,			/**< ignore only this time. */
		action_ignore_always,	/**< ignore until the next run. */
		action_debug,			/**< break into the code. */
		action_abort,			/**< abort execution. */
	};


	//internal use only, don't use this
	class AssertContext
	{
	public:
		AssertContext(const wchar_t* expr, const wchar_t* date, const wchar_t* time, const wchar_t* file, const wchar_t* func, int line, const wchar_t* format...);
		AssertContext(const wchar_t* expr, const wchar_t* date, const wchar_t* time, const wchar_t* file, const wchar_t* func, int line);

		const wchar_t* Expression() const { return m_expression; }
		const wchar_t* Date() const { return m_date; }
		const wchar_t* Time() const { return m_time; }
		const wchar_t* File() const { return m_file; }
		const wchar_t* Function() const { return m_function; }
		const wchar_t* Message() const { return m_message.get(); }
		int Line() const { return m_line; }

	private:

		const wchar_t* m_expression;
		const wchar_t* m_date;
		const wchar_t* m_time;
		const wchar_t* m_file;
		const wchar_t* m_function;
		int m_line;
		std::unique_ptr<wchar_t[]> m_message;

	};


	//internal use only, don't use these
	UserAction ProcessAssert(const AssertContext& context);
	void ProcessVerify(const AssertContext& context);

} // _internal
} // common
} // xtest

