/* KICK: used when operator kicks a member of a channel

	Syntax: KICK <channel> <member> [<reason>]

	Example: KICK #42prague bad_user :Don't spam!
*/

#include "ACmd.hpp"

class CmdKick : public ACmd
{
	public:
		CmdKick(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}
		virtual ~CmdKick() {}

		/* LOGIC FUNCTIONS */
		virtual void execute(User &user, Server &server);
};