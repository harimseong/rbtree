NAME		= rbtree_test

CFLAGS	= -Wall -Wextra -std=c++14 -g -fsanitize=address

MAIN		=	rbtree_test.o
OBJS		=	rbtree_write.o \
					rbtree_read.o

SRCS		= $(OBJS:%.o=%.c)

all: $(NAME)

$(NAME): $(OBJS) rbtree_test.o
	$(CXX) $(CFLAGS) -o $@ $^ -I.

$(MAIN): %.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^ -I.

$(OBJS): %.o: %.c
	$(CXX) $(CFLAGS) -o $@ -c $^ -I.

clean:
	$(RM) $(OBJS) $(MAIN)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re

