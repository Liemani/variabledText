#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <sys/types.h>

/*
**	sys/types.h
**		ssize_t
*/

# define A_LINE_HAS_BEEN_READ 1
# define EOF_HAS_BEEN_REACHED 0
# define AN_ERROR_HAPPENED -1

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

/*
**	t_buf_list
**	{
**		buf				: size of BUFFER_SIZE
**		fd				: any fd
**		bytes_read		: number of bytes actually read
**		pos_cur			: [buf, buf + bytes_read], where to read
**		next			: next element
**	}
*/

typedef struct			s_buf_list
{
	char				*buf;
	int					fd;
	ssize_t				bytes_read;
	char				*pos_cur;
	struct s_buf_list	*next;
}						t_buf_list;

int						get_next_line(int fd, char **line);
void					*ft_memcpy(void *dst, const void *src, size_t n);
int						list_addnew_front(t_buf_list **list,
											int fd, t_buf_list **ele);
void					free_ele(t_buf_list **list, t_buf_list *ele);
int						free_list(t_buf_list **list);

#endif
