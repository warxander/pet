#include <pet/Script.hpp>

#include <toolkit/StringUtils.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace pet;

namespace
{
	void ProcessQuitCommand()
	{
		std::cout << "Quitted" << std::endl;
		std::exit(EXIT_SUCCESS);
	}

	using CommandHandlerType = std::function<void(Script&, const std::vector<std::string>&)>;
	std::unordered_map<std::string_view, CommandHandlerType> CommandHandlers = {{"quit", std::bind(ProcessQuitCommand)}};
}

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		const std::string_view scriptFileName(argv[1]);
		std::ifstream		   istream(scriptFileName.data());

		if (istream)
		{
			try
			{
				Script().Run(istream);
			}
			catch (const std::exception& ex)
			{
				std::cout << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
		}
		else
			std::cout << "IOError: Failed to open file \'" << scriptFileName << "'" << std::endl;
	}
	else
	{
		std::cout << "Use '$' prefix for commands.\nSupported commands: ";
		for (const auto& command : CommandHandlers) std::cout << command.first << " ";
		std::cout << std::endl;

		Script script;

		while (true)
		{
			std::cout << "\n>> ";

			std::string input;
			if (!std::getline(std::cin, input))
				ProcessQuitCommand();

			if (input.empty())
				continue;

			if (input == "quit" || input == "exit")
			{
				std::cout << "Use $quit to exit the program." << std::endl;
				continue;
			}

			if (input[0] == '$')
			{
				if (input.size() == 1)
				{
					std::cout << "No command." << std::endl;
					continue;
				}

				const auto args = StringUtils::Split(input.substr(1));

				const auto& command = args[0];
				const auto	it = CommandHandlers.find(command);
				if (it == CommandHandlers.end())
				{
					std::cout << "CommandError: Unknown command '" << command << "'" << std::endl;
					continue;
				}

				try
				{
					it->second(script, args);
				}
				catch (const std::exception& ex)
				{
					std::cout << "CommandError: " << ex.what() << std::endl;
					continue;
				}
			}
			else
			{
				try
				{
					std::stringstream inputStream(input);
					script.Run(inputStream);
				}
				catch (const std::exception& ex)
				{
					std::cout << ex.what() << std::endl;
					continue;
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
