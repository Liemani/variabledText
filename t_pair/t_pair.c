#include <stdlib.h>
#include "t_pair.h"

//	stdlib.h
//		NULL, free()



t_pair	*newPair(char *key, char *value)
{
	t_pair	*pair;

	pair = malloc(sizeof(*pair));
	if (pair == NULL)
		return (NULL);

	pair->key = key;
	pair->value = value;
	
	return (pair);
}

void	freePair(t_pair **pair)
{
	free((*pair)->identifier);
	free((*pair)->value);
	free(*pair);
	*pair = NULL;
}
