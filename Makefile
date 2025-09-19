NAME = irc

CFLAGS = -Wall -g -Wextra -Werror -std=c++98 -I. #-fsanitize=address

CC = c++

SRC_DIR = ./src

SRCS := ./src/server.cpp ./src/main.cpp ./src/client.cpp ./src/channel.cpp ./src/message.cpp ./src/parser.cpp ./src/command_router.cpp

OBJ_DIR = ./objs
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Colors
RED     			= \033[0;31m
GREEN   			= \033[0;32m
YELLOW  			= \033[0;33m
CYAN    			= \033[0;36m
WHITE   			= \033[0;37m
RESET   			= \033[0m

# Reglas
all: $(NAME) 

$(NAME): $(OBJ_DIR) $(OBJS)
	@printf "$(CYAN)[Building Main] Creating $(NAME)...\n$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)[Success] $(NAME) created successfully!\n$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)[Compiling]$(RESET) $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@printf "$(CYAN)[Directory] Creating object directory $(OBJ_DIR)...\n$(RESET)"
	@mkdir -p $(OBJ_DIR)

clean:
	@printf "$(RED)[Cleaning] Removing object files...\n$(RESET)"
	@rm -rf $(OBJ_DIR)
	@printf "$(GREEN)[Cleaned] Object files removed successfully!\n$(RESET)"

fclean: clean
	@printf "$(RED)[Full Clean] Removing binaries...\n$(RESET)"
	@rm -f $(NAME)
	@printf "$(GREEN)[Cleaned] All binaries and libraries removed successfully!\n$(RESET)"

re: fclean all

.PHONY: all clean fclean re 