#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "t_list.h"

//	stdio.h
//		printf()
//
//	fcntl.h
//		open()
//
//	string.h
//		strcmp()
//
//	unistd.h
//		close()
//		read()
//		write()
//
//	sys/types.h
//		ssize_t



#define WRONG_INPUT 1
#define FAIL_OPEN	2
#define FAIL_WRITE	3



int		g_input_fd;
int		g_output_fd;
t_list	g_lists;



int	process(int argc, char **argv)
{
	if (!(2 <= argc))
		return (WRONG_INPUT);

	for (int index = 1; index < argc; ++index)
	{
		if (!strcmp(argv[index], "-o"))
		{
			if (!(++index < argc)
					|| !(g_output_fd == 0))
				return (WRONG_INPUT);

			g_output_fd = open(argv[index], O_WRONLY | O_CREAT);
			if (g_output_fd < 0)
				return (FAIL_OPEN);
		}
		else
		{
			if (!(g_input_fd == 0))
				return (WRONG_INPUT);

			g_input_fd = open(argv[index], O_RDONLY);
			if (g_input_fd < 0)
				return (FAIL_OPEN);
		}
	}

	if (!(0 < g_input_fd)
			|| !(0 < g_output_fd))
		return (WRONG_INPUT);

	ssize_t	result = write(g_output_fd, "hi\n", 3);
	if (result < 0)
		return (FAIL_WRITE);

	return (0);
}

int	main(int argc, char **argv)
{
	int	result_value;

	result_value = process(argc, argv);
	switch (result_value)
	{
		case WRONG_INPUT:
			printf("You entered wrong input! Check again inputs! \n");
			printf("variabledText [-o target_file] source_file \n");
			break;
		case FAIL_OPEN:
			printf("Failed openning file \n");
			break;
		case FAIL_WRITE:
			printf("Failed writing to file \n");
			break;
		default:
			break;
	}

	if (0 < g_input_fd)
	{
		close(g_input_fd);
		g_input_fd = 0;
	}
	if (0 < g_output_fd)
	{
		close(g_output_fd);
		g_output_fd = 0;
	}

	return (0);
}
