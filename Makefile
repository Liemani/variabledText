NAME		= variabledText
CLIBRARYD	= cLibrary
CLIBRARYF	= $(CLIBRARYD)/lib$(CLIBRARYD).a

SRCS		= program.c \
			  get_next_line.c \
			  get_next_line_utils.c
OBJS		= $(SRCS:.c=.o)



CC			= clang
INCLUDE		= -I $(CLIBRARYD)
LIBRARY		= -L $(CLIBRARYD) -l $(CLIBRARYD)

COMPILE		= $(CC) $(INCLUDE)
LINK		= $(COMPILE) $(LIBRARY)



.PHONY:		all debug test setdebug
all:		$(CLIBRARYF) $(NAME)
debug:		fclean setdebug $(CLIBRARYF) $(NAME)
setdebug:
			$(eval OPTION = -g)

$(NAME):	$(OBJS)
			$(LINK) $(OPTION) -o $@ $^

$(CLIBRARYF):
			if [[ ! -e $(CLIBRARYD) ]]; then git clone https://github.com/Liemani/cLibrary.git; fi
			make -C $(CLIBRARYD)

%.o:		%.c
			$(COMPILE) $(OPTION) -o $@ -c $<

.PHONY:		clean fclean re
clean:
			rm -f $(OBJS)
			make -C $(CLIBRARYD) fclean
fclean:		clean
			rm -f $(NAME)
			rm -rf $(CLIBRARYD)
re:			fclean all
