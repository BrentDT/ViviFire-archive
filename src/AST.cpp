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

#include "AST.h"
#include "ASTVisitor.h"

namespace AST {

void Block::add(Statement *s) {
	if (stmts == NULL) stmts = new list<Statement *>();
	stmts->push_back(s);
}

void UnaryOp::Visit(Visitor *vtr) {
}

void BinaryOp::Visit(Visitor *vtr) {
}

void ForEach::Visit(Visitor *vtr) {
}

void While::Visit(Visitor *vtr) {
}

} // namespace AST
