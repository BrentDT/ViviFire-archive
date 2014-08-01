/*----------------------------------------------------------------------
ViviFire Programming Language
Copyright 2012-2014, Brent D. Thorn

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

