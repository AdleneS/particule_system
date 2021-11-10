NAME	= ft_vox

SRCS 	= ./srcs/main.cpp ./srcs/buffer.cpp

OBJ_DIR = ./.objs
SRC_DIR = ./srcs
INC_DIR = ./include
INC_HDR =	-I./ -I./glfw/include -I./gl3w/include/GL -I./OpenCL-CLHPP/include/ -I./OpenCL-Headers/include -I./headers/
INC_LIB =	-L./glfw/src -lglfw3 -lpthread -pthread \
			-framework OpenGL -framework AppKit -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenCL
INC_H	= ./headers/vox.hpp ./headers/shader.hpp ./headers/camera.hpp

CFLAGS	:= -std=c++11 -stdlib=libc++ -Wall -Wextra -O3 -I$(INC_DIR) $(INC_HDR)
OBJS	:= $(patsubst .$(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,  $(SRCS))

all: CREATE_OBJDIR $(NAME)

CREATE_OBJDIR:
		@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
		@g++ ${CFLAGS} ${INC_LIB} $^ -o ${NAME}
		printf "\033[1m[ ft_vox ]\033[1m\t\tcompilation complete.\t\033[0m\n"

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(INC_H)
		g++ $(CFLAGS) -o $@ -c $<

mk: 
	@mkdir -p .objs

clean:
		@rm -rf $(OBJ_DIR)
		@echo "\033[1m[ ft_vox ]\033[0m\t\tobjects cleared.\033[0m"

fclean : clean
		@rm -f $(NAME)
		@echo "\033[1m[ ft_vox ]\033[0m\t\tbinary deleted.\033[0m"
re:		fclean all