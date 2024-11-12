#include <pet/Script.hpp>

#include <pet/parser/Parser.hpp>

#include <pet/runtime/Interpreter.hpp>
#include <pet/runtime/Globals.hpp>

#include <toolkit/Profiler.hpp>

namespace pet
{
	namespace
	{
		void RegisterFunction(Context& context, Globals& globals, const FunctionPtr& function)
		{
			PET_CHECK(globals.emplace(context.GetIdentifierPool().Add(function->GetName()), std::make_shared<Value>(function)).second,
					  RuntimeError(StringBuilder() % "Redeclaration of function '" % function->GetName() % "'"));
		}
	}

	class Script::Impl
	{
	private:
		Context		_context;
		Interpreter _interpreter;

	public:
		Impl() : _interpreter(_context, RegisterGlobals(_context))
		{
		}

		void Run(std::istream& stream)
		{
			PET_PROFILE_DEBUG("Script::Run()");

			Parser parser(_context, stream);
			while (!parser.IsEndOfStream()) _interpreter.Execute(parser.GetStatement());
		}

	private:
		Globals RegisterGlobals(Context& context)
		{
			auto globals = Globals();

			RegisterFunction(context, globals, std::make_shared<AssertFunction>());

			RegisterFunction(context, globals, std::make_shared<NowFunction>());

			RegisterFunction(context, globals, std::make_shared<PrintFunction>());
			RegisterFunction(context, globals, std::make_shared<ReadLnFunction>());

			RegisterFunction(context, globals, std::make_shared<TypeFunction>());
			RegisterFunction(context, globals, std::make_shared<IntFunction>());
			RegisterFunction(context, globals, std::make_shared<FloatFunction>());
			RegisterFunction(context, globals, std::make_shared<StrFunction>());

			RegisterFunction(context, globals, std::make_shared<LenFunction>());

			return globals;
		}
	};

	Script::Script() : _impl(std::make_unique<Impl>())
	{
	}

	Script::~Script()
	{
	}

	void Script::Run(std::istream& istream)
	{
		_impl->Run(istream);
	}
}
