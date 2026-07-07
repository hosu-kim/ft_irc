#include "ACmd.hpp"
#include "User.hpp"
#include <stdexcept>
#include <sstream>
#include <stdexcept>

// ORTHODOX CANONICAL FORM
ACmd::ACmd() {}
ACmd::ACmd(std::string cmd, std::vector<std::string> params)
	: cmd_(cmd), params_(params) {}
ACmd::ACmd(const ACmd& src) : cmd_(src.cmd_), params_(src.params_) {}
ACmd& ACmd::operator=(const ACmd& src) {
	if (this != &src) {
		this->cmd_ = src.cmd_;
		this->params_ = src.params_;
	}
	return *this;
}
ACmd::~ACmd() {}

/* GETTERS */
size_t ACmd::getParamCount() const { return this->params_.size(); }

const std::string& ACmd::getParam(size_t index) const {
	if (index >= this->getParamCount())
		throw std::out_of_range("Parameter index out of range");
	return this->params_[index];
}

std::string ACmd::combineParamsToOneStr(std::size_t start) const {
	std::string output;
	for (std::size_t i = start; i < this->getParamCount(); ++i) {
		if (!output.empty()) { output += ' '; }
		output += this->getParam(i);
	}
	if (!output.empty() && output[0] == ':') { output.erase(output.begin()); }
	return output;
}

std::vector<std::string> ACmd::splitStr(char delimiter) const {
	std::vector<std::string> output;
	if (this->params_.empty()) return output;

	std::stringstream ss(this->params_[0]);
	std::string token;

	while (std::getline(ss, token, delimiter)) 
		if (!token.empty()) output.push_back(token);

	return output;
}
