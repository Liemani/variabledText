NAME		= variabledText
CLIBRARYD	= cLibrary
CLIBRARYF	= $(CLIBRARYD)/libcLibrary.a

SRCS		= main.c
OBJS		= $(SRCS:.c=.o)



CC			= clang
LIBRARY		= -L $(CLIBRARYD) -l$(patsubst $(CLIBRARYD)/lib%.a,%,$(CLIBRARYF))
COMPILE		= $(CC) $(LIBRARY)



.PHONY:		all debug setdebug
all:		$(NAME)
debug:		setdebug $(NAME)
setdebug:
			$(eval OPTION = -g)

$(NAME):	$(OBJS) $(CLIBRARYF)
			$(COMPILE) $(OPTION) -o $@ $^

$(CLIBRARYF):
			if [[ ! -e $(CLIBRARYD) ]]; then
				git clone https://github.com/Liemani/cLibrary.git
			fi
			make -C $(CLIBRARYD)

%.o:		%.c
			$(COMPILE) $(OPTION) -o $@ -c $<

.PHONY:		clean fclean re
clean:
			rm -f $(OBJS)
			make -C $(CLIBRARYD) fclean
fclean:		clean
			rm -f $(NAME)
re:			fclean all
