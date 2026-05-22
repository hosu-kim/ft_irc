#include "utils.hpp"

std::vector<std::string> split_params(const std::string& params, char delimiter) {
	std::vector<std::string> output;
	std::string token;
	std::stringstream ss(params);
	while (std::getline(ss, token, delimiter))
	return output;
}