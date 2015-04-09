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

#if !defined(_AST_H_)
#define _AST_H_

#include <list>
#include "ASTVisitor.h"

using namespace std;

namespace AST {

struct Node {
	virtual void Visit(Visitor *) = 0;
};

struct Type {
};

enum primitive_type {
	typeBoolean, typeString,
	typeTiny, typeByte,
	typeShort, typeUShort,
	typeInteger, typeUInteger,
	typeInt, typeUInt,
	typeLong, typeULong,
	typeHuge, typeUHuge,
	typeSingle, typeDouble,
	typeXFP, typeQuad
};

struct PrimitiveType : public Type {
	primitive_type type;
};

struct Symbol : public Node {
	wchar_t *name;
	Type *type;
	Symbol(wchar_t *n, Type *t): name(n), type(t) {}
protected:
	Symbol() {}
};

struct Expression : public Node { };
typedef list<Expression *> ExpressionList;

enum op_type {
	opCAT, opADD, opSUB, opMUL, opDIV, opMOD, opREM, opPOW, opLSS, opLEQ, opGTR, opGEQ, opEQU, opNEQ, opNOT, opAND, opAT, opOR, opOE, opXOR, opSHL, opSHR, opIN, opIS,
	opClnEq, opCatEq, opAddEq, opSubEq, opMulEq, opDivEq, opModEq, opRemEq, opPowEq, opShlEq, opShrEq, opAndEq, opOrEq, opXorEq
};

struct UnaryOp : public Expression {
	op_type op;
	Expression *expr;
	UnaryOp(op_type o, Expression *e): op(o), expr(e) {}
	virtual void Visit(Visitor*);
protected:
	UnaryOp() {}
};

struct BinaryOp : public Expression {
	op_type op;
	Expression *lhs, *rhs;
	BinaryOp(Expression *l, op_type o, Expression *r): lhs(l), op(o), rhs(r) {}
	virtual void Visit(Visitor*);
protected:
	BinaryOp() {}
};

struct ComparisonOp : public BinaryOp {
	// FIXME: doesn't allow chaining (e.g. x < y < z)
	double tol;
	ComparisonOp(Expression *l, op_type o, Expression *r, double t): BinaryOp(l, o, r), tol(t) {}
protected:
	ComparisonOp();
};

struct IfOp : public Expression {
	Expression *cond, *tval, *fval;
	IfOp(Expression *c, Expression *t, Expression *f): cond(c), tval(t), fval(f) {}
protected:
	IfOp() {}
};

struct Statement : public Node { };
typedef list<Statement *> StatementList;

struct Block : public Statement {
	StatementList *stmts;
	
	Block(): stmts(NULL) {}
	
	void add(Statement *);
};

struct Assignment : public Statement {
	Symbol *var;
	Expression *expr;
	Assignment(Symbol*, Expression*);
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

struct CaseExpression : public Node {
	Expression *expr;
	CaseExpression(Expression *e): expr(e) {}
protected:
	CaseExpression();
};
typedef list<CaseExpression *> CaseExprList;

struct CaseIs : public CaseExpression {
	op_type op;
	CaseIs(op_type o, Expression *e): CaseExpression(e), op(o) {}
protected:
	CaseIs();
};

struct CaseTo : public CaseExpression {
	Expression *expr2;
	CaseTo(Expression *e1, Expression *e2): CaseExpression(e1), expr2(e2) {}
protected:
	CaseTo();
};

struct Case : public Statement {
	CaseExprList *expr;
	StatementList *stmt;
	Case(CaseExprList*, StatementList*);
protected:
	Case() {}
};
typedef list<Case *> CaseList;

struct Select : public Statement {
	Expression *expr;
	double tol;
	CaseList *cases;
	StatementList *caseElse;
	Select(Expression*, double, CaseList*, StatementList*);
protected:
	Select() {}
};

struct AfterwardOtherwise : public Statement {
	StatementList *after, *other;
	AfterwardOtherwise(StatementList*, StatementList*);
protected:
	AfterwardOtherwise();
};

enum do_type { doWhileLoop, doUntilLoop, doLoopWhile, doLoopUntil };

struct Do : public Statement {
	do_type type;
	Expression *expr;
	Block *block;
	AfterwardOtherwise *after;
	Do(do_type, Expression*, Block*, AfterwardOtherwise*);
protected:
	Do() {}
};

struct For : public Statement {
	Symbol *var;
	Expression *init, *max, *step;
	Block *block;
	AfterwardOtherwise *after;
	For(Symbol *v, Expression *i, Expression *m, Expression *s, Block *b, AfterwardOtherwise *a): var(v), init(i), max(m), step(s), block(b), after(a) {}
protected:
	For() {}
};

struct ForEach : public Statement {
	Symbol *key;
	Expression *coll;
	Block *block;
	AfterwardOtherwise *after;
	ForEach(Symbol *k, Expression *c, Block *b, AfterwardOtherwise *a): key(k), coll(c), block(b), after(a) {}
	virtual void Visit(Visitor*);
protected:
	ForEach() {}
};

struct While : public Statement {
	Expression *expr;
	Block *block;
	AfterwardOtherwise *after;
	While(Expression *e, Block *b, AfterwardOtherwise *a): expr(e), block(b), after(a) {}
	virtual void Visit(Visitor*);
protected:
	While() {}
};

struct Parameter : public Symbol {
	bool byref;
	Parameter(bool r, wchar_t *n, Type *t): byref(r), Symbol(n, t) {}
protected:
	Parameter() {}
};

typedef list<Parameter *> ParameterList;

struct Procedure : public Symbol {
	ParameterList *params;
	Block *block;
};

} // namespace AST

#endif // _AST_H_
