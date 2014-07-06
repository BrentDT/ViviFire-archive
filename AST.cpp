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

#include "AST.h"
#include "ASTVisitor.h"

namespace AST {

UnaryOp::UnaryOp(int o, Expression *e) : op(o), expr(e) {
}

void UnaryOp::Visit(Visitor *vtr) {
}

BinaryOp::BinaryOp(int o, Expression *l, Expression *r) : op(o), lhs(l), rhs(r) {
}

void BinaryOp::Visit(Visitor *vtr) {
}

ForEach::ForEach(Variable *k, Expression *c, StatementList *s, AfterwardOtherwise *a) : key(k), coll(c), stmt(s), after(a) {
}

void ForEach::Visit(Visitor *vtr) {
}

While::While(Expression *e, StatementList *s, AfterwardOtherwise *a) : expr(e), stmt(s), after(a) {
}

void While::Visit(Visitor *vtr) {
}

} // namespace AST
