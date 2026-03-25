#include "CmdKICK.hpp"

/* ORTHODOX CANONICAL FORM */
// vv default constructor
CmdKick::CmdKick() : ACmd("", std::vector<std::string>()) {}
// vv parameterized constructor
CmdKick::CmdKick(std::string cmd, std::vector<std::string> params)
	: ACmd(cmd, params) {}
CmdKick::CmdKick(const CmdKick& src) : ACmd(src) {
	// vvv this class has no member variable to be copied.
	(void)src;
}
CmdKick& CmdKick::operator=(const CmdKick& src) {
	if (this != &src)
		this->ACmd::operator=(src);
	return *this;
}
CmdKick::~CmdKick() {}