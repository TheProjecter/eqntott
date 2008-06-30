/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

#include <stdlib.h>

/*
 * setup process's standard input to read from a succession
 * of C preprocessor invocations, one for each source file.
 */
#define CPREPROCESSOR "/lib/cpp"

extern char *Argv[], *files[];
extern int Argc, nfiles;


int
preprocess()
{
	int p[2], i;

	if (0 > pipe (p))
		return (1);
	if (fork())
		/* redirect parent's standard input */
		return (
			0 > close (0) ||
			0 > dup (p[0]) ||
			0 > close (p[1]) ||
			0 > close (p[0])
		);

	/*
	 * You are now in the child process, who's job it is to 
	 * repeatedly fork/exec the C preprocessor.
	 * First redirect the child's standard output to the
	 * the pipe to the parent process.
	 */

	if (
		0 > close (1) ||
		0 > dup (p[1]) ||
		0 > close (p[0]) ||
		0 > close (p[1])
	)
		exit (1);

	/*
	 * Argv must be set up for C preprocessor invocatons as follows:
	 * Argv[0] = CPROPRECESSOR : C preprocessor path name
	 * Argv[1] through Argv[Argc-1] = -D, -I, -U options taken
	 *	from the command line by procargs.
	 * Argv[Argc] = one of the "nfiles" file names in "files"
	 *	taken from the command line by procargs.
	 * Argv[Argc+1] = (null) : marks end of argument list
	 */

	Argv[0] = CPREPROCESSOR;
	Argv[Argc+1] = (char *) 0;

	for (i = 0; i < nfiles; i++) {
		Argv[Argc] = files[i];
		if (fork())
			wait ((int *)0);
		else	{
			execv (Argv[0], Argv);
			exit (1);
		}
	}
	exit (0);
}
