/* KICK: used when operator kicks a member of a channel

	Syntax: KICK <channel> <member> [<reason>]

	Example: KICK #42prague bad_user :Don't spam!
*/

#include "ACmd.hpp"

class CmdKick : public ACmd
{
	public:
		/* ORTHODOX CANONICAL FORM */
		CmdKick();
		CmdKick(std::string cmd, std::vector<std::string> params);
		CmdKick(const CmdKick& src);
		CmdKick& operator=(const CmdKick& src);
		virtual ~CmdKick();

		/* LOGIC FUNCTIONS */
		// virtual void execute(User &user, Server &server);
};