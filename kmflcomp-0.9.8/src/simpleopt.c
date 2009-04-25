

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "simpleopt.h"

int getopt (int ___argc, char *const *___argv, const char *__shortopts)	
{
	static int i = 1;
	unsigned int j = 0;
	if (i < ___argc)
	{
		if ((___argv[i])[0] == '-' && (strlen(___argv[i]) > 1))
		{
			for (j = 0; j < strlen(__shortopts); j++)
			{
				if (__shortopts[j] == (___argv[i])[1])
				{
					++i;
					return __shortopts[j];
				}
			}
		}
		++i;
	}
	return EOF;
}
