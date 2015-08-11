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

#if !defined(_AST_H_)
#define _AST_H_

#include <list>

namespace AST {

class Visitor; // forward declaration

struct Node {
	virtual void Accept(Visitor *) = 0;
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
	virtual void Accept(Visitor *);
protected:
	Symbol() {}
};

struct Expression : public Node {
	virtual void Accept(Visitor *);
};
typedef std::list<Expression *> ExpressionList;

enum op_type {
	opCAT, opADD, opSUB, opMUL, opDIV, opMOD, opREM, opPOW, opLSS, opLEQ, opGTR, opGEQ, opEQU, opNEQ, opNOT, opAND, opAT, opOR, opOE, opXOR, opSHL, opSHR, opIN, opIS,
	opClnEq, opCatEq, opAddEq, opSubEq, opMulEq, opDivEq, opModEq, opRemEq, opPowEq, opShlEq, opShrEq, opAndEq, opOrEq, opXorEq
};

struct UnaryOp : public Expression {
	op_type op;
	Expression *expr;
	UnaryOp(op_type o, Expression *e): op(o), expr(e) {}
	virtual void Accept(Visitor *);
protected:
	UnaryOp() {}
};

struct BinaryOp : public Expression {
	op_type op;
	Expression *lhs, *rhs;
	BinaryOp(Expression *l, op_type o, Expression *r): lhs(l), op(o), rhs(r) {}
	virtual void Accept(Visitor *);
protected:
	BinaryOp() {}
};

struct ComparisonOp : public BinaryOp {
	// FIXME: doesn't allow chaining (e.g. x < y < z)
	double tol;
	ComparisonOp(Expression *l, op_type o, Expression *r, double t): BinaryOp(l, o, r), tol(t) {}
	virtual void Accept(Visitor *);
protected:
	ComparisonOp();
};

struct IfOp : public Expression {
	Expression *cond, *tval, *fval;
	IfOp(Expression *c, Expression *t, Expression *f): cond(c), tval(t), fval(f) {}
	virtual void Accept(Visitor *);
protected:
	IfOp() {}
};

struct Statement : public Node {
	virtual void Accept(Visitor *);
};
typedef std::list<Statement *> StatementList;

struct Block : public Statement {
	StatementList *stmts;
	
	Block(StatementList *sl): stmts(sl) {}
	virtual void Accept(Visitor *);
protected:
	Block() {}
};

struct Assignment : public Statement {
	Symbol *var;
	Expression *expr;
	Assignment(Symbol*, Expression*);
	virtual void Accept(Visitor *);
protected:
	Assignment() {}
};

struct ElseIf : public Statement {
	Expression *cond;
	StatementList *stmt;
	ElseIf(Expression *c, StatementList *sl): cond(c), stmt(sl) {}
	virtual void Accept(Visitor *);
protected:
	ElseIf() {}
};
typedef std::list<ElseIf *> ElseIfList;

struct If : public Statement {
	Expression *cond;
	StatementList *thens;
	ElseIfList *elifs;
	StatementList *elses;
	If(Expression *c, StatementList *t, ElseIfList *ei, StatementList *e): cond(c), thens(t), elifs(ei), elses(e) {}
	virtual void Accept(Visitor *);
protected:
	If() {}
};

struct CaseExpression : public Node {
	Expression *expr;
	CaseExpression(Expression *e): expr(e) {}
	virtual void Accept(Visitor *);
protected:
	CaseExpression();
};
typedef std::list<CaseExpression *> CaseExprList;

struct CaseIs : public CaseExpression {
	op_type op;
	CaseIs(op_type o, Expression *e): CaseExpression(e), op(o) {}
	virtual void Accept(Visitor *);
protected:
	CaseIs();
};

struct CaseTo : public CaseExpression {
	Expression *expr2;
	CaseTo(Expression *e1, Expression *e2): CaseExpression(e1), expr2(e2) {}
	virtual void Accept(Visitor *);
protected:
	CaseTo();
};

struct Case : public Statement {
	CaseExprList *expr;
	StatementList *stmt;
	Case(CaseExprList*, StatementList*);
	virtual void Accept(Visitor *);
protected:
	Case() {}
};
typedef std::list<Case *> CaseList;

struct Select : public Statement {
	Expression *expr;
	double tol;
	CaseList *cases;
	StatementList *elses;
	Select(Expression *e, double t, CaseList *cl, StatementList *ce): expr(e), tol(t), cases(cl), elses(ce) {}
	virtual void Accept(Visitor *);
protected:
	Select() {}
};

struct AfterwardOtherwise : public Statement {
	StatementList *after, *other;
	AfterwardOtherwise(StatementList *a, StatementList *o): after(a), other(o) {}
	virtual void Accept(Visitor *);
protected:
	AfterwardOtherwise();
};

enum do_type { doWhileLoop, doUntilLoop, doLoopWhile, doLoopUntil };

struct Do : public Statement {
	do_type type;
	Expression *expr;
	Block *block;
	AfterwardOtherwise *after;
	Do(do_type t, Expression *e, Block *b, AfterwardOtherwise *ao): type(t), expr(e), block(b), after(ao) {}
	virtual void Accept(Visitor *);
protected:
	Do() {}
};

struct For : public Statement {
	Symbol *var;
	Expression *init, *max, *step;
	Block *block;
	AfterwardOtherwise *after;
	For(Symbol *v, Expression *i, Expression *m, Expression *s, Block *b, AfterwardOtherwise *a): var(v), init(i), max(m), step(s), block(b), after(a) {}
	virtual void Accept(Visitor *);
protected:
	For() {}
};

struct ForEach : public Statement {
	Symbol *key;
	Expression *coll;
	Block *block;
	AfterwardOtherwise *after;
	ForEach(Symbol *k, Expression *c, Block *b, AfterwardOtherwise *a): key(k), coll(c), block(b), after(a) {}
	virtual void Accept(Visitor *);
protected:
	ForEach() {}
};

struct While : public Statement {
	Expression *expr;
	Block *block;
	AfterwardOtherwise *after;
	While(Expression *e, Block *b, AfterwardOtherwise *a): expr(e), block(b), after(a) {}
	virtual void Accept(Visitor *);
protected:
	While() {}
};

struct Parameter : public Symbol {
	bool byref;
	Parameter(bool r, wchar_t *n, Type *t): byref(r), Symbol(n, t) {}
protected:
	Parameter() {}
};

typedef std::list<Parameter *> ParameterList;

struct Procedure : public Symbol {
	ParameterList *params;
	Block *block;
};

class Visitor {
public:
	virtual void Visit(AfterwardOtherwise *) = 0;
	virtual void Visit(Assignment *) = 0;
	virtual void Visit(BinaryOp *) = 0;
	virtual void Visit(Block *) = 0;
	virtual void Visit(Case *) = 0;
	virtual void Visit(CaseExpression *) = 0;
	virtual void Visit(CaseIs *) = 0;
	virtual void Visit(CaseTo *) = 0;
	virtual void Visit(ComparisonOp *) = 0;
	virtual void Visit(Do *) = 0;
	virtual void Visit(ElseIf *) = 0;
	virtual void Visit(Expression *) = 0;
	virtual void Visit(For *) = 0;
	virtual void Visit(ForEach *) = 0;
	virtual void Visit(If *) = 0;
	virtual void Visit(IfOp *) = 0;
	virtual void Visit(Select *) = 0;
	virtual void Visit(Statement *) = 0;
	virtual void Visit(Symbol *) = 0;
	virtual void Visit(UnaryOp *) = 0;
	virtual void Visit(While *) = 0;
};

} // namespace AST

#endif // _AST_H_
