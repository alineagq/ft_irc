NAME        = ircserver

CC          = c++
FLAGS       = -Wall -Wextra -Werror -std=c++98 -g -O0
RM          = rm -rf

OBDIR       = .obj

SRC         = 	src/applications/CommandHandler.cpp\
				src/applications/cmdPass.cpp \
				src/applications/cmdNick.cpp \
				src/applications/cmdUser.cpp \
				src/applications/cmdJoin.cpp \
				src/applications/cmdPrivMsg.cpp \
				src/applications/cmdKick.cpp \
				src/applications/cmdInvite.cpp \
				src/applications/cmdCap.cpp \
				src/domain/entities/Channel.cpp \
				src/domain/entities/Server.cpp \
				src/domain/entities/User.cpp \
				src/domain/exceptions/ServerException.cpp \
				src/entrypoint/utils/utils.cpp \
				src/entrypoint/start_server.cpp \
				src/infrastructure/configuration/Logger.cpp \
				src/infrastructure/network/Socket.cpp \

OBJ         = $(patsubst $(SRC_DIR)/%.cpp, $(OBDIR)/%.o, $(SRC))

INCLUDE_DIR = src/nework
SRC_DIR     = src
TESTS_DIR   = tests

GREEN       = \033[92;5;118m
YELLOW      = \033[93;5;226m
GRAY        = \033[33;2;37m
RESET       = \033[0m
CURSIVE     = \033[3m

all: $(NAME)

$(NAME): $(OBJ)
	@printf "$(CURSIVE)$(GRAY)    - Compiling $(NAME)... $(RESET)\n"
	@$(CC) $(OBJ) -o $(NAME)
	@printf "$(GREEN)- Executable ready.\n$(RESET)"

$(OBDIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	@$(RM) -rf $(OBDIR)
	@printf "$(YELLOW)    - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME) server_error.log server.log
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re