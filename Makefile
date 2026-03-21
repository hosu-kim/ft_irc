NAME = ircserv

FILES =	main.cpp \
		User.cpp \
		Server.cpp \
		Channel.cpp \
		commands/ACommand.cpp \
		commands/CommandINVITE.cpp \
		commands/CommandKICK.cpp \
		commands/CommandMODE.cpp \
		commands/CommandTOPIC.cpp

SRC_DIR = src
OBJ_DIR = .obj
INC_DIR = include

SRCS = $(addprefix $(SRC_DIR)/, $(FILES))
OBJS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

COMPILER = c++
#FLAGS = -Wall -Wextra -Werror -g -std=c++98 -I$(INC_DIR)
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