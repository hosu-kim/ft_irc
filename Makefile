NAME = ircserv

CORE_FILES =	main.cpp \
				User.cpp \
				Server.cpp \
				Channel.cpp \
				CmdFactory.cpp \

CMD_FILES =	commands/ACmd.cpp \
			commands/CmdINVITE.cpp \
			commands/CmdJOIN.cpp \
			commands/CmdKICK.cpp \
			commands/CmdMODE.cpp \
			commands/CmdNICK.cpp \
			commands/CmdPASS.cpp \
			commands/CmdPING.cpp \
			commands/CmdPRIMSG.cpp \
			commands/CmdQUIT.cpp \
			commands/CmdTOPIC.cpp \
			commands/CmdUSER.cpp \

FILES = $(CORE_FILES) $(CMD_FILES)

SRC_DIR = src
OBJ_DIR = .obj
INC_DIR = include

SRCS = $(addprefix $(SRC_DIR)/, $(FILES))
OBJS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

COMPILER = c++
#FLAGS = -Wall -Wextra -Werror -g -std=c++98 -I$(INC_DIR) -I$(INC_DIR)/commands
FLAGS = -std=c++98 -I$(INC_DIR) -I$(INC_DIR)/commands
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Compiling..."
	@$(COMPILER) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "Compilation successful"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	@echo "Cleaning"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re