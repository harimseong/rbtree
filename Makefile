NAME			:= rbtree_test


C					:= clang++
CFLAGS		:= -Wall -Wextra -std=c++14 -O3 -DRB_VIS


CXX				:= clang++
CXXFLAGS	:= -Wall -Wextra -std=c++14 -O3


DEBUGFLAGS:= -g -fsanitize=address #-DRB_DEBUG


SRC_CPP		:=  rbtree_test.cpp
SRC_C			:=	rbtree_write.c\
							rbtree_read.c
OBJ_CPP		:= $(SRC_CPP:%.cpp=%.o)
OBJ_C			:= $(SRC_C:%.c=%.o)


STATE     :=  $(shell ls .DEBUG 2> /dev/null)
ifeq ($(STATE), .DEBUG)
CFLAGS  		+=  $(DEBUGFLAGS)
CXXFLAGS  	+=  $(DEBUGFLAGS)
COMPILE_MODE:=  .DEBUG
else
COMPILE_MODE:=  .RELEASE
endif


all: $(COMPILE_MODE)
	$(MAKE) $(NAME)

release: .RELEASE
	$(MAKE) all

debug: .DEBUG
	$(MAKE) all

.RELEASE:
	$(MAKE) fclean
	touch .RELEASE

.DEBUG:
	$(MAKE) fclean
	touch .DEBUG

$(NAME): $(OBJ_CPP) $(OBJ_C)
	$(CXX) $(CXXFLAGS) -o $@ $^ -I.

$(OBJ_CPP): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< -I.

$(OBJ_C): %.o: %.c
	$(C) $(CFLAGS) -o $@ -c $< -I.

clean:
	$(RM) $(OBJ_C) $(OBJ_CPP) .DEBUG .RELEASE

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re

