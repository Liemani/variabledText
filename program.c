#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

#include "cLibrary.h"
#include "get_next_line.h"



#define WRONG_INPUT 			1
#define FAIL_OPEN				2

#define OUTPUT_EXTENSION		".lmtdestination"

#ifndef	READ_BUFFER_SIZE
#define READ_BUFFER_SIZE		128
#endif

#define WRONG_VARIABLE_FORMAT	"Wrong variable format detected! \n"
#define WRONG_TEXT_FORMAT		"Wrong text format detected! \n"
#define WRONG_TEXT_VARIABLE		"Wrong text variabled detected! \n"
#define FAIL_WRITE				"Failed writing to file! \n"



int				g_input_fd;
int				g_output_fd;

t_dictionary	*dictionary;



static void		updateVariable(char *line)
{
	char	*keyStart = line;
	char	*keyEnd = strchr(line, '=');

	if (keyEnd == NULL)
	{
		printf(WRONG_VARIABLE_FORMAT);
		free(line);
		return ;
	}

	char	*valueStart = keyEnd + 1;
	char	*valueEnd = strchr(valueStart, '\0');

	while (isspace(*keyStart))
		++keyStart;
	if (*keyStart == '=' || *keyStart == '\0')
	{
		printf(WRONG_VARIABLE_FORMAT);
		free(line);
		return ;
	}
	--keyEnd;
	while (isspace(*keyEnd))
		--keyEnd;
	*(keyEnd + 1) = '\0';
	if (strchr(keyStart, ')') != NULL)
	{
		printf(WRONG_VARIABLE_FORMAT);
		free(line);
		return ;
	}
	t_string *key = initString(keyStart);

	while (isspace(*valueStart))
		++valueStart;
	if (*valueStart != '\0')
	{
		--valueEnd;
		while (isspace(*valueEnd))
			--valueEnd;
		*(valueEnd + 1) = '\0';
	}
	t_string *value = initString(valueStart);

	dictionaryUpdate(dictionary, key, value);

	free(line);
}

typedef struct	s_position
{
	char		*save;
	char		*current;
	char		*search;
}				t_position;

typedef void	(*workType)(t_position *position, void *memo);

static void		stringWork(char *string, void (*work)(t_position *position, void *memo), void *memo)
{
	t_position	*position;
	char		ch;

	lmtAlloc(position);
	position->save = string;
	position->current = string;
	while ((ch = *position->current) != '\0')
	{
		if (ch == '\\')
		{
			ch = *(++position->current);
			switch (ch)
			{
				case '(':
					position->search = ++position->current;
					position->search = strchr(position->search, ')');
					if (position->search == NULL)
					{
						position->search = strchr(position->current, '\0');
						printf(WRONG_TEXT_FORMAT);
					}
					work(position, memo);
					break;
				default:
					++position->current;
					break;
			}
		}
		else
			++position->current;
	}
	position->search = position->current;
	work(position, memo);
}

static void		predictLengthWork(t_position *position, size_t *length)
{
	if (*position->search == '\0')
	{
		*length += position->search - position->save;
		return ;
	}

	*length += position->current - position->save - 2;
	*position->search = '\0';
	const t_string *value = dictionarySubscript(dictionary, position->current);
	*position->search = ')';
	if (value != NULL)
		*length += value->length;
	else
		printf(WRONG_TEXT_VARIABLE);
	position->current = position->search + 1;
	position->save = position->current;
}

static void		applyVariableWork(t_position *position, char **pointer)
{
	if (*position->search == '\0')
	{
		const size_t length = position->search - position->save;
		memcpy(*pointer, position->save, length);
		(*pointer)[length] = '\n';
		return ;
	}

	const size_t length = position->current - position->save - 2;
	memcpy(*pointer, position->save, length);
	*pointer += length;
	*position->search = '\0';
	const t_string *value = dictionarySubscript(dictionary, position->current);
	*position->search = ')';
	if (value != NULL)
	{
		memcpy(*pointer, value->value, value->length);
		*pointer += value->length;
	}
	else
		printf(WRONG_TEXT_VARIABLE);
	position->current = position->search + 1;
	position->save = position->current;
}

static void		process()
{
	char	*line;
	int		return_value;
	size_t	length;
	char	*stringToPrint;
	char	*pointer;

	dictionary = Dictionary->new();

	while (0 < (return_value = get_next_line(g_input_fd, &line))
			&& strcmp(line, "") != 0)
	{
		updateVariable(line);
		line = NULL;
	}

	if (line != NULL)
		free(line);

	while (0 < (return_value = get_next_line(g_input_fd, &line)))
	{
		length = 0;
		stringWork(line, (workType)predictLengthWork, &length);
		const size_t size = length + 1;
		_lmtAlloc((void **)&stringToPrint, size);
		pointer = stringToPrint;
		stringWork(line, (workType)applyVariableWork, &pointer);
		free(line);
		const ssize_t result = write(g_output_fd, stringToPrint, size);
		if (result < 0)
			printf(FAIL_WRITE);
		free(stringToPrint);
	}
}

static char		*getDefaultOutputName(char *inputName)
{
	const char		*lastPeriod = strrchr(inputName, '.');
	const size_t	nameLength = lastPeriod - inputName;

	char	*outputName;
	_lmtAlloc((void **)&outputName, nameLength);
	strncpy(outputName, inputName, nameLength + strlen(OUTPUT_EXTENSION));
	strcpy(outputName + nameLength, OUTPUT_EXTENSION);

	return (outputName);
}

static int		readArgument(int argc, char **argv)
{
	char	*inputFileName = NULL;
	char	*outputFileName = NULL;

	if (!(2 <= argc))
		return (WRONG_INPUT);

	for (int index = 1; index < argc; ++index)
	{
		if (!strcmp(argv[index], "-o"))
		{
			if (!(++index < argc)
					|| !(outputFileName == NULL))
				return (WRONG_INPUT);

			outputFileName = argv[index];
		}
		else
		{
			if (!(inputFileName == NULL))
				return (WRONG_INPUT);

			inputFileName = argv[index];
		}
	}

	if (inputFileName)
	{
		g_input_fd = open(inputFileName, O_RDONLY);
		if (g_input_fd < 0)
			return (FAIL_OPEN);
	}
	else
		return (WRONG_INPUT);

	if (outputFileName)
		g_output_fd = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
	{
		outputFileName = getDefaultOutputName(inputFileName);
		g_output_fd = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		free(outputFileName);
	}

	if (g_output_fd < 0)
		return (FAIL_OPEN);

	return (0);
}

int				variabledText(int argc, char **argv)
{
	int	result_value;

	result_value = readArgument(argc, argv);
	switch (result_value)
	{
		case WRONG_INPUT:
			printf("You entered wrong input! Check again inputs! \n");
			printf("variabledText [-o <target_file>] <source_file> \n");
			break;
		case FAIL_OPEN:
			printf("Failed opening file \n");
			break;
		default:
			process();
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

int				program(int argc, char **argv)
{
	return (variabledText(argc, argv));
}
