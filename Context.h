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

#if !defined(_CONTEXT_H__)
#define _CONTEXT_H__

#include <map>

static std::map<int, int> ctx;

class Context {
	int kind;
	int *deps;
public:
	explicit Context(int k, int *d = NULL);
	~Context();
	void operator()(int k);
};

extern bool OutOfContext(int);

#endif
