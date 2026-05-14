- Files To implement
	.
	├── include
	│   ├── Channel.hpp [DONE]
	│   ├── CmdFactory.hpp
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
	│   │   ├── CmdQUIT.hpp
	│   │   ├── CmdTOPIC.hpp
	│   │   └── CmdUSER.hpp
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
	    │   ├── CmdMODE.cpp [DONE] 08.05.2026
	    │   ├── CmdPASS.cpp [DONE]
	    │   ├── CmdPING.cpp [DONE] 12.05.2026
	    │   ├── CmdPRIMSG.cpp [Working] 13.05.2026
	    │   ├── CmdQUIT.cpp
	    │   ├── CmdTOPIC.cpp
	    │   └── CmdUSER.cpp
	    ├── main.cpp [DONE]
	    ├── Server.cpp [DONE] => But some code blocks need to be moved to the Cmd classes - hosu [23.03.26]
	    └── User.cpp [DONE]
