/*----------------------------------------------------------------------
ViviFire Programming Language
Copyright 2012-2016, Brent D. Thorn

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

#include <inttypes.h>
#include <list>
#include "Type.h"

template<class T>
void delete_list(std::list<T> *plist) {
/*
	while (!plist->empty()) {
		delete plist->front();
		plist->pop_front();
	}
	delete plist;
*/
}

namespace AST {

// Forward declarations
struct Assignment;
struct Begin;
struct BinaryOp;
struct Call;
struct Catch;
struct CharConst;
struct ComparisonChain;
struct ComparisonOp;
struct DTConst;
struct Do;
struct ElseIf;
struct Enum;
struct Exit;
struct Expression;
struct FPConst;
struct For;
struct ForEach;
struct Generics;
struct GotoCase;
struct If;
struct IfOp;
struct IntConst;
struct LibraryAttrib;
struct LibraryModule;
struct Loop;
struct Parameter;
struct Procedure;
struct Return;
struct Select;
struct Symbol;
struct Throw;
struct Try;
struct UnaryOp;
struct UserModule;
struct Visitor;
struct Wait;
struct While;

typedef std::list<Case *> CaseList;
typedef std::list<CaseExpression *> CaseExpressionList;
typedef std::list<Catch *> CatchList;
typedef std::list<ElseIf *> ElseIfList;
typedef std::list<Expression *> ExpressionList;
typedef std::list<LibraryAttrib> LibraryAttribList;
typedef std::list<Parameter *> ParameterList;
typedef std::list<Statement *> StatementList;

// Enumerations

enum block_type {
	DoBlock, ForBlock, FunctionBlock, MethodBlock, PropertyBlock, SubBlock, WhileBlock
};
enum do_type {
	DoWhileLoop, DoUntilLoop, DoLoopWhile, DoLoopUntil
};
enum op_type {
	ConcatOp,
	AddOp, SubOp,
	MulOp, DivOp, ModOp, RemOp,
	PowOp,
	LessOp, LessOrEqualOp, GreaterOp, GreaterOrEqualOp, EqualOp, NotEqualOp,
	NegOp, NotOp,
	AndOp, AndThenOp, OrOp, OrElseOp, XorOp,
	SHLOp, SHROp,
	InOp, IsOp
};
enum param_type {
	NormalParam, ByRefParam, OptionalParam
};

// Nodes

struct Node {
	int line, col;
	
	virtual ~Node() = 0;
	virtual void Accept(Visitor *) = 0;
	void SetLineCol(int l, int c) { line = l; col = c; }
};

struct Generics : public Node {
	
};

struct Symbol : public Node {
	wchar_t *id;
	Type::Type *type;
	
	Symbol(wchar_t *id, Type::Type *type): id(id), type(type) {}
	virtual ~Symbol() {
		if (id) delete id;
		if (type) delete type;
	}
	virtual void Accept(Visitor *);
protected:
	Symbol() {}
};

/*---------------------------------------------------------------------------*/

struct Expression : public Node {
	virtual ~Expression() = 0;
};

struct IntConst : public Expression {
	int64_t val;

	IntConst(int64_t val): val(val) {}
	IntConst(wchar_t *s);
	virtual void Accept(Visitor *);
protected:
	IntConst() {}
};

struct FPConst : public Expression {
	double val;

	FPConst(double val): val(val) {}
	FPConst(wchar_t *);
	virtual void Accept(Visitor *);
protected:
	FPConst() {}
};

struct StringConst : public Expression {
	
};

struct CharConst : public Expression {
	wchar_t val;

	CharConst(wchar_t val): val(val) {}
	CharConst(wchar_t *);
	virtual void Accept(Visitor *);
protected:
	CharConst() {}
};

struct DTConst : public Expression {
	uint64_t val;

	DTConst(uint64_t val): val(val) {}
	DTConst(wchar_t *);
	virtual void Accept(Visitor *);
protected:
	DatetimeConst() {}
};

struct UnaryOp : public Expression {
	op_type op;
	Expression *expr;
	
	UnaryOp(op_type op, Expression *expr): op(op), expr(expr) {}
	virtual ~UnaryOp() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	UnaryOp() {}
};

struct BinaryOp : public Expression {
	op_type op;
	Expression *left, *right;
	
	BinaryOp(Expression *left, op_type op, Expression *right): left(left), op(op), right(right) {}
	virtual ~BinaryOp() {
		if (left) delete left;
		if (right) delete right;
	}
	virtual void Accept(Visitor *);
protected:
	BinaryOp() {}
};

struct ComparisonOp : public BinaryOp {
	double tol;
	
	ComparisonOp(Expression *left, op_type op, Expression *right, double tol): BinaryOp(left, op, right), tol(tol) {}
	virtual ~ComparisonOp() {}
	virtual void Accept(Visitor *);
protected:
	ComparisonOp();
};

struct ComparisonChain : public Expression {
	struct link {
		op_type op;
		Expression *expr;
	};
	std::list<link> chain;
	
	virtual void Accept(Visitor *);
};

struct IfOp : public Expression {
	Expression *cond, *tval, *fval;
	
	IfOp(Expression *cond, Expression *tval, Expression *fval): cond(cond), tval(tval), fval(fval) {}
	virtual ~IfOp() {
		if (cond) delete cond;
		if (tval) delete tval;
		if (fval) delete fval;
	}
	virtual void Accept(Visitor *);
protected:
	IfOp() {}
};

/*---------------------------------------------------------------------------*/

struct Statement : public Node {
	virtual ~Statement() = 0;
///	virtual void Accept(Visitor *) = 0;
};

struct Assignment : public Statement {
	Symbol *var;
	op_type op;
	Expression *expr;
	
	Assignment(Symbol *var, op_type op, Expression *expr): var(var), op(op), expr(expr) {}
	virtual ~Assignment() {
		if (var) delete var;
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	Assignment() {}
};

struct Begin : public Statement {
	// TODO: class id
	wchar_t *id;
	ExpressionList *arguments;
	StatementList members;
	
	virtual ~Begin() {
		if (arguments) delete_list(arguments);
		for (StatementList::iterator it = members.begin(), end = members.end(); it != end; ++it) delete (*it);
	}
	virtual void Accept(Visitor *);
	
	void AddMember(Statement *s);
};

struct Call : public Statement {
	Symbol *id;
	ExpressionList *exprs;
	
	Call(Symbol *id, ExpressionList *exprs): id(id), exprs(exprs) {}
	virtual ~Call() {
		if (id) delete id;
		if (exprs) delete_list(exprs);
	}
	virtual void Accept(Visitor *);
protected:
	Call() {}
};

struct Enum : public Statement {
	struct constant {
		wchar_t *constId;
		int64_t constVal;
		
		constant(wchar_t *id, int64_t val): constVal(val) {
			constId = coco_string_create(id);
		}
		~constant() {
			coco_string_delete(constId);
		}
	};
	wchar_t *enumId;
	op_type op;  // AddOp or MulOp
	int step;
	int64_t enumVal;
	std::list<constant *> constants;
	
	Enum(wchar_t *id, op_type op, int step): enumId(id), op(op), step(step) {
		if (id) enumId = coco_string_create(id);
	}
	virtual ~Enum() {
		if (enumId) coco_string_delete(enumId);
		for (std::list<constant *>::iterator it = constants.begin(), end = constants.end(); it != end; ++it) {
			delete (*it);
		}
	}
	virtual void Accept(Visitor *);
	
	void AddConstant(wchar_t *id, int64_t val) {
		enumVal = val;
		constants.push_back(new constant(id, val);
	}
	void AddConstant(wchar_t *id) {
		if (op == AddOp) enumVal += step; else enumVal *= step;
		constants.push_back(new constant(id, val);
	}
protected:
	Enum() {}
};

struct Exit : public Statement {
	block_type block;
	
	Exit(block_type block): block(block) {}
	virtual ~Exit() {}
	virtual void Accept(Visitor *);
protected:
	Exit();
};

struct GotoCase : public Statement {
	Expression *expr; // NULL for Goto Case Else
	
	GotoCase(Expression *expr): expr(expr) {}
	virtual ~GotoCase() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	GotoCase();
};

struct Return : public Statement {
	Expression *expr;
	
	Return(Expression *expr): expr(expr) {}
	virtual ~Return() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	Return() {}
};

struct Throw : public Statement {
	Expression *expr;
	
	Throw(Expression *expr): expr(expr) {}
	virtual ~Throw() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	Throw();
};

struct Wait : public Statement {
	Wait() {}
	virtual ~Wait() {}
	virtual void Accept(Visitor *);
};

/*---------------------------------------------------------------------------*/

struct Block : public Statement {
	StatementList members;
	Block *parent_scope;
	
	Block() {}
	virtual ~Block() {
		for (StatementList::iterator it = members.begin(), end = members.end(); it != end; ++it) delete *it;
	}
	virtual void Accept(Visitor *);
	
	void AddStatement(Statement *s) {
		members.push_back(s);
	}
};

struct UserModule : public Block {
	UserModule(): Block() {}
	virtual ~UserModule() {}
	virtual void Accept(Visitor *);
};

struct LibraryAttrib {
	wchar_t *id, *value;
};

struct LibraryModule : public Block {
	wchar_t *id;
	LibraryAttribList *attribs;
	
	LibraryModule(wchar_t *id, LibraryAttribList *attribs): id(id), attribs(attribs), Block() {}
	~LibraryModule() {
		if (id) delete id;
		if (attribs) delete_list(attribs);
	}
	virtual void Accept(Visitor *);
protected:
	LibraryModule() {}
};

struct Class : public Block {
	Symbol id;
	Generics *generics;
	Class *base;
};

struct Constructor : public Procedure {
	Constructor(ParameterList *params): Procedure(NULL, params) {}
};

struct Destructor : public Procedure {
	Destructor(): Procedure(NULL, NULL) {}
};

struct ElseIf : public Block {
	Expression *cond;
	
	ElseIf(Expression *cond): cond(cond), Block() {}
	virtual ~ElseIf() {
		if (cond) delete cond;
	}
	virtual void Accept(Visitor *);
protected:
	ElseIf() {}
};

struct If : public Block {
	Expression *cond;
	ElseIfList *elifs;
	Block *elseb;
	
	If(Expression *cond, ElseIfList *elifs, Block *elseb): cond(cond), Block(), elifs(elifs), elseb(elseb) {}
	virtual ~If() {
		if (cond) delete cond;
		if (elifs) delete_list(elifs);
		if (elseb) delete elseb;
	}
	virtual void Accept(Visitor *);
	void Add(ElseIf *elif) { elifs->push_back(elif); }
protected:
	If() {}
};

struct CaseExpression : public Node {
	Expression *expr;
	
	CaseExpression(Expression *expr): expr(expr) {}
	virtual ~CaseExpression() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
protected:
	CaseExpression();
};

struct CaseIs : public CaseExpression {
	op_type op;
	
	CaseIs(op_type op, Expression *expr): op(op), CaseExpression(expr) {}
	virtual ~CaseIs() {}
	virtual void Accept(Visitor *);
protected:
	CaseIs();
};

struct CaseTo : public CaseExpression {
	Expression *max;
	
	CaseTo(Expression *min, Expression *max): CaseExpression(min), max(max) {}
	virtual ~CaseTo() {
		if (max) delete max;
	}
	virtual void Accept(Visitor *);
protected:
	CaseTo();
};

struct Case : public Block {
	CaseExpressionList exprs;
	
	Case(): Block() {}
	virtual ~Case() {
		for (CaseExpressionList::iterator it = exprs.begin(), end = exprs.end(); it != end; ++it) delete (*it);
	}
	virtual void Accept(Visitor *);

	void AddExpression(CaseExpression *e) {
		exprs.push_back(e);
	}
};

struct Select : public Statement {
	Expression *test;
	double tol;
	CaseList *cases;
	Block *elseb;
	
	Select(Expression *test, double tol, CaseList *cases, Block *elseb): test(test), tol(tol), cases(cases), elseb(elseb) {}
	virtual ~Select() {
		if (test) delete test;
		if (cases) delete_list(cases);
		if (elseb) delete elseb;
	}
	virtual void Accept(Visitor *);
protected:
	Select() {}
};

struct Catch : public Block {
	Expression *expr;

	virtual ~Catch() {
		if (expr) delete expr;
	}
	virtual void Accept(Visitor *);
};

struct Try : public Block {
	CatchList catches;
	Block *finally;

	virtual ~Try() {
		for (CatchList::iterator it = catches.begin(), end = catches.end(); it != end; ++it) delete *it;
	}
	virtual void Accept(Visitor *);
	void Add(Catch *cat) { catches.push_back(cat); }
};

/*---------------------------------------------------------------------------*/

struct Loop : public Block {
	Block *after, *other;
	
	Loop(Block *after, Block *other): Block(), after(after), other(other) {}
	virtual ~Loop() {
		if (after) delete after;
		if (other) delete other;
	}
	virtual void Accept(Visitor *);
protected:
	Loop() {}
};

struct Do : public Loop {
	do_type type;
	Expression *cond;
	
	Do(do_type type, Expression *cond, Block *after, Block *other): type(type), cond(cond), Loop(after,  other) {}
	virtual ~Do() {
		if (cond) delete cond;
	}
	virtual void Accept(Visitor *);
protected:
	Do() {}
};

struct For : public Loop {
	Symbol *var;
	Expression *init, *max, *step;
	
	For(Symbol *var, Expression *init, Expression *max, Expression *step, Block *after, Block *other): var(var), init(init), max(max), step(step), Loop(after, other) {}
	virtual ~For() {
		if (var) delete var;
		if (init) delete init;
		if (max) delete max;
		if (step) delete step;
	}
	virtual void Accept(Visitor *);
protected:
	For() {}
};

struct ForEach : public Loop {
	Symbol *var;
	Expression *coll;
	
	ForEach(Symbol *var, Expression *coll, Block *after, Block *other): var(var), coll(coll), Loop(after, other) {}
	virtual ~ForEach() {
		if (var) delete var;
		if (coll) delete coll;
	}
	virtual void Accept(Visitor *);
protected:
	ForEach() {}
};

struct While : public Loop {
	Expression *cond;
	
	While(Expression *cond, Block *after, Block *other): cond(cond), Loop(after, other) {}
	virtual ~While() {
		if (cond) delete cond;
	}
	virtual void Accept(Visitor *);
protected:
	While() {}
};

struct Parameter : public Symbol {
	param_type modifiers;
	
	Parameter(param_type mods, wchar_t *id, Type::Type *type): modifiers(mods), Symbol(id, type) {}
protected:
	Parameter() {}
};

struct Procedure : public Block {
	Symbol *id;
	Generics *generics;
	ParameterList *params;
	
	Procedure(Symbol *id, ParameterList *params): id(id), params(params), Block() {}
	virtual ~Procedure() {
		if (id) delete id;
		if (params) delete_list(params);
	}
protected:
	Procedure() {}
};

struct Visitor {
	virtual void Visit(Assignment *) = 0;
	virtual void Visit(Begin *) = 0;
	virtual void Visit(BinaryOp *) = 0;
	virtual void Visit(Block *node) {
		for (StatementList::iterator it = node->stmts.begin(), end = node->stmts.end(); it != end; ++it)
			(*it)->Accept(this);
	}
	virtual void Visit(Call *) = 0;
	virtual void Visit(Case *) = 0;
	virtual void Visit(CaseExpression *) = 0;
	virtual void Visit(CaseIs *) = 0;
	virtual void Visit(CaseTo *) = 0;
	virtual void Visit(Catch *) = 0;
	virtual void Visit(CharConst *) = 0;
	virtual void Visit(Class *) = 0;
	virtual void Visit(ComparisonChain *) = 0;
	virtual void Visit(ComparisonOp *) = 0;
	virtual void Visit(Constructor *) = 0;
	virtual void Visit(Destructor *) = 0;
	virtual void Visit(Do *) = 0;
	virtual void Visit(DTConst *) = 0;
	virtual void Visit(ElseIf *) = 0;
	virtual void Visit(Enum *) = 0;
	virtual void Visit(Exit *) = 0;
	virtual void Visit(For *) = 0;
	virtual void Visit(ForEach *) = 0;
	virtual void Visit(FPConst *) = 0;
	virtual void Visit(GotoCase *) = 0;
	virtual void Visit(If *) = 0;
	virtual void Visit(IfOp *) = 0;
	virtual void Visit(IntConst *) = 0;
	virtual void Visit(LibraryModule *) = 0;
	virtual void Visit(Return *) = 0;
	virtual void Visit(Select *) = 0;
	virtual void Visit(Symbol *) = 0;
	virtual void Visit(Throw *) = 0;
	virtual void Visit(Try *) = 0;
	virtual void Visit(UnaryOp *) = 0;
	virtual void Visit(UserModule *) = 0;
	virtual void Visit(Wait *) = 0;
	virtual void Visit(While *) = 0;
};

} // namespace AST

#endif // _AST_H_
