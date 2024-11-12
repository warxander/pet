#pragma once

#include <istream>
#include <memory>

namespace pet
{
	class Script
	{
		class Impl;
		std::unique_ptr<Impl> _impl;

	public:
		Script();
		~Script();

		void Run(std::istream& stream);
	};
}
