#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "get_next_line.h"

/*
**	stdlib.h
**		malloc(), free(), size_t
**
**	unistd.h
**		read()
**
**	sys/types.h
**		ssize_t
*/

/*
**	return value:
**		-1: AN_ERROR_HAPPENED
**		0: read more
**		1: A_LINE_HAS_BEEN_READ
**		2: EOF_HAS_BEEN_REACHED
*/

static int		append_line(t_buf_list *ele, char **p_line_gnl, size_t *p_len)
{
	const char	*pos_end = ele->buf + (ele->bytes_read - 1);
	char		*pos_cur;
	char		*pos_prev;
	size_t		add_len;
	char		*line_tmp;

	pos_cur = ele->pos_cur;
	if (!(pos_cur <= pos_end))
		return (!ele->bytes_read << 1);
	pos_prev = pos_cur;
	while (pos_cur != pos_end && *pos_cur != '\n')
		++pos_cur;
	ele->pos_cur = pos_cur + 1;
	add_len = pos_cur - pos_prev + (*pos_cur != '\n');
	line_tmp = *p_line_gnl;
	if (!(*p_line_gnl = malloc(sizeof(char) * (*p_len + add_len + 1))))
		return (AN_ERROR_HAPPENED);
	ft_memcpy(*p_line_gnl, line_tmp, *p_len);
	free(line_tmp);
	ft_memcpy(*p_line_gnl + *p_len, pos_prev, add_len);
	*p_len += add_len;
	if (*pos_cur == '\n')
		return (A_LINE_HAS_BEEN_READ);
	return ((ele->bytes_read != BUFFER_SIZE) << 1);
}

static int		main_loop(t_buf_list *ele, char **line)
{
	char	*line_gnl;
	size_t	len;
	ssize_t	return_value;

	if (!(line_gnl = malloc(sizeof(*line_gnl))))
		return (AN_ERROR_HAPPENED);
	len = 0;
	while (!(return_value = append_line(ele, &line_gnl, &len)))
	{
		if ((return_value = read(ele->fd, ele->buf, BUFFER_SIZE)) <= 0)
			break ;
		ele->bytes_read = return_value;
		ele->pos_cur = ele->buf;
	}
	if (return_value == AN_ERROR_HAPPENED)
		return (AN_ERROR_HAPPENED);
	*(line_gnl + len) = '\0';
	*line = line_gnl;
	return (return_value == 1 ? A_LINE_HAS_BEEN_READ : EOF_HAS_BEEN_REACHED);
}

int				get_next_line(int fd, char **line)
{
	static t_buf_list	*list;
	t_buf_list			*ele;
	int					return_value;

	if (fd < 0 || !line || BUFFER_SIZE < 1)
		return (AN_ERROR_HAPPENED);
	ele = list;
	while (ele && ele->fd != fd)
		ele = ele->next;
	if (!ele)
		if (list_addnew_front(&list, fd, &ele))
			return (free_list(&list));
	return_value = main_loop(ele, line);
	if (return_value < 0)
		return (free_list(&list));
	if (!return_value)
		free_ele(&list, ele);
	return (return_value);
}
