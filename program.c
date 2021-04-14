#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

#include "cLibrary.h"
#include "get_next_line.h"

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

static size_t	predictSize(char *string)
{
	char	*positionPrevious;
	char	*positionCurrent;
	char	ch;
	size_t	size;

	positionPrevious = string;
	positionCurrent = string;
	size = 0;

	while ((ch = *positionCurrent))
	{
		if (ch == '\\')
		{
			size += positionCurrent - positionPrevious;

			ch = *(++positionCurrent);
			switch (ch)
			{
				case '(':
					positionPrevious = ++positionCurrent;
					positionCurrent = strchr(positionCurrent, ')');
					if (positionCurrent == NULL)
						positionCurrent = strchr(positionPrevious, '\0');
					if (*positionCurrent == '\0')
					{
						printf(WRONG_TEXT_FORMAT);
						size += positionCurrent - positionPrevious + 2;

						return (size + 1);
					}
					*positionCurrent = '\0';
					const t_string *value = dictionarySubscript(dictionary, positionPrevious);
					if (value != NULL)
						size += value->length;
					else
						printf(WRONG_TEXT_VARIABLE);
					*positionCurrent = ')';
					++positionCurrent;
					positionPrevious = positionCurrent;
					break;
				default:
					positionPrevious = positionCurrent - 1;
					break;
			}
		}
		else
			++positionCurrent;
	}
	size += positionCurrent - positionPrevious;

	return (size + 1);
}

static void		applyVariable(char *line)
{
	char	*positionPrevious;
	char	*positionCurrent;
	char	ch;
	char	*stringToPrint;
	char	*ptr;

	positionPrevious = line;
	positionCurrent = line;
	const size_t sizeToPrint = predictSize(line);
	_lmtAlloc((void **)&stringToPrint, sizeToPrint);
	ptr = stringToPrint;

	while ((ch = *positionCurrent))
	{
		if (ch == '\\')
		{
			const size_t currentSize = positionCurrent - positionPrevious;
			memcpy(ptr, positionPrevious, currentSize);
			ptr += currentSize;

			ch = *(++positionCurrent);
			switch (ch)
			{
				case '(':
					positionPrevious = ++positionCurrent;
					positionCurrent = strchr(positionCurrent, ')');
					if (positionCurrent == NULL)
						positionCurrent = strchr(positionPrevious, '\0');
					if (*positionCurrent == '\0')
					{
						positionPrevious = positionPrevious - 2;
						break;
					}
					*positionCurrent = '\0';
					const t_string *value = dictionarySubscript(dictionary, positionPrevious);
					if (value != NULL)
					{
						memcpy(ptr, value->value, value->length);
						ptr += value->length;
					}
					*positionCurrent = ')';
					++positionCurrent;
					positionPrevious = positionCurrent;
					break;
				default:
					positionPrevious = positionCurrent - 1;
					break;
			}
		}
		else
			++positionCurrent;
	}
	const size_t currentSize = positionCurrent - positionPrevious;
	memcpy(ptr, positionPrevious, currentSize);
	ptr[currentSize] = '\n';

	ssize_t	result = write(g_output_fd, stringToPrint, sizeToPrint);
	if (result < 0)
		printf(FAIL_WRITE);
	free(stringToPrint);
	free(line);
}

static void		process()
{
	char	*line;
	int		return_value;

	dictionary = Dictionary->new();

	while (0 < (return_value = get_next_line(g_input_fd, &line))
			&& strcmp(line, ""))
		updateVariable(line);

	while (0 < (return_value = get_next_line(g_input_fd, &line)))
		applyVariable(line);
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

int				program(int argc, char **argv)
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
			break;
	}

	process();

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
