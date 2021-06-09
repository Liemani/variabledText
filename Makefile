NAME		= variabledText.out
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

RM			= rm -rf



.PHONY:		all debug setdebug
all:		$(CLIBRARYF) $(NAME)
debug:		clean setdebug $(CLIBRARYF) $(NAME)
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
			$(RM) $(OBJS)
fclean:		clean
			$(RM) $(NAME)
			$(RM) $(CLIBRARYD)
re:			fclean all
