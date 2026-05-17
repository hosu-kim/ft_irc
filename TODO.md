- Files To implement
	.
	├── include
	│   ├── Channel.hpp [DONE]
	│   ├── CmdFactory.hpp [Working] 15.05.26
	│   ├── commands
	│   │   ├── ACmd.hpp [DONE]
	│   │   ├── CmdINVITE.hpp [DONE]
	│   │   ├── CmdJOIN.hpp [DONE]
	│   │   ├── CmdKICK.hpp [DONE] 08.05.2026
	│   │   ├── CmdMODE.hpp [DONE] 08.05.2026
	│   │   ├── CmdNICK.hpp [Working] 12.05.2026 => [DONE] 13.05.2026
	│   │   ├── CmdPASS.hpp [DONE]
	│   │   ├── CmdPING.hpp [DONE] 12.05.2026
	│   │   ├── CmdPRIMSG.hpp [Working] 13.05.2026 => [DONE] 14.05.2026
	│   │   ├── CmdQUIT.hpp [DONE] 14.05.2026
	│   │   ├── CmdTOPIC.hpp [DONE] 14.05.2026
	│   │   └── CmdUSER.hpp [Working] 14.05.2026 [DONE] 15.05.2026
	│   ├── Server.hpp [DONE] => But some code blocks need to be moved to the Cmd classes - hosu (23.03.26)
	│   └── User.hpp [DONE]
	├── Makefile [DONE]
	└── src
	    ├── Channel.cpp [DONE]
	    ├── CmdFactory.cpp
	    ├── commands
	    │   ├── ACmd.cpp [DONE]
	    │   ├── CmdINVITE.cpp [DONE]
	    │   ├── CmdJOIN.cpp [DONE]
	    │   ├── CmdKICK.cpp [Working] 12.05.2026 => [DONE] 13.05.2026
	    │   ├── CmdPART.cpp 
	    │   ├── CmdMODE.cpp [DONE] 08.05.2026
	    │   ├── CmdPASS.cpp [DONE]
	    │   ├── CmdPING.cpp [DONE] 12.05.2026
	    │   ├── CmdPONG.cpp 
	    │   ├── CmdPRIMSG.cpp [Working] 13.05.2026
	    │   ├── CmdQUIT.cpp [DONE] 14.05.2026
	    │   ├── CmdTOPIC.cpp [DONE] 14.05.2026
	    │   └── CmdUSER.cpp [Working] 14.05.2026 [DONE] 15.05.2026
	    ├── main.cpp [DONE]
	    ├── Server.cpp [DONE] => But some code blocks need to be moved to the Cmd classes - hosu [23.03.26]
	    └── User.cpp [DONE]
