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

Context::Context(int k): kind(k) {
	ctx[kind]++;
}

Context::~Context() {
	ctx[kind]--;
}

bool OutOfContext(int kind) {
	std::map<int, int>::const_iterator it;
	it = ctx.find(kind);
	return (it == ctx.end() || it->second == 0);
}

