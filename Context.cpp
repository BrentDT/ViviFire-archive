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

#include "Context.h"
#include <map>

Context::Context(int k, int *d): kind(k), deps(d) {
	ctx[kind]++;
}

Context::~Context() {
	ctx[kind]--;
	if (deps) {
		for (int *p = deps; *p; p++) {
			if (*p < 0) ctx[-*p]--;
		}
	}
}

void Context::operator()(int k) {
	if (deps) {
		for (int *p = deps; *p; p++) {
			if (k == *p) {
				ctx[k]++;
				*p = -*p;
			}
		}
	}
}

bool OutOfContext(int kind) {
	std::map<int, int>::const_iterator it;
	it = ctx.find(kind);
	return (it == ctx.end() || it->second == 0);
}

