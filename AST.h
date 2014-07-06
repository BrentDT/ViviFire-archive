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

#if !defined(_AST_H_)
#define _AST_H_

#include <list>
#include "ASTVisitor.h"

using namespace std;

namespace AST {

struct Node {
	virtual void Visit(Visitor *) = 0;
};

struct Variable : public Node {
};

struct Expression : public Node { };
typedef list<Expression *> ExpressionList;

struct UnaryOp : public Expression {
	int op; // FIXME: temporary placeholder
	Expression *expr;
	UnaryOp(int, Expression*);
	virtual void Visit(Visitor*);
protected:
	UnaryOp() {}
};

struct BinaryOp : public Expression {
	int op; // FIXME: temporary placeholder
	Expression *lhs, *rhs;
	BinaryOp(int, Expression*, Expression*);
	virtual void Visit(Visitor*);
protected:
	BinaryOp() {}
};

struct ComparisonOp : public BinaryOp {
	double tol;
	ComparisonOp(Expression*, Expression*, double);
protected:
	ComparisonOp();
};

struct Statement : public Node { };
typedef list<Statement *> StatementList;

struct Assignment : public Statement {
	Variable *var;
	Expression *expr;
	Assignment(Variable*, Expression*);
protected:
	Assignment() {}
};

struct ElseIf : public Statement {
	Expression *expr;
	StatementList *stmt;
	ElseIf(Expression*, StatementList*);
protected:
	ElseIf() {}
};
typedef list<ElseIf *> ElseIfList;

struct If : public Statement {
	Expression *expr;
	StatementList *ifThen;
	ElseIfList *elseIf;
	StatementList *ifElse;
	If(Expression*, StatementList*, StatementList*);
protected:
	If() {}
};

struct Case : public Statement {
	ExpressionList *expr;
	StatementList *stmt;
	Case(ExpressionList*, StatementList*);
protected:
	Case() {}
};
typedef list<Case *> CaseList;

struct Select : public Statement {
	Expression *expr;
	double tol;
	CaseList *cases;
	Select(Expression*, double, CaseList*);
protected:
	Select() {}
};

struct AfterwardOtherwise : public Statement {
	StatementList *after, *other;
	AfterwardOtherwise(StatementList*, StatementList*);
protected:
	AfterwardOtherwise();
};

enum DoType { DoWhileLoop, DoUntilLoop, DoLoopWhile, DoLoopUntil };

struct Do : public Statement {
	DoType type;
	Expression *expr;
	StatementList *stmt;
	AfterwardOtherwise *after;
	Do(DoType, Expression*, StatementList*, AfterwardOtherwise*);
protected:
	Do() {}
};

struct For : public Statement {
	Variable *var;
	Expression *init, *max, *step;
	StatementList *stmt;
	AfterwardOtherwise *after;
	For(Variable*, Expression*, Expression*, Expression*, StatementList*, AfterwardOtherwise*);
protected:
	For() {}
};

struct ForEach : public Statement {
	Variable *key;
	Expression *coll;
	StatementList *stmt;
	AfterwardOtherwise *after;
	ForEach(Variable*, Expression*, StatementList*, AfterwardOtherwise*);
	virtual void Visit(Visitor*);
protected:
	ForEach() {}
};

struct While : public Statement {
	Expression *expr;
	StatementList *stmt;
	AfterwardOtherwise *after;
	While(Expression*, StatementList*, AfterwardOtherwise*);
	virtual void Visit(Visitor*);
protected:
	While() {}
};

} // namespace AST

#endif // _AST_H_
