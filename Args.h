/*----------------------------------------------------------------------
ViviFire Programming Language
Copyright 2012-2014, Brent D. Thorn

You can get the latest version at the Bay Six Software website at
http://www.b6sw.com/

This program is free software; you can redistribute it and/or modify it 
in any way you want under the terms of Creative Commons CC0.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 

For more information on the lisence, consult the website at
http://creativecommons.org/publicdomain/zero/1.0/legalcode
----------------------------------------------------------------------*/

#if !defined(_ARGS_H_)
#define _ARGS_H_

#include <ctype.h>
#include <stdio.h>

struct Args {
	bool m;
	int v;
	char *file;
	
	Args(): m(false), v(2), file(NULL) {}
	
	bool Parse (int argc, char *argv[]) {
	// Returns true if the command line args are valid.
	// Otherwise, prints an error msg and returns false.
		if (argc >= 2) {
			for (int i = 1; i < argc; i++) {
				if (argv[i][0] == '-') {
					switch (tolower(argv[i][1])) {
					case 'm':
						m = true;
						break;
					case 'v':
						switch (argv[i][2]) {
						case '0': case '1': case '2': case '3':
							v = argv[i][2] - '0';
							break;
						case '\0':
							v = 2;
							break;
						default:
							printf("Invalid switch: %s\n", argv[i]);
							return false;
						}
						break;
					default:
						printf("Invalid switch: %s\n", argv[i]);
						return false;
					} // switch
				} else if (file == NULL) {
					file = argv[i];
				} else {
					puts("Cannot specify more than one file.");
					return false;
				} // if
			} // for
		} else {
			puts(
			"ViviFire Test Parser by Brent D. Thorn, " __DATE__ "\n"
			"Syntax:\n"
			"\tVF [-m] [-v[0-3]] file\n"
			"Switches:\n"
			"\t-m   Display memory usage\n"
			"\t-v0  Minimal verbosity; Errors only\n"
			"\t-v1  Displays tokens and errors\n"
			"\t-v2  Displays expressions, tokens, and errors (default)\n"
			"\t-v3  Maximum verbosity"
			);
			return false;
		}
		if (file == NULL) {
			puts("No file specified.");
			return false;
		}
		return true;
	}
};
extern Args args;

#endif
