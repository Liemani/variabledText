#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "get_next_line.h"

/*
**	stdlib.h
**		malloc(), free(), NULL, size_t
**
**	unistd.h
**		read()
**
**	sys/types.h
**		ssize_t
*/

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*ptr;

	if (dst == src || !n)
		return (dst);
	ptr = dst;
	while (n--)
		*ptr++ = *(const unsigned char *)src++;
	return (dst);
}

/*
**	return value:
**		-1: AN_ERROR_HAPPENED
**		0: Success making new element
*/

int		list_addnew_front(t_buf_list **list, int fd, t_buf_list **p_ele)
{
	t_buf_list	*ele_new;
	char		*buf;
	ssize_t		bytes_read;

	if (!(buf = malloc(sizeof(*buf) * BUFFER_SIZE)))
		return (AN_ERROR_HAPPENED);
	if ((bytes_read = read(fd, buf, BUFFER_SIZE)) < 0)
	{
		free(buf);
		return (AN_ERROR_HAPPENED);
	}
	if (!(ele_new = malloc(sizeof(*ele_new))))
	{
		free(buf);
		return (AN_ERROR_HAPPENED);
	}
	ele_new->buf = buf;
	ele_new->fd = fd;
	ele_new->bytes_read = bytes_read;
	ele_new->pos_cur = buf;
	ele_new->next = *list ? *list : NULL;
	*list = ele_new;
	*p_ele = ele_new;
	return (0);
}

void	free_ele(t_buf_list **list, t_buf_list *ele)
{
	t_buf_list	*prev_ele;

	if (*list == ele)
		*list = ele->next;
	else
	{
		prev_ele = *list;
		while (prev_ele->next != ele)
			prev_ele = prev_ele->next;
		prev_ele->next = ele->next;
	}
	free(ele->buf);
	free(ele);
}

int		free_list(t_buf_list **list)
{
	t_buf_list	*ele_cur;
	t_buf_list	*ele_next;

	ele_cur = *list;
	while (ele_cur)
	{
		ele_next = ele_cur->next;
		free(ele_cur->buf);
		free(ele_cur);
		ele_cur = ele_next;
	}
	*list = NULL;
	return (AN_ERROR_HAPPENED);
}
