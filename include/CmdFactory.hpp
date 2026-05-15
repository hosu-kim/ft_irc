#pragma once

#include <string>
#include <vector>

class ACmd;

class CmdFactory {
	private:
		CmdFactory();

	public:
		static std::vector<std::string> splitBySpaces(const std::string& str);

		static ACmd* createCommand(const std::vector<std::string>& tokens);
};