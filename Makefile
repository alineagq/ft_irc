NAME        = ircserver

CC          = c++
FLAGS       = -Wall -Wextra -Werror -std=c++98 -g -O0
RM          = rm -rf

OBDIR       = .obj

SRC         = src/network/Socket.cpp src/network/TcpConnection.cpp \
			src/logger/Logger.cpp  src/auth/User.cpp src/core/Server.cpp \
			src/handlers/HandleUser.cpp  src/main.cpp src/handlers/HandleMessages.cpp
OBJ         = $(patsubst $(SRC_DIR)/%.cpp, $(OBDIR)/%.o, $(SRC))

INCLUDE_DIR = src/network
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
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re