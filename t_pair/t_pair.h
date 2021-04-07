#ifndef T_PAIR_H
#define T_PAIR_H



typedef struct		s_pair
{
	char			*key;
	char			*value;
}					t_pair



t_pair	*newPair(char *key, char *value);
void	freePair(t_pair **pair);



#endif
