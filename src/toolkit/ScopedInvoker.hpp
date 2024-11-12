#pragma once

#include <toolkit/Macro.hpp>

namespace pet
{
	class ScopedInvoker
	{
		PET_NON_COPYABLE(ScopedInvoker);

		using FuncType = std::function<void()>;

	private:
		FuncType _func;

	public:
		explicit ScopedInvoker(const FuncType& func) : _func(func)
		{
		}

		~ScopedInvoker()
		{
			_func();
		}
	};
}
