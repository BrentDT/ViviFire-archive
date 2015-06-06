/*----------------------------------------------------------------------
ViviFire Programming Language
Copyright 2012-2015, Brent D. Thorn

You can get the latest version at the Bay Six Software website at
http://www.b6sw.com/

To the extent possible under law, the author(s) have dedicated all 
copyright and related and neighboring rights to this software to the 
public domain worldwide. This software is distributed without any warranty. 
You should have received a copy of the CC0 Public Domain Dedication along 
with this software. 
If not, please consult the website at
http://creativecommons.org/publicdomain/zero/1.0/legalcode
----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
This file must be touched before every build due to use of __DATE__.
----------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include "Args.h"

extern Args args;

bool Args::Parse (int argc, char *argv[]) {
// Returns true if the command line args are valid.
// Otherwise, prints an error msg and returns false.
	bool show_help = false;
	if (argc >= 2) {
		for (int i = 1; i < argc; i++) {
			// Look for args starting with dash or slash.
			if (argv[i][0] == '-' || argv[i][0] == '/') {
				switch (tolower(argv[i][1])) {
				case '?':
					show_help = true;
					break;
				case 'm':
					m = true;
					break;
				case 'v':
					switch (argv[i][2]) {
					case '0': case '1': case '2': case '3':
						v = argv[i][2] - '0';
						break;
					case '\0':
						v = default_v;
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
				// Doesn't look like a switch, assume it's a filename.
				file = argv[i];
			} else {
				puts("Cannot specify more than one file.");
				return false;
			} // if
		} // for
	}
	// If not enough args or show_help is true, display syntax.
	if (argc < 2 || show_help) {
		puts(
		"ViviFire Test Parser by Brent D. Thorn, " __DATE__ "\n"
		"Syntax:\n"
		"\tVF [-m] [-v[0-3]] file\n"
		"Switches:\n"
		"\t-m   Display memory usage\n"
		"\t-v0  Minimal verbosity; Errors only\n"
		"\t-v1  Displays tokens and errors (default)\n"
		"\t-v2  Displays expressions, tokens, and errors\n"
		"\t-v3  Maximum verbosity"
		);
		return false;
	}
	// Just avout done, but make sure we've got a file.
	if (file == NULL) {
		puts("No file specified.");
		return false;
	}
	return true;
}
