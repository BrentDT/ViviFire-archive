
/*----------------------------------------------------------------------
ViviFire Programming Language
Authors:
2012-2016 Brent D. Thorn <brent@b6sw.com>

You can get the latest version at the Bay Six Software website at
http://www.b6sw.com/

To the extent possible under law, the author(s) have dedicated all 
copyright and related and neighboring rights to this software to the 
public domain worldwide. This software is distributed without any warranty. 
You should have received a copy of the CC0 Public Domain Dedication along 
with this software. 
If not, please consult the website at
http://creativecommons.org/publicdomain/zero/1.0/
----------------------------------------------------------------------*/

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"




void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		// BEGIN MOD
		if (t->kind && args.v >= 1) wprintf(L"%s%d:%s", (t->val[0] == L'@' && !iswdigit(t->val[1])) ? L"" : L"\n", t->kind, t->val[0] >= 32 ? t->val : L"???");
		// END MOD
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }
		if (la->kind == _atCONST) {
				if (la->col != 1) Err(L"Directive not at start of line"); 
		}
		if (la->kind == _atDEPRECATE) {
				printv(3, "@Deprecate"); 
		}
		if (la->kind == _atIF) {
		}
		if (la->kind == _atELSE) {
		}
		if (la->kind == _atENDIF) {
		}
		if (la->kind == _atERROR) {
				errors->Error(la->line, la->col, la->val + 7); 
		}
		if (la->kind == _atWARN) {
				Warn(la->val + 6); 
		}
		if (la->kind == _continuation) {
		}
		if (la->kind == _comment) {
		}

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::VF1() {
		printv(3, "VF1"); 
		AST::Node *m = NULL; 
		while (la->kind == _newline) {
			Get();
		}
		if (la->kind == 122 /* "library" */) {
			LibraryModule(m);
		} else if (StartOf(1)) {
			UserModule(m);
		} else if (la->kind == _EOF) {
			Warn(L"Module is empty."); 
		} else SynErr(173);
		Expect(_EOF);
		printf("\n-- %d %s\n", errors->count, (errors->count==1)?"error":"errors"); 
}

void Parser::LibraryModule(AST::Library *lib) {
		printv(3, "LibraryModule"); 
		AST::Statement *s; 
		Expect(122 /* "library" */);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,2) ) {
			LibraryAttribute();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(174); Get();}
		Newline();
		while (la->kind == _WHERE) {
			Get();
			LibraryAttribute();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(175); Get();}
			Newline();
		}
		while (la->kind == 161 /* "require" */) {
			RequireStatement(s);
		}
		LibraryModuleDeclaration(s);
		while (StartOf(4)) {
			if (StartOf(5)) {
				LibraryModuleDeclaration(s);
			} else {
				ModuleMistake();
			}
		}
}

void Parser::UserModule(AST::UserModule *um) {
		printv(3, "UserModule"); 
		AST::StatementList *stmts = new AST::StatementList(); 
		while (la->kind == 161 /* "require" */) {
			RequireStatement(s);
		}
		while (StartOf(6)) {
			UserModuleDeclaration(s);
		}
		while (StartOf(7)) {
			AST::Statement *stmt; 
			Statement(stmt);
			stmts->push_end(stmt); 
		}
		Expect(_End);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(176); Get();}
		Newline();
		while (StartOf(8)) {
			if (StartOf(6)) {
				UserModuleDeclaration(s);
			} else if (la->kind == _FUNCTION) {
				FunctionDefinition(s);
			} else if (la->kind == _SUB) {
				SubDefinition(s);
			} else {
				ModuleMistake();
			}
		}
		if (stmts->empty()) delete stmts; 
}

void Parser::AbstractClass(AST::Class *s) {
		printv(3, "AbstractClass"); 
		Expect(_ABSTRACT);
		ClassDefinition(s, _ABSTRACT);
}

void Parser::ClassDefinition(AST::Class *block, int abstract) {
		printv(3, "ClassDefinition"); 
		int elems = 0; 
		bool isGeneric = false; 
		Context *subclass = NULL; 
		Context foo(_CLASS); 
		Expect(_CLASS);
		Expect(_plainIdentifier);
		if (la->kind == _leftBracket) {
			GenericDefinition();
			isGeneric = true; 
		}
		if (la->kind == _IS) {
			Inheritance();
			subclass = new Context(_IS); 
		}
		if (la->kind == _DOES) {
			Traits();
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(177); Get();}
		Newline();
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(178); Get();}
			Newline();
		}
		if (StartOf(9)) {
			ClassMember(s, elems);
			while (StartOf(10)) {
				if (StartOf(9)) {
					ClassMember(s, elems);
				} else {
					ClassMistake();
				}
			}
		}
		Expect(_END_CLASS);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(179); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::AbstractMember(AST::Statement *s) {
		printv(3, "AbstractMember"); 
		Expect(_ABSTRACT);
		if (la->kind == _CLASS) {
			ClassDefinition(s, _ABSTRACT);
		} else if (la->kind == _METHOD) {
			bool dummy = false; 
			MethodSignature(dummy);
		} else if (la->kind == _PROPERTY) {
			PropertySignature();
		} else SynErr(180);
}

void Parser::MethodSignature(bool &isBodiless) {
		printv(3, "MethodSignature"); 
		isBodiless = false; 
		bool isGeneric = false; 
		Expect(_METHOD);
		if (la->kind == _DOES || scanner->Peek()->kind == _DOES) {
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
				DeclaredName();
			}
			Expect(_DOES);
			Expect(_plainIdentifier);
			Expect(_dot);
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
				DeclaredName();
			} else if (la->kind == _stringLiteral) {
				Get();
			} else SynErr(181);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(182); Get();}
			Newline();
			isBodiless = true; 
		} else if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
			DeclaredName();
			if (la->kind == _leftBracket) {
				GenericDefinition();
				isGeneric = true; 
			}
			if (StartOf(11)) {
				if (la->kind == _leftParen) {
					ParameterListEnclosed(p);
					if (la->kind == _IN || la->kind == 112 /* "as" */) {
						DataTypeClause();
					}
				} else if (StartOf(12)) {
					ParameterListUnenclosed(p);
				} else {
					DataTypeClause();
				}
			}
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(183); Get();}
			Newline();
		} else if (StartOf(13)) {
			if (la->kind == _leftBracket) {
				GenericDefinition();
				isGeneric = true; 
			}
			if (la->kind == _leftParen) {
				AnonMethodParameter(p);
			} else if (la->kind == _IN || la->kind == 112 /* "as" */) {
				DataTypeClause();
			} else SynErr(184);
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(185); Get();}
			Newline();
		} else SynErr(186);
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(187); Get();}
			Expect(_newline);
		}
}

void Parser::PropertySignature() {
		printv(3, "PropertySignature"); 
		bool isGeneric = true; 
		Expect(_PROPERTY);
		DeclaratorList(isGeneric);
		if (la->kind == _DOES) {
			Get();
			if (la->kind == 159 /* "get" */) {
				Get();
				if (la->kind == _comma) {
					ExpectWeak(_comma, 14);
					Expect(160 /* "set" */);
				}
			} else if (la->kind == 160 /* "set" */) {
				Get();
			} else SynErr(188);
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(189); Get();}
		Newline();
}

void Parser::AdditiveExpression(AST::Expression *e) {
		printv(2, "Additive"); 
		AST::op_type op; 
		AST::Expression *e2; 
		MultiplicativeExpression(e);
		while (la->kind == 92 /* "+" */ || la->kind == 93 /* "-" */) {
			AdditiveOp(op);
			MultiplicativeExpression(e2);
			e = new AST::BinaryOp(e, op, e2); 
		}
		if (la->kind == _IN) {
			Get();
			Unit();
		}
}

void Parser::MultiplicativeExpression(AST::Expression *e) {
		printv(2, "Multiplicative"); 
		AST::op_type op; 
		AST::Expression *e2; 
		PowerExpression(e);
		while (StartOf(15)) {
			MultiplicativeOp(op);
			PowerExpression(e2);
			e = new AST::BinaryOp(e, op, e2); 
		}
		if (la->kind == _plainIdentifier) {
			Unit();
		}
}

void Parser::AdditiveOp(AST::op_type &op) {
		if (la->kind == 92 /* "+" */) {
			Get();
			op = AST::AddOp; 
		} else if (la->kind == 93 /* "-" */) {
			Get();
			op = AST::SubOp; 
		} else SynErr(190);
}

void Parser::Unit() {
		printv(3, "Unit"); 
		Expect(_plainIdentifier);
		if (la->kind == 130 /* "/" */) {
			Get();
			Expect(_plainIdentifier);
		}
}

void Parser::AfterwardClause(AST::Block *block) {
		printv(3, "AfterwardClause"); 
		AST::Statement *stmt; 
		Expect(94 /* "afterward" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(191); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
}

void Parser::Newline() {
		printv(3, "Newline"); 
		Expect(_newline);
		while (la->kind == _newline) {
			Get();
		}
}

void Parser::Statement(AST::Statement *s) {
		printv(3, "Statement"); 
		if (StartOf(16)) {
			SimpleStatement(s);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(192); Get();}
			Newline();
		} else if (StartOf(17)) {
			CompoundStatement(s);
		} else SynErr(193);
}

void Parser::AnonMethodParameter(AST::Parameter *p) {
		printv(3, "AnonMethodParameter"); 
		opt_param opt = opt_no; 
		pass_by by = by_any; 
		Expect(_leftParen);
		Parameter(p, opt, by);
		Expect(_rightParen);
}

void Parser::Parameter(AST::Parameter *p, opt_param &opt, pass_by &by) {
		printv(3, "Parameter"); 
		if (la->kind == 102 /* "optional" */) {
			Get();
			if (opt == opt_no) Err(L"Parameter cannot be optional"); 
			else if (opt == opt_warn) Warn(L"Ignoring redundant OPTIONAL modifier"); 
			opt = opt_yes; 
		} else if (StartOf(18)) {
			opt = opt_no; 
		} else SynErr(194);
		if (la->kind == 119 /* "byref" */) {
			Get();
			if (by == by_val) Err(L"BYREF is not allowed here"); 
			by = by_ref; 
		} else if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
			if (by == by_ref) Err(L"BYREF is required here"); 
			by = by_val; 
		} else SynErr(195);
		if (la->kind == _typedIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				Get();
				Expect(_rightParen);
			}
		} else if (la->kind == _plainIdentifier) {
			Get();
			if (StartOf(19)) {
				if (la->kind == _IN || la->kind == 112 /* "as" */) {
					DataTypeClause();
				} else if (la->kind == _leftParen) {
					Get();
					Expect(_rightParen);
					if (la->kind == _IN || la->kind == 112 /* "as" */) {
						DataTypeClause();
					}
				} else if (la->kind == _plainIdentifier) {
					Get();
					if (la->kind == _leftParen) {
						Get();
						Expect(_rightParen);
					}
				} else {
					if (la->kind == _leftBracket) {
						GenericUsage();
					}
					Expect(_objectIdentifier);
					if (la->kind == _leftParen) {
						Get();
						Expect(_rightParen);
					}
				}
			}
		} else if (la->kind == _objectIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				Get();
				Expect(_rightParen);
			}
		} else SynErr(196);
}

void Parser::Argument(arg_kind &prev) {
		printv(3, "Argument"); 
		if (StartOf(20)) {
			if (prev == arg_named) Err(L"Positional arguments cannot follow named arguments"); 
			prev = arg_pos; 
			EnclosedExpression(e);
		} else if (la->kind == _leftBracket) {
			if (prev == arg_none) Warn(L"Ignoring empty arguments before named argument"); 
			prev = arg_named; 
			Get();
			VariableName();
			Expect(_rightBracket);
			if (la->kind == _equals) {
				Get();
			}
			EnclosedExpression(e);
		} else SynErr(197);
}

void Parser::EnclosedExpression(AST::Expression *e) {
		printv(2, "Enclosed"); 
		LogicalXORExpression(e);
}

void Parser::VariableName() {
		if (la->kind == _plainIdentifier) {
			printv(3, "VariableName"); 
			Get();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else if (la->kind == _objectIdentifier) {
			Get();
		} else if (la->kind == _boxedIdentifier) {
			Get();
		} else SynErr(198);
}

void Parser::ArgumentList() {
		printv(3, "ArgumentList"); 
		arg_kind prev = arg_any; 
		Argument(prev);
		while (WeakSeparator(_comma,22,21) ) {
			if (StartOf(23)) {
				Argument(prev);
			} else if (StartOf(24)) {
				if (prev == arg_named) Err(L"Positional arguments cannot follow named arguments"); 
				prev = arg_none; 
			} else SynErr(199);
		}
}

void Parser::AssignmentExpression(AST::Expression *e) {
		printv(2, "Assignment"); 
		AST::op_type op; 
		AST::Expression *e2; 
		ConditionalExpression(e);
		while (StartOf(25)) {
			AssignmentOp(op);
			ConditionalExpression(e2);
			e = new AST::Assignment(e, op, e2); 
		}
}

void Parser::ConditionalExpression(AST::Expression *e) {
		printv(2, "Conditional"); 
		AST::Expression *t, *f; 
		if (StartOf(20)) {
			EnclosedExpression(e);
		} else if (la->kind == 110 /* "if" */) {
			Get();
			EnclosedExpression(e);
			Expect(104 /* "then" */);
			EnclosedExpression(t);
			Expect(_ELSE);
			EnclosedExpression(f);
			e = new AST::IfOp(e, t, f); 
		} else SynErr(200);
}

void Parser::AssignmentOp(AST::op_type &op) {
		switch (la->kind) {
		case _assignOp: {
			Get();
			op = AST::EqualOp; 
			break;
		}
		case _concatAssign: {
			Get();
			op = AST::ConcatOp; 
			break;
		}
		case _andAssign: {
			Get();
			op = AST::AndOp; 
			break;
		}
		case _orAssign: {
			Get();
			op = AST::OrOp; 
			break;
		}
		case _xorAssign: {
			Get();
			op = AST::XorOp; 
			break;
		}
		case _shlAssign: {
			Get();
			op = AST::SHLOp; 
			break;
		}
		case _shrAssign: {
			Get();
			op = AST::SHROp; 
			break;
		}
		case _addAssign: {
			Get();
			op = AST::AddOp; 
			break;
		}
		case _subAssign: {
			Get();
			op = AST::SubOp; 
			break;
		}
		case _mulAssign: {
			Get();
			op = AST::MulOp; 
			break;
		}
		case _divAssign: {
			Get();
			op = AST::DivOp; 
			break;
		}
		case _modAssign: {
			Get();
			op = AST::ModOp; 
			break;
		}
		case _remAssign: {
			Get();
			op = AST::RemOp; 
			break;
		}
		default: SynErr(201); break;
		}
}

void Parser::AssignmentStatement(AST::Statement *s) {
		printv(3, "AssignmentStatement"); 
		Mutable(v);
		if (la->kind == _equals) {
			Get();
			Expression(e);
		} else if (la->kind == _assignOp) {
			Get();
			Expression(e);
		} else SynErr(202);
}

void Parser::Mutable(AST::Symbol *s) {
		printv(3, "Mutable"); 
		if (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			IdentifierExpression(e);
		} else if (la->kind == _objectIdentifier || la->kind == _boxedIdentifier) {
			if (la->kind == _objectIdentifier) {
				Get();
			} else {
				Get();
			}
			if (la->kind == _bang || la->kind == _leftParen) {
				if (la->kind == _leftParen) {
					Subscript();
				} else {
					Get();
					if (la->kind == _plainIdentifier) {
						Get();
					} else if (la->kind == _typedIdentifier) {
						Get();
					} else SynErr(203);
				}
			}
		} else SynErr(204);
}

void Parser::Expression(AST::Expression *e) {
		printv(3, "Expression"); 
		AssignmentExpression(e);
		if (la->kind == _rightParen) { Get(); Err(L"Mismatched parentheses"); } 
}

void Parser::BaseUnitDefinition(AST::Statement *s) {
		printv(3, "BaseUnitDefinition"); 
		AST::Expression *e = NULL; 
		Expect(95 /* "base" */);
		Expect(_UNIT);
		Expect(_plainIdentifier);
		Expect(_IN);
		Unit();
		if (la->kind == _equals) {
			Get();
			MultiplicativeExpression(e);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(205); Get();}
		Newline();
}

void Parser::BitShiftExpression(AST::Expression *e) {
		printv(2, "BitShift"); 
		AST::op_type op; 
		AST::Expression *e2; 
		ConcatenativeExpression(e);
		while (la->kind == 96 /* "shl" */ || la->kind == 97 /* "shr" */) {
			BitShiftOp(op);
			ConcatenativeExpression(e2);
			e = new AST::BinaryOp(e, op, e2); 
		}
}

void Parser::ConcatenativeExpression(AST::Expression *e) {
		printv(2, "Concatenative"); 
		AST::Expression *e2; 
		AdditiveExpression(e);
		while (la->kind == _concatOp) {
			Get();
			AdditiveExpression(e2);
			e = new AST::BinaryOp(e, AST::ConcatOp, e2); 
		}
}

void Parser::BitShiftOp(AST::op_type &op) {
		if (la->kind == 96 /* "shl" */) {
			Get();
			op = AST::SHLOp; 
		} else if (la->kind == 97 /* "shr" */) {
			Get();
			op = AST::SHROp; 
		} else SynErr(206);
}

void Parser::CallStatement(AST::Statement *s) {
		printv(3, "CallStatement"); 
		Expect(98 /* "call" */);
		DeclaredName();
		if (StartOf(23)) {
			ArgumentList();
		}
}

void Parser::DeclaredName() {
		if (la->kind == _plainIdentifier) {
			printv(3, "DeclaredName"); 
			Get();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else if (la->kind == _objectIdentifier) {
			Get();
		} else SynErr(207);
}

void Parser::CaseExpression(AST::CaseExpression *ce) {
		printv(3, "CaseExpression"); 
		AST::Expression *e, *e2; 
		AST::op_type op; 
		if (StartOf(26)) {
			Expression(e);
			if (la->kind == 99 /* "to" */) {
				Get();
				Expression(e2);
				ce = new AST::CaseTo(e, e2); 
			} else if (la->kind == _newline || la->kind == _comma) {
				ce = new AST::CaseExpression(e); 
			} else SynErr(208);
		} else if (la->kind == _IS) {
			Get();
			ComparisonOp(op);
			Expression(e);
			ce = new AST::CaseIs(op, e); 
		} else SynErr(209);
}

void Parser::ComparisonOp(AST::op_type &op) {
		switch (la->kind) {
		case 105 /* "<" */: {
			Get();
			op = AST:LessOp; 
			break;
		}
		case 106 /* "<=" */: {
			Get();
			op = AST::LessOrEqualOp; 
			break;
		}
		case _equals: {
			Get();
			op = AST::EqualOp; 
			break;
		}
		case 109 /* "<>" */: {
			Get();
			op = AST::NotEqualOp; 
			break;
		}
		case 108 /* ">=" */: {
			Get();
			op = AST:GreaterOrEqualOp; 
			break;
		}
		case 107 /* ">" */: {
			Get();
			op = AST::GreaterOp; 
			break;
		}
		default: SynErr(210); break;
		}
}

void Parser::CaseStatement(AST::Statement *block, int &bCaseElse, int &line, int &col) {
		printv(3, "CaseStatement"); 
		AST::CaseExpression *ce; 
		AST::CaseExpressionList *exprs = new AST::CaseExpressionList(); 
		AST::Statement *stmt; 
		Expect(_CASE);
		line = t->line; col = t->col; 
		if (StartOf(27)) {
			CaseExpression(ce);
			exprs->push_back(ce); 
			while (WeakSeparator(_comma,27,2) ) {
				CaseExpression(ce);
				exprs->push_back(ce); 
			}
		} else if (la->kind == _ELSE) {
			Get();
			bCaseElse = true; 
		} else SynErr(211);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(212); Get();}
		Newline();
		block = new AST::Case(exprs); 
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
}

void Parser::GenericDefinition() {
		printv(3, "GenericDefinition"); 
		Expect(_leftBracket);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,28) ) {
			Expect(_plainIdentifier);
		}
		Expect(_rightBracket);
}

void Parser::Inheritance() {
		printv(3, "Inheritance"); 
		Expect(_IS);
		ClassType();
}

void Parser::Traits() {
		printv(3, "Traits"); 
		Expect(_DOES);
		ClassType();
		while (WeakSeparator(_comma,3,29) ) {
			ClassType();
		}
}

void Parser::GenericConstraints(bool isGeneric) {
		printv(3, "GenericConstraints"); 
		Expect(_WHERE);
		if (!isGeneric) Err(L"Illegal use of WHERE in non-generic context"); 
		GenericConstraint();
		while (WeakSeparator(_comma,3,2) ) {
			GenericConstraint();
		}
}

void Parser::ClassMember(AST::Class *s, int &elems) {
		printv(3, "ClassMember"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractMember(s);
			break;
		}
		case _CLASS: {
			ClassDefinition(s, 0);
			break;
		}
		case _CONSTRUCTOR: case 111 /* "ctor" */: {
			ConstructorDefinition(s, elems);
			break;
		}
		case _DESTRUCTOR: case 113 /* "dtor" */: {
			DestructorDefinition(s, elems);
			break;
		}
		case _EVENT: {
			EventDefinition(s);
			break;
		}
		case _FUNCTION: {
			FunctionDefinition(s);
			elems |= elem_body; 
			break;
		}
		case _METHOD: {
			MethodDefinition(s);
			elems |=  elem_body; 
			break;
		}
		case 135 /* "object" */: {
			ObjectDefinition(s);
			break;
		}
		case 136 /* "override" */: {
			OverrideMember(s);
			break;
		}
		case _PROPERTY: {
			PropertyDefinition(s);
			break;
		}
		case _SHARED: {
			SharedMember(s, elems);
			break;
		}
		case _plainIdentifier: case _typedIdentifier: case _objectIdentifier: case _boxedIdentifier: case _DO: case _FOR: case _FOR_EACH: case _SELECT: case _TRY: case _WHILE: case 98 /* "call" */: case 110 /* "if" */: case 114 /* "dim" */: case 115 /* "var" */: case 117 /* "exit" */: case 121 /* "goto" */: case 133 /* "new" */: case 162 /* "return" */: case 163 /* "let" */: case 166 /* "throw" */: case 170 /* "wait" */: {
			Statement(s);
			break;
		}
		case _SUB: {
			SubDefinition(s);
			elems |= elem_body; 
			break;
		}
		default: SynErr(213); break;
		}
}

void Parser::ClassMistake() {
		printv(3, "ClassMistake"); 
		switch (la->kind) {
		case 94 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 100 /* "catch" */: {
			Get();
			Err(L"CATCH without TRY"); 
			break;
		}
		case _ELSE: {
			Get();
			Err(L"ELSE without IF"); 
			break;
		}
		case _ELSEIF: {
			Get();
			Err(L"ELSEIF without IF"); 
			break;
		}
		case _END_IF: {
			Get();
			Err(L"END IF without IF"); 
			break;
		}
		case _END_CONSTRUCTOR: {
			Get();
			Err(L"END CONSTRUCTOR without CONSTRUCTOR"); 
			break;
		}
		case _END_DESTRUCTOR: {
			Get();
			Err(L"END DESTRUCTOR without DESTRUCTOR"); 
			break;
		}
		case _END_ENUM: {
			Get();
			Err(L"END ENUM without ENUM"); 
			break;
		}
		case _END_FOR: {
			Get();
			Err(L"END FOR without FOR"); 
			break;
		}
		case _END_OBJECT: {
			Get();
			Err(L"END OBJECT without OBJECT"); 
			break;
		}
		case _EndOfInitializer: {
			Get();
			Err(L"END without BEGIN"); 
			break;
		}
		case _END_PROPERTY: {
			Get();
			Err(L"END PROPERTY without PROPERTY"); 
			break;
		}
		case _END_SELECT: {
			Get();
			Err(L"END SELECT without SELECT"); 
			break;
		}
		case _END_STRUCT: {
			Get();
			Err(L"END STRUCT without STRUCT"); 
			break;
		}
		case _END_TRAIT: {
			Get();
			Err(L"END TRAIT without TRAIT"); 
			break;
		}
		case _END_TRY: {
			Get();
			Err(L"END TRY without TRY"); 
			break;
		}
		case _END_WHILE: {
			Get();
			Err(L"END WHILE without WHILE"); 
			break;
		}
		case 101 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 102 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 103 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 104 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(214); break;
		}
}

void Parser::ConstructorDefinition(AST::Class *s, int &elems) {
		printv(3, "ConstructorDefinition"); 
		Context foo(_CONSTRUCTOR); 
		if (la->kind == _CONSTRUCTOR) {
			Get();
		} else if (la->kind == 111 /* "ctor" */) {
			Get();
		} else SynErr(215);
		if (elems & elem_body) Err(L"Constructors and destructor must come before other procedures"); 
		if (StartOf(30)) {
			ParameterList(p);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(216); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
		}
		Expect(_END_CONSTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(217); Get();}
		Newline();
		elems |= elem_head; 
}

void Parser::DestructorDefinition(AST::Class *s, int &elems) {
		printv(3, "DestructorDefinition"); 
		Context foo(_DESTRUCTOR); 
		if (la->kind == _DESTRUCTOR) {
			Get();
		} else if (la->kind == 113 /* "dtor" */) {
			Get();
		} else SynErr(218);
		if (elems & elem_dtor) Err(L"Class cannot have multiple destructors"); 
		else if (elems & elem_body) Err(L"Constructors and destructor must come before other procedures"); 
		if (la->kind == _leftParen) {
			Get();
			Expect(_rightParen);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(219); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
		}
		Expect(_END_DESTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(220); Get();}
		Newline();
		elems |= elem_head | elem_dtor; 
}

void Parser::EventDefinition(AST::Statement *s) {
		printv(3, "EventDefinition"); 
		AST::ParameterList *p = NULL; 
		Context foo(_EVENT); 
		Expect(_EVENT);
		Expect(_plainIdentifier);
		if (StartOf(30)) {
			ParameterList(p);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(221); Get();}
		Newline();
}

void Parser::FunctionDefinition(AST::Statement *s) {
		printv(3, "FunctionDefinition"); 
		Context foo(_FUNCTION); 
		bool isBodiless = false; 
		Expect(_FUNCTION);
		FunctionSignature(isBodiless);
		if (!isBodiless) {
			if (StartOf(7)) {
				Statement(stmt);
				while (StartOf(31)) {
					if (StartOf(7)) {
						Statement(stmt);
					} else {
						ProcMistake();
					}
				}
			}
			Expect(_END_FUNCTION);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(222); Get();}
			Newline();
		}
}

void Parser::MethodDefinition(AST::Statement *s) {
		printv(3, "MethodDefinition"); 
		bool isBodiless = false; 
		Context foo(_METHOD); 
		MethodSignature(isBodiless);
		if (!isBodiless) {
			if (StartOf(7)) {
				Statement(stmt);
				while (StartOf(31)) {
					if (StartOf(7)) {
						Statement(stmt);
					} else {
						ProcMistake();
					}
				}
			}
			Expect(_END_METHOD);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(223); Get();}
			Newline();
		}
}

void Parser::ObjectDefinition(AST::Statement *s) {
		printv(3, "ObjectDefinition"); 
		Context *subclass = NULL; 
		Expect(135 /* "object" */);
		Expect(_objectIdentifier);
		if (la->kind == _IS) {
			Inheritance();
			subclass = new Context(_IS); 
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(224); Get();}
		Newline();
		while (StartOf(32)) {
			switch (la->kind) {
			case _ABSTRACT: {
				AbstractClass(stmt);
				break;
			}
			case _CLASS: {
				ClassDefinition(stmt, 0);
				break;
			}
			case _FUNCTION: {
				FunctionDefinition(stmt);
				break;
			}
			case _METHOD: {
				MethodDefinition(stmt);
				break;
			}
			case 135 /* "object" */: {
				ObjectDefinition(stmt);
				break;
			}
			case _PROPERTY: {
				PropertyDefinition(stmt);
				break;
			}
			case _plainIdentifier: case _typedIdentifier: case _objectIdentifier: case _boxedIdentifier: case _DO: case _FOR: case _FOR_EACH: case _SELECT: case _TRY: case _WHILE: case 98 /* "call" */: case 110 /* "if" */: case 114 /* "dim" */: case 115 /* "var" */: case 117 /* "exit" */: case 121 /* "goto" */: case 133 /* "new" */: case 162 /* "return" */: case 163 /* "let" */: case 166 /* "throw" */: case 170 /* "wait" */: {
				Statement(stmt);
				break;
			}
			case _SUB: {
				SubDefinition(stmt);
				break;
			}
			}
		}
		Expect(_END_OBJECT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(225); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::OverrideMember(AST::Statement *s) {
		printv(3, "OverrideMember"); 
		Expect(136 /* "override" */);
		if (la->kind == _METHOD) {
			MethodDefinition(s);
		} else if (la->kind == _PROPERTY) {
			PropertyDefinition(s);
		} else SynErr(226);
}

void Parser::PropertyDefinition(AST::Statement *s) {
		printv(3, "PropertyDefinition"); 
		PropertySignature();
		if (la->kind == 134 /* "begin" */) {
			Context foo(_PROPERTY); 
			opt_param opt = opt_no; pass_by by = by_val; 
			Get();
			if (la->kind == _leftParen) {
				Get();
				Parameter(opt, by);
				while (WeakSeparator(_comma,12,33) ) {
					opt = opt_no; by = by_val; 
					Parameter(opt, by);
				}
				Expect(_rightParen);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(227); Get();}
			Newline();
			bool fGetter = false, fSetter = false; 
			while (la->kind == 158 /* "on" */) {
				Get();
				if (la->kind == 159 /* "get" */) {
					Get();
					if (fGetter) Err(L"Property already has a getter"); else fGetter = true; 
				} else if (la->kind == 160 /* "set" */) {
					Get();
					if (fSetter) Err(L"Property already has a Setter"); else fSetter = true; 
					opt_param opt = opt_no; pass_by by = by_val; 
					if (StartOf(12)) {
						Parameter(opt, by);
					} else if (la->kind == _leftParen) {
						Get();
						Parameter(opt, by);
						Expect(_rightParen);
					} else SynErr(228);
				} else SynErr(229);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(230); Get();}
				Newline();
				while (StartOf(7)) {
					Statement(stmt);
				}
			}
			Expect(_END_PROPERTY);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(231); Get();}
			Newline();
		}
}

void Parser::SharedMember(AST::Statement *s, int &elems) {
		printv(3, "SharedMember"); 
		bool f = false; 
		Expect(_SHARED);
		if (StartOf(34)) {
			elems |= elem_head; 
			if (la->kind == 114 /* "dim" */ || la->kind == 115 /* "var" */) {
				DimStatement(s);
			} else if (la->kind == 133 /* "new" */) {
				NewStatement(s);
			} else if (la->kind == 135 /* "object" */) {
				ObjectDefinition(s);
			} else if (la->kind == _PROPERTY) {
				PropertyDefinition(s);
			} else if (IsObjectInitializer()) {
				ObjectInitializerStatement(s);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(232); Get();}
				Newline();
			} else {
				DeclaratorList(f);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(233); Get();}
				Newline();
			}
		} else if (la->kind == _FUNCTION || la->kind == _METHOD || la->kind == _SUB) {
			elems |= elem_body; 
			if (la->kind == _FUNCTION) {
				FunctionDefinition(s);
			} else if (la->kind == _METHOD) {
				MethodDefinition(s);
			} else {
				SubDefinition(s);
			}
		} else SynErr(234);
}

void Parser::SubDefinition(AST::Statement *s) {
		printv(3, "SubDefinition"); 
		Context foo(_SUB); 
		bool isGeneric = false; 
		Expect(_SUB);
		Expect(_plainIdentifier);
		if (la->kind == _leftBracket) {
			GenericDefinition();
			isGeneric = true; 
		}
		if (StartOf(30)) {
			ParameterList(p);
		}
		if (la->kind == 165 /* "handles" */) {
			Get();
			Expect(_plainIdentifier);
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(235); Get();}
		Newline();
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(236); Get();}
			Expect(_newline);
		}
		if (StartOf(7)) {
			Statement(stmt);
			while (StartOf(31)) {
				if (StartOf(7)) {
					Statement(stmt);
				} else {
					ProcMistake();
				}
			}
		}
		Expect(_END_SUB);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(237); Get();}
		Newline();
}

void Parser::ClassType() {
		printv(3, "ClassType"); 
		Expect(_plainIdentifier);
		if (la->kind == _leftBracket) {
			GenericUsage();
		}
}

void Parser::GenericUsage() {
		printv(3, "GenericUsage"); 
		Expect(_leftBracket);
		if (la->kind == _plainIdentifier) {
			ClassType();
		} else if (StartOf(35)) {
			PrimitiveType(primitive_any, t);
		} else SynErr(238);
		while (WeakSeparator(_comma,36,28) ) {
			if (la->kind == _plainIdentifier) {
				ClassType();
			} else if (StartOf(35)) {
				PrimitiveType(t);
			} else SynErr(239);
		}
		Expect(_rightBracket);
}

void Parser::CompoundDoStatement(AST::Do *block) {
		printv(3, "CompoundDoStatement"); 
		bool until; 
		AST::Expression *e; 
		AST::Statement *s; 
		AST::Block *c; 
		if (la->kind == _WHILE || la->kind == 171 /* "until" */) {
			WhileOrUntil(until);
			block->type = (until) ? AST::DoUntilLoop : AST::DoWhileLoop; 
			Expression(e);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(240); Get();}
			Newline();
			while (StartOf(7)) {
				Statement(s);
				block->AddStatement(s); 
			}
			if (la->kind == 94 /* "afterward" */) {
				AfterwardClause((*block).after);
			}
			if (la->kind == 103 /* "otherwise" */) {
				OtherwiseClause((*block).other);
			}
			Expect(_LOOP);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(241); Get();}
			Newline();
		} else if (la->kind == _newline) {
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(242); Get();}
			Newline();
			while (StartOf(7)) {
				Statement(stmt);
				block->AddStatement(stmt); 
			}
			if (la->kind == 94 /* "afterward" */) {
				AfterwardClause((*block).after);
			}
			if (la->kind == 103 /* "otherwise" */) {
				Get();
				Err(L"OTHERWISE not allowed in post-conditional DO"); 
				while (StartOf(37)) {
					Get();
				}
			}
			Expect(_LOOP);
			WhileOrUntil(until);
			block->type = (until) ? AST::DoLoopUntil : AST::DoLoopWhile; 
			Expression((*block).cond);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(243); Get();}
			Newline();
		} else SynErr(244);
}

void Parser::WhileOrUntil(bool &until) {
		printv(3, "WhileOrUntil"); 
		if (la->kind == _WHILE) {
			Get();
			return false; 
		} else if (la->kind == 171 /* "until" */) {
			Get();
			return true; 
		} else SynErr(245);
}

void Parser::OtherwiseClause(AST::Block *block) {
		printv(3, "OtherwiseClause"); 
		AST::Statement *stmt; 
		Expect(103 /* "otherwise" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(246); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
		if (la->kind == 94 /* "afterward" */) {
			Get();
			Err(L"AFTERWARD must precede OTHERWISE"); 
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(247); Get();}
			Newline();
		}
}

void Parser::CompoundIfStatement(AST::If *block) {
		printv(3, "CompoundIfStatement"); 
		AST::Statement *stmt; 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(248); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
		while (la->kind == _ELSEIF) {
			AST::ElseIf *elif = new AST::ElseIf(NULL); 
			Get();
			Expression(e);
			elif->cond = e; 
			if (la->kind == 104 /* "then" */) {
				Get();
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(249); Get();}
			Newline();
			while (StartOf(7)) {
				Statement(stmt);
				elif->AddStatement(stmt); 
			}
			block->AddStatement(elif); 
		}
		if (la->kind == _ELSE) {
			Get();
			Newline();
			block->elseb = new Block(); 
			while (StartOf(7)) {
				Statement(stmt);
				block->elseb->AddStatement(stmt); 
			}
		}
		Expect(_END_IF);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(250); Get();}
		Newline();
}

void Parser::CompoundStatement(AST::Statement *stmt) {
		printv(3, "CompoundStatement"); 
		switch (la->kind) {
		case 114 /* "dim" */: case 115 /* "var" */: {
			DimStatement(s);
			break;
		}
		case _DO: {
			DoStatement(s);
			break;
		}
		case _FOR_EACH: {
			ForEachStatement(s);
			break;
		}
		case _FOR: {
			ForStatement(s);
			break;
		}
		case 110 /* "if" */: {
			IfStatement(s);
			break;
		}
		case 133 /* "new" */: {
			NewStatement(s);
			break;
		}
		case _SELECT: {
			SelectStatement(s);
			break;
		}
		case _TRY: {
			TryStatement(s);
			break;
		}
		case _WHILE: {
			WhileStatement(s);
			break;
		}
		default: SynErr(251); break;
		}
}

void Parser::DimStatement(AST::Statement *s) {
		printv(3, "DimStatement"); 
		bool f = false; 
		if (la->kind == 114 /* "dim" */) {
			Get();
		} else if (la->kind == 115 /* "var" */) {
			Get();
		} else SynErr(252);
		if (la->kind == _SHARED) {
			Get();
		}
		DeclaratorList(f);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(253); Get();}
		Newline();
}

void Parser::DoStatement(AST::Do *block) {
		printv(3, "DoStatement"); 
		Context foo(_DO); 
		Expect(_DO);
		if (la->kind == _newline || la->kind == _WHILE || la->kind == 171 /* "until" */) {
			CompoundDoStatement(block);
		} else if (StartOf(16)) {
			SimpleStatement(stmt);
			block->AddStatement(stmt); 
			WhileOrUntil(until);
			block->type = (until) ? AST::DoLoopUntil : AST::DoLoopWhile; 
			Expression((*block).cond);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(254); Get();}
			Newline();
		} else SynErr(255);
}

void Parser::ForEachStatement(AST::ForEach *block) {
		printv(3, "ForEachStatement"); 
		Context foo(_FOR); 
		AST::Statement *stmt; 
		Expect(_FOR_EACH);
		DeclaredName();
		Expect(_IN);
		Initializer();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(256); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
		if (la->kind == 94 /* "afterward" */) {
			AfterwardClause((*block).after);
		}
		if (la->kind == 103 /* "otherwise" */) {
			OtherwiseClause((*block).other);
		}
		Expect(_END_FOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(257); Get();}
		Newline();
}

void Parser::ForStatement(AST::For *block) {
		printv(3, "ForStatement"); 
		Context foo(_FOR); 
		Expect(_FOR);
		Expect(_plainIdentifier);
		Expect(_equals);
		Expression((*block).init);
		Expect(99 /* "to" */);
		Expression((*block).max);
		if (la->kind == 120 /* "step" */) {
			Get();
			Expression((*block).step);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(258); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
		if (la->kind == 94 /* "afterward" */) {
			AfterwardClause((*block).after);
		}
		if (la->kind == 103 /* "otherwise" */) {
			OtherwiseClause((*block).other);
		}
		Expect(_END_FOR);
		if (la->kind == _plainIdentifier) {
			Get();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(259); Get();}
		Newline();
}

void Parser::IfStatement(AST::If *block) {
		printv(3, "IfStatement"); 
		Expect(110 /* "if" */);
		Expression((*block).cond);
		if (la->kind == 104 /* "then" */) {
			Get();
			if (StartOf(16)) {
				SimpleStatement(stmt);
				block->AddStatement(stmt); 
				if (la->kind == _ELSE) {
					Get();
					SimpleStatement(stmt);
					block->elseb = new Block(); block->elseb->AddStatement(stmt); 
				}
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(260); Get();}
				Newline();
			} else if (la->kind == _newline) {
				CompoundIfStatement(block);
			} else SynErr(261);
		} else if (la->kind == _newline) {
			CompoundIfStatement(block);
		} else SynErr(262);
}

void Parser::NewStatement(AST::Statement *s) {
		printv(3, "NewStatement"); 
		Expect(133 /* "new" */);
		if (la->kind == _SHARED) {
			Get();
		}
		if (la->kind == _plainIdentifier) {
			ObjectInitializerStatement(s);
		} else if (la->kind == _leftBracket || la->kind == _objectIdentifier) {
			if (OutOfContext(_IS) || OutOfContext(_CONSTRUCTOR)) Err(L"Call to base constructor outside subclass constructor"); 
			if (la->kind == _leftBracket) {
				GenericUsage();
			}
			Expect(_objectIdentifier);
			if (wcscmp(t->val, L"#base") != 0) Err(L"#BASE required in call to base constructor"); 
			if (la->kind == _comma) {
				ExpectWeak(_comma, 38);
				ArgumentList();
			}
		} else SynErr(263);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(264); Get();}
		Newline();
		if (la->kind == 134 /* "begin" */) {
			int count = 0; 
			Get();
			if (la->kind == _plainIdentifier) {
				ClassType();
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(265); Get();}
			Newline();
			while (StartOf(39)) {
				NewStatementMember(s);
				count++; 
			}
			if (la->kind == _END_NEW) {
				Get();
			} else if (la->kind == _EndOfInitializer) {
				Get();
				Expect(_plainIdentifier);
			} else SynErr(266);
			if (count == 0) Err(L"NEW...BEGIN requires at least one member"); 
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(267); Get();}
			Newline();
		}
}

void Parser::SelectStatement(AST::Statement *s) {
		printv(3, "SelectStatement"); 
		int bCaseElse = false, line, col, count = 0; 
		Context foo(_SELECT); 
		Expect(_SELECT);
		if (la->kind == _CASE) {
			Get();
		}
		AdditiveExpression(e);
		if (la->kind == 167 /* "tol" */) {
			Tolerance(e);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(268); Get();}
		Newline();
		while (la->kind == _CASE) {
			CaseStatement(bCaseElse, line, col);
			CheckCase(bCaseElse, line, col, count); 
		}
		Expect(_END_SELECT);
		if (count == 0) Err(L"SELECT requires one or more CASEs"); 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(269); Get();}
		Newline();
}

void Parser::TryStatement(AST::Try *block) {
		printv(3, "TryStatement"); 
		Context foo(_TRY); 
		AST::Statement *stmt; 
		Expect(_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(270); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
			block->AddStatement(stmt); 
		}
		while (la->kind == 100 /* "catch" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(271); Get();}
			Newline();
			AST::Catch *cat = new AST::Catch(); 
			while (StartOf(7)) {
				Statement(stmt);
				cat->AddStatement(stmt); 
			}
			block->AddStatement(cat); 
		}
		if (la->kind == 101 /* "finally" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(272); Get();}
			Newline();
			block->finally = new AST::Block(); 
			while (StartOf(7)) {
				Statement(stmt);
				block->finally->AddStatement(stmt); 
			}
		}
		Expect(_END_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(273); Get();}
		Newline();
}

void Parser::WhileStatement(AST::Statement *b) {
		printv(3, "WhileStatement"); 
		Context foo(_WHILE); 
		Expect(_WHILE);
		Expression((*b).cond);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(274); Get();}
		Newline();
		while (StartOf(7)) {
			Statement(stmt);
		}
		if (la->kind == 94 /* "afterward" */) {
			AfterwardClause((*b).after);
		}
		if (la->kind == 103 /* "otherwise" */) {
			OtherwiseClause((*b).other);
		}
		Expect(_END_WHILE);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(275); Get();}
		Newline();
}

void Parser::ComparisonExpression(AST::Expression *e) {
		printv(2, "Comparison"); 
		unsigned int less = 0, notLess = 0; 
		AST::op_type op; 
		AST::Expression *e2; 
		BitShiftExpression(e);
		if (StartOf(40)) {
			while (StartOf(41)) {
				switch (la->kind) {
				case 105 /* "<" */: {
					Get();
					less++; op = AST::LessOp; 
					break;
				}
				case 106 /* "<=" */: {
					Get();
					less++; op = AST::LessOrEqualOp; 
					break;
				}
				case 107 /* ">" */: {
					Get();
					notLess++; op = AST::GreaterOp; 
					break;
				}
				case 108 /* ">=" */: {
					Get();
					notLess++; op = AST::GreaterOrEqualOp; 
					break;
				}
				case _equals: {
					Get();
					notLess++; op = AST::EqualOp; 
					break;
				}
				case 109 /* "<>" */: {
					Get();
					notLess++; op = AST::NotEqualOp; 
					break;
				}
				}
				BitShiftExpression(e2);
				e = new AST::ComparisonOp(e, op, e2); 
			}
			if (la->kind == 167 /* "tol" */) {
				Tolerance(e);
			}
		} else if (la->kind == _IS) {
			Inheritance();
		} else SynErr(276);
}

void Parser::Tolerance(AST::ComparisonOp *e) {
		printv(3, "Tolerance"); 
		Expect(167 /* "tol" */);
		if (StartOf(42)) {
			Number(e);
		} else if (la->kind == _plainIdentifier) {
			Get();
		} else SynErr(277);
}

void Parser::ParameterList(AST::ParameterList *p) {
		printv(3, "ParameterList"); 
		if (la->kind == _leftParen) {
			ParameterListEnclosed(p);
		} else if (StartOf(12)) {
			ParameterListUnenclosed(p);
		} else SynErr(278);
}

void Parser::DataTypeClause() {
		printv(3, "DataTypeClause"); 
		if (la->kind == 112 /* "as" */) {
			Get();
			if (la->kind == _plainIdentifier) {
				ClassType();
			} else if (StartOf(35)) {
				PrimitiveType(primitive_any, t);
			} else SynErr(279);
		} else if (la->kind == _IN) {
			Get();
			Unit();
		} else SynErr(280);
}

void Parser::PrimitiveType(primitive_limit lim, Type::Type *type) {
		printv(3, "PrimitiveType"); 
		primitive_limit sel; 
		switch (la->kind) {
		case 138 /* "boolean" */: {
			Get();
			sel = primitive_misc; type = new Type::Boolean(); 
			break;
		}
		case 139 /* "tiny" */: {
			Get();
			sel = primitive_int; type = new Type::Tiny(); 
			break;
		}
		case 140 /* "byte" */: {
			Get();
			sel = primitive_int; type = new Type::Byte(); 
			break;
		}
		case 141 /* "char" */: {
			Get();
			sel = primitive_char; type = new Type::Char(); 
			break;
		}
		case 142 /* "string" */: {
			Get();
			sel = primitive_char; type = new Type::String(); 
			break;
		}
		case 143 /* "short" */: {
			Get();
			sel = primitive_int; type = new Type::Short(); 
			break;
		}
		case 144 /* "ushort" */: {
			Get();
			sel = primitive_int; type = new Type:UShort(); 
			break;
		}
		case 145 /* "integer" */: {
			Get();
			sel = primitive_int; type = new Type::Integer(); 
			break;
		}
		case 146 /* "uinteger" */: {
			Get();
			sel = primitive_int; type = new Type::UInteger(); 
			break;
		}
		case 147 /* "single" */: {
			Get();
			sel = primitive_fp; type = new Type::Single(); 
			break;
		}
		case 148 /* "int" */: {
			Get();
			sel = primitive_int; type = new Type::Int(); 
			break;
		}
		case 149 /* "uint" */: {
			Get();
			sel = primitive_int; type = new Type::UInt(); 
			break;
		}
		case 150 /* "long" */: {
			Get();
			sel = primitive_int; type = new Type::Long(); 
			break;
		}
		case 151 /* "ulong" */: {
			Get();
			sel = primitive_int; type = new Type::ULong(); 
			break;
		}
		case 152 /* "date" */: {
			Get();
			sel = primitive_misc; type = new Type::Date(); 
			break;
		}
		case 153 /* "double" */: {
			Get();
			sel = primitive_fp; type = new Type::Double(); 
			break;
		}
		case 154 /* "xfp" */: {
			Get();
			sel = primitive_fp; type = new Type::XFP(); 
			break;
		}
		case 155 /* "huge" */: {
			Get();
			sel = primitive_int; type = new Type::Huge(); 
			break;
		}
		case 156 /* "uhuge" */: {
			Get();
			sel = primitive_int; type = new Type::UHuge(); 
			break;
		}
		case 157 /* "quad" */: {
			Get();
			sel = primitive_fp; type = new Type::Quad(); 
			break;
		}
		default: SynErr(281); break;
		}
		if (lim != primitive_any && sel != lim) { 
		switch (lim) { 
		case primitive_int: Err(L"Integer type expected"); break; 
		case primitive_char: Err(L"Char or String type expected"); break; 
		case primitive_fp: Err(L"Floating-point type expected"); break; 
		case primitive_misc: Err(L"Boolean or Date type expected"); break; 
		} 
		} 
}

void Parser::Declarator(bool &isGeneric) {
		printv(3, "Declarator"); 
		DeclaredName();
		if (la->kind == _leftBracket) {
			if (!isGeneric) Err(L"Declarator cannot be generic"); 
			GenericDefinition();
			isGeneric = true; 
		} else if (StartOf(43)) {
			isGeneric = false; 
		} else SynErr(282);
		if (la->kind == _leftParen) {
			Get();
			if (StartOf(20)) {
				EnclosedExpression(e);
				while (WeakSeparator(_comma,20,33) ) {
					EnclosedExpression(e);
				}
			}
			Expect(_rightParen);
		}
		if (la->kind == _IN || la->kind == 112 /* "as" */) {
			DataTypeClause();
		}
		if (la->kind == _equals) {
			Get();
			Initializer();
		}
}

void Parser::Initializer() {
		printv(3, "Initializer"); 
		if (StartOf(26)) {
			Expression(e);
		} else if (la->kind == _leftBrace) {
			Get();
			Initializer();
			while (WeakSeparator(_comma,45,44) ) {
				Initializer();
			}
			Expect(_rightBrace);
		} else SynErr(283);
}

void Parser::DeclaratorList(bool &isGeneric) {
		printv(3, "DeclaratorList"); 
		bool f = false; 
		Declarator(isGeneric);
		while (WeakSeparator(_comma,47,46) ) {
			Declarator(f);
		}
}

void Parser::SimpleStatement(AST::Statement *s) {
		printv(3, "SimpleStatement"); 
		if (la->kind == 98 /* "call" */) {
			CallStatement(s);
		} else if (la->kind == 121 /* "goto" */) {
			GotoStatement(s);
		} else if (la->kind == 117 /* "exit" */) {
			ExitStatement(s);
		} else if (la->kind == 163 /* "let" */) {
			Get();
			AssignmentStatement(s);
		} else if (la->kind == 162 /* "return" */) {
			ReturnStatement(s);
		} else if (la->kind == 166 /* "throw" */) {
			ThrowStatement(s);
		} else if (la->kind == 170 /* "wait" */) {
			WaitStatement(s);
		} else if (IsObjectInitializer()) {
			ObjectInitializerStatement(s);
		} else if (IsMethodCall()) {
			MethodCallStatement(s);
		} else if (StartOf(48)) {
			AssignmentStatement(s);
		} else SynErr(284);
}

void Parser::DotMember() {
		printv(3, "DotMember"); 
		Expect(_dot);
		if (la->kind == _plainIdentifier) {
			Get();
			if (la->kind == _dot) {
				DotMember();
			}
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else SynErr(285);
}

void Parser::LogicalXORExpression(AST::Expression *e) {
		printv(2, "LogicalXOR"); 
		AST::Expression *e2; 
		LogicalORExpression(e);
		while (la->kind == 127 /* "xor" */) {
			Get();
			LogicalORExpression(e2);
			e = new AST::BinaryOp(e, AST::XorOp, e2); 
		}
}

void Parser::EnumConstant(AST::Enum *s) {
		printv(3, "EnumConstant"); 
		wchar_t *id; 
		AST::Expression *e; 
		Expect(_plainIdentifier);
		id = coco_string_create(t->val); 
		if (la->kind == _equals) {
			Get();
			Number(e);
			s->AddConstant(id, e->val); 
		} else if (la->kind == _newline) {
			s->AddConstant(id); 
		} else SynErr(286);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(287); Get();}
		Newline();
		coco_string_delete(id); 
}

void Parser::Number(AST::Expression *e) {
		printv(2, "Number"); 
		switch (la->kind) {
		case _characterLiteral: {
			Get();
			e = new AST::CharConst(t->val); 
			break;
		}
		case _binaryLiteral: {
			Get();
			e = new AST::IntConst(t->val); 
			break;
		}
		case _octalLiteral: {
			Get();
			e = new AST::IntConst(t->val); 
			break;
		}
		case _decimalLiteral: {
			Get();
			e = new AST::IntConst(t->val); 
			break;
		}
		case _hexadecimalLiteral: {
			Get();
			e = new AST::IntConst(t->val); 
			break;
		}
		case _realLiteral: {
			Get();
			e = new AST::FPConst(t->val); 
			break;
		}
		case _dateLiteral: {
			Get();
			e = new AST::DTConst(t->val); 
			if (la->kind == _timeLiteral) {
				Get();
				AST::DTConst time(t->val); e->val += time.val; 
			}
			break;
		}
		case _timeLiteral: {
			Get();
			e = new AST::DTConst(t->val); 
			break;
		}
		default: SynErr(288); break;
		}
}

void Parser::EnumDefinition(AST::Statement *s) {
		printv(3, "EnumDefinition"); 
		wchar_t *id = NULL; 
		Type::Type *pt = NULL; 
		int count = 0; 
		Expect(116 /* "enum" */);
		if (la->kind == _plainIdentifier) {
			Get();
			id = coco_string_create(t->val); 
		}
		if (la->kind == 112 /* "as" */) {
			Get();
			if (la->kind == _plainIdentifier) {
				Get();
			} else if (StartOf(35)) {
				PrimitiveType(primitive_int, pt);
			} else SynErr(289);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(290); Get();}
		Newline();
		s = new AST::Enum(id); 
		while (la->kind == _plainIdentifier) {
			EnumConstant(s);
			count++; 
		}
		Expect(_END_ENUM);
		if (count == 0) Err(L"ENUM requires one or more constants"); 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(291); Get();}
		Newline();
}

void Parser::ExitStatement(AST::Statement *s) {
		printv(3, "ExitStatement"); 
		AST::block_type bt; 
		Expect(117 /* "exit" */);
		switch (la->kind) {
		case _DO: {
			Get();
			bt = AST::DoBlock; MustBeIn(_DO, L"EXIT DO outside DO"); 
			break;
		}
		case _FOR: {
			Get();
			bt = AST::ForBlock; MustBeIn(_FOR, L"EXIT FOR outside FOR"); 
			break;
		}
		case _FUNCTION: {
			Get();
			bt = AST::FunctionBlock; MustBeIn(_FUNCTION, L"EXIT FUNCTION outside FUNCTION"); 
			break;
		}
		case _METHOD: {
			Get();
			bt = AST::MethodBlock; MustBeIn(_METHOD, L"EXIT METHOD outside METHOD"); 
			break;
		}
		case _PROPERTY: {
			Get();
			bt = AST::PropertyBlock; MustBeIn(_PROPERTY, L"EXIT PROPERTY outside PROPERTY"); 
			break;
		}
		case _SUB: {
			Get();
			bt = AST::SubBlock; MustBeIn(_SUB, L"EXIT SUB outside SUB"); 
			break;
		}
		case _WHILE: {
			Get();
			bt = AST::WhileBlock; MustBeIn(_WHILE, L"EXIT WHILE outside WHILE"); 
			break;
		}
		default: SynErr(292); break;
		}
		s = new AST::Exit(bt); 
}

void Parser::FlagsDefinition(AST::Statement *s) {
		printv(3, "FlagsDefinition"); 
		wchar_t *id; 
		Type::Type *pt = NULL; 
		int count = 0; 
		Expect(118 /* "flags" */);
		Expect(_plainIdentifier);
		id = coco_string_create(t->val); 
		Expect(112 /* "as" */);
		if (la->kind == _plainIdentifier) {
			Get();
		} else if (StartOf(35)) {
			PrimitiveType(primitive_int, pt);
		} else SynErr(293);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(294); Get();}
		Newline();
		s = new AST::Flags(id); 
		while (la->kind == _plainIdentifier) {
			EnumConstant(s);
			count++; 
		}
		Expect(_END_FLAGS);
		if (count == 0) Err(L"Flags requires one or more constants"); 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(295); Get();}
		Newline();
}

void Parser::ParameterListEnclosed(AST::ParameterList *pl) {
		printv(3, "ParameterListEnclosed"); 
		AST::Parameter *p; 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		pl = new ParameterList(); 
		Expect(_leftParen);
		if (StartOf(12)) {
			Parameter(p, opt, by);
			pl->push_back(p); 
			while (WeakSeparator(_comma,12,33) ) {
				if (opt == opt_yes) opt = opt_warn; 
				by = by_any; 
				Parameter(p, opt, by);
				pl->push_back(p); 
			}
		}
		Expect(_rightParen);
}

void Parser::ParameterListUnenclosed(AST::ParameterList *pl) {
		printv(3, "ParameterListUnenclosed"); 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		pl = new ParameterList(); 
		AST::Parameter *p; 
		Parameter(p, opt, by);
		while (WeakSeparator(_comma,12,49) ) {
			if (opt == opt_yes) opt = opt_warn; 
			by = by_any; 
			Parameter(opt, by);
		}
}

void Parser::FunctionSignature(bool &isBodiless) {
		printv(3, "FunctionSignature"); 
		isBodiless = false; 
		bool isGeneric = false; 
		DeclaredName();
		if (StartOf(50)) {
			if (la->kind == _leftBracket) {
				GenericDefinition();
				isGeneric = true; 
			}
			if (StartOf(11)) {
				if (la->kind == _leftParen) {
					ParameterListEnclosed(p);
					if (la->kind == _IN || la->kind == 112 /* "as" */) {
						DataTypeClause();
					}
				} else if (StartOf(12)) {
					ParameterListUnenclosed(p);
				} else {
					DataTypeClause();
				}
			}
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(296); Get();}
			Newline();
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(297); Get();}
				Expect(_newline);
			}
		} else if (la->kind == _DOES) {
			Get();
			Expect(_plainIdentifier);
			Expect(_dot);
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
				DeclaredName();
			} else if (la->kind == _stringLiteral) {
				Get();
			} else SynErr(298);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(299); Get();}
			Newline();
			isBodiless = true; 
		} else SynErr(300);
}

void Parser::ProcMistake() {
		printv(3, "ProcMistake"); 
		switch (la->kind) {
		case 94 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 100 /* "catch" */: {
			Get();
			Err(L"CATCH without TRY"); 
			break;
		}
		case _ELSE: {
			Get();
			Err(L"ELSE without IF"); 
			break;
		}
		case _ELSEIF: {
			Get();
			Err(L"ELSEIF without IF"); 
			break;
		}
		case _END_CLASS: {
			Get();
			Err(L"END CLASS without CLASS"); 
			break;
		}
		case _END_IF: {
			Get();
			Err(L"END IF without IF"); 
			break;
		}
		case _END_CONSTRUCTOR: {
			Get();
			Err(L"END CONSTRUCTOR without CONSTRUCTOR"); 
			break;
		}
		case _END_DESTRUCTOR: {
			Get();
			Err(L"END DESTRUCTOR without DESTRUCTOR"); 
			break;
		}
		case _END_ENUM: {
			Get();
			Err(L"END ENUM without ENUM"); 
			break;
		}
		case _END_FOR: {
			Get();
			Err(L"END FOR without FOR"); 
			break;
		}
		case _END_OBJECT: {
			Get();
			Err(L"END OBJECT without OBJECT"); 
			break;
		}
		case _EndOfInitializer: {
			Get();
			Err(L"END without BEGIN"); 
			break;
		}
		case _END_PROPERTY: {
			Get();
			Err(L"END PROPERTY without PROPERTY"); 
			break;
		}
		case _END_SELECT: {
			Get();
			Err(L"END SELECT without SELECT"); 
			break;
		}
		case _END_STRUCT: {
			Get();
			Err(L"END STRUCT without STRUCT"); 
			break;
		}
		case _END_TRAIT: {
			Get();
			Err(L"END TRAIT without TRAIT"); 
			break;
		}
		case _END_TRY: {
			Get();
			Err(L"END TRY without TRY"); 
			break;
		}
		case _END_WHILE: {
			Get();
			Err(L"END WHILE without WHILE"); 
			break;
		}
		case 101 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 102 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 103 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 104 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(301); break;
		}
		while (StartOf(51)) {
			Get();
		}
		Newline();
}

void Parser::GenericConstraint() {
		printv(3, "GenericConstraint"); 
		Expect(_plainIdentifier);
		if (la->kind == 105 /* "<" */ || la->kind == 106 /* "<=" */) {
			if (la->kind == 105 /* "<" */) {
				Get();
			} else {
				Get();
			}
			Expect(_plainIdentifier);
			if (la->kind == 105 /* "<" */ || la->kind == 106 /* "<=" */) {
				if (la->kind == 105 /* "<" */) {
					Get();
				} else {
					Get();
				}
				Expect(_plainIdentifier);
			}
			if (la->kind == _DOES) {
				Get();
				Expect(_plainIdentifier);
			}
		} else if (la->kind == _DOES) {
			Get();
			Expect(_plainIdentifier);
		} else SynErr(302);
}

void Parser::GotoStatement(AST::Statement *s) {
		printv(3, "GotoStatement"); 
		AST::Expression *e; 
		Expect(121 /* "goto" */);
		MustBeIn(_SELECT, L"GOTO without SELECT"); 
		Expect(_CASE);
		if (StartOf(26)) {
			Expression(e);
			s = new AST::GotoCase(e); 
		} else if (la->kind == _ELSE) {
			Get();
			s = new AST::GotoCase(NULL); 
		} else SynErr(303);
}

void Parser::IdentifierExpression(AST::Expression *e) {
		printv(2, "Identifier"); 
		if (la->kind == _plainIdentifier) {
			Get();
			if (la->kind == _dot) {
				DotMember();
			} else if (IsVariable(t->val)) {
				Subscript();
			} else if (la->kind == _leftParen) {
				Get();
				if (StartOf(23)) {
					ArgumentList();
				}
				Expect(_rightParen);
			} else if (StartOf(52)) {
			} else SynErr(304);
		} else if (la->kind == _typedIdentifier) {
			TypedIdentifier(e);
			if (la->kind == _leftParen) {
				if (IsVariable(t->val)) {
					Subscript();
				} else {
					Get();
					if (StartOf(23)) {
						ArgumentList();
					}
					Expect(_rightParen);
				}
			}
		} else SynErr(305);
}

void Parser::Subscript() {
		printv(3, "Subscript"); 
		Expect(_leftParen);
		if (StartOf(20)) {
			EnclosedExpression(e);
			while (WeakSeparator(_comma,20,33) ) {
				EnclosedExpression(e);
			}
		}
		Expect(_rightParen);
}

void Parser::TypedIdentifier(AST::Expression *e) {
		Expect(_typedIdentifier);
		wchar_t *pc;
		if ((pc = wcschr(t->val, L'%'))) {
		bool isUnsigned = false;
		if (pc[1] == L's' || pc[1] == L'u') { isUnsigned = (pc[1] == L'u'); pc++; }
		switch ((short) pc[1]) {
		case 8: // Byte/Tiny
		break;
		case 1: // 16, (U)Short
		break;
		case 3: // 32, (U)Integer
		break;
		case 6: // 64, (U)Long
		break;
		default: // (U)Int
		}
		}
		else if ((pc = wcschr(t->val, L'#'))) {
		switch ((short) pc[1]) {
		case 1: // Single
		break;
		case 2: // Double
		break;
		case 4: // Quad
		break;
		default: // XFP
		break;
		}
		}
		else if (wcschr(t->val, L'$')) { // String
		} 
}

void Parser::LibraryAttribute() {
		printv(3, "LibraryAttribute"); 
		Expect(_plainIdentifier);
		Expect(_equals);
		if (la->kind == _versionLiteral) {
			Get();
		} else if (la->kind == _stringLiteral) {
			Get();
		} else SynErr(306);
}

void Parser::RequireStatement(AST::Statement *s) {
		printv(3, "RequireStatement"); 
		Expect(161 /* "require" */);
		Requirement();
		while (WeakSeparator(_comma,3,2) ) {
			Requirement();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(307); Get();}
		Newline();
}

void Parser::LibraryModuleDeclaration(AST::Statement *s) {
		printv(3, "LibraryModuleDeclaration"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractClass(s);
			break;
		}
		case 95 /* "base" */: {
			BaseUnitDefinition(s);
			break;
		}
		case _CLASS: {
			ClassDefinition(s, 0);
			break;
		}
		case 114 /* "dim" */: case 115 /* "var" */: {
			DimStatement(s);
			break;
		}
		case 116 /* "enum" */: {
			EnumDefinition(s);
			break;
		}
		case _EVENT: {
			EventDefinition(s);
			break;
		}
		case 118 /* "flags" */: {
			FlagsDefinition(s);
			break;
		}
		case _FUNCTION: {
			FunctionDefinition(s);
			break;
		}
		case _METHOD: {
			MethodDefinition(s);
			break;
		}
		case 133 /* "new" */: {
			NewStatement(s);
			break;
		}
		case 135 /* "object" */: {
			ObjectDefinition(s);
			break;
		}
		case _PROPERTY: {
			PropertyDefinition(s);
			break;
		}
		case 164 /* "struct" */: {
			StructDefinition(s);
			break;
		}
		case _SUB: {
			SubDefinition(s);
			break;
		}
		case 168 /* "trait" */: {
			TraitDefinition(s);
			break;
		}
		case _UNIT: {
			UnitDefinition(s);
			break;
		}
		default: SynErr(308); break;
		}
}

void Parser::ModuleMistake() {
		printv(3, "ModuleMistake"); 
		switch (la->kind) {
		case 94 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 100 /* "catch" */: {
			Get();
			Err(L"CATCH without TRY"); 
			break;
		}
		case _ELSE: {
			Get();
			Err(L"ELSE without IF"); 
			break;
		}
		case _ELSEIF: {
			Get();
			Err(L"ELSEIF without IF"); 
			break;
		}
		case _END_CLASS: {
			Get();
			Err(L"END CLASS without CLASS"); 
			break;
		}
		case _END_IF: {
			Get();
			Err(L"END IF without IF"); 
			break;
		}
		case _END_CONSTRUCTOR: {
			Get();
			Err(L"END CONSTRUCTOR without CONSTRUCTOR"); 
			break;
		}
		case _END_DESTRUCTOR: {
			Get();
			Err(L"END DESTRUCTOR without DESTRUCTOR"); 
			break;
		}
		case _END_ENUM: {
			Get();
			Err(L"END ENUM without ENUM"); 
			break;
		}
		case _END_FOR: {
			Get();
			Err(L"END FOR without FOR"); 
			break;
		}
		case _END_FUNCTION: {
			Get();
			Err(L"END FUNCTION without FUNCTION"); 
			break;
		}
		case _END_METHOD: {
			Get();
			Err(L"END METHOD without METHOD"); 
			break;
		}
		case _END_OBJECT: {
			Get();
			Err(L"END OBJECT without OBJECT"); 
			break;
		}
		case _EndOfInitializer: {
			Get();
			Err(L"END without BEGIN"); 
			break;
		}
		case _END_PROPERTY: {
			Get();
			Err(L"END PROPERTY without PROPERTY"); 
			break;
		}
		case _END_SELECT: {
			Get();
			Err(L"END SELECT without SELECT"); 
			break;
		}
		case _END_STRUCT: {
			Get();
			Err(L"END STRUCT without STRUCT"); 
			break;
		}
		case _END_SUB: {
			Get();
			Err(L"END SUB without SUB"); 
			break;
		}
		case _END_TRAIT: {
			Get();
			Err(L"END TRAIT without TRAIT"); 
			break;
		}
		case _END_TRY: {
			Get();
			Err(L"END TRY without TRY"); 
			break;
		}
		case _END_WHILE: {
			Get();
			Err(L"END WHILE without WHILE"); 
			break;
		}
		case 101 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 102 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 103 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 104 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(309); break;
		}
		while (StartOf(51)) {
			Get();
		}
		Newline();
}

void Parser::StructDefinition(AST::Statement *s) {
		printv(3, "StructDefinition"); 
		unsigned int members = 0; 
		Expect(164 /* "struct" */);
		Expect(_plainIdentifier);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(310); Get();}
		Newline();
		while (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			NarrowDeclaration();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(311); Get();}
			Newline();
			members++; 
		}
		if (members == 0) Err(L"STRUCT requires one or more members"); 
		Expect(_END_STRUCT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(312); Get();}
		Newline();
}

void Parser::TraitDefinition(AST::Statement *s) {
		printv(3, "TraitDefinition"); 
		Expect(168 /* "trait" */);
		Expect(_plainIdentifier);
		if (la->kind == _leftBracket) {
			GenericDefinition();
		}
		if (la->kind == _DOES) {
			Traits();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(313); Get();}
		Newline();
		while (la->kind == _METHOD) {
			MethodDefinition(s);
		}
		Expect(_END_TRAIT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(314); Get();}
		Newline();
}

void Parser::UnitDefinition(AST::Statement *s) {
		printv(3, "UnitDefinition"); 
		Expect(_UNIT);
		if (la->kind == _leftBracket) {
			UnitParameter();
		}
		if (la->kind == _leftParen) {
			Get();
			Expect(_plainIdentifier);
			Expect(_rightParen);
		}
		UnitAliases();
		if (la->kind == _equals) {
			Get();
			AdditiveExpression(e);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(315); Get();}
		Newline();
}

void Parser::LogicalANDExpression(AST::Expression *e) {
		printv(2, "LogicalAND"); 
		AST::op_type op; 
		AST::Expression *e2; 
		ComparisonExpression(e);
		while (la->kind == 123 /* "and" */ || la->kind == 124 /* "andthen" */) {
			LogicalANDOp(op);
			ComparisonExpression(e2);
			e = new AST::BinaryOp(e, op, e2); 
		}
}

void Parser::LogicalANDOp(AST::op_type &op) {
		if (la->kind == 123 /* "and" */) {
			Get();
			op = AST::AndOp; 
		} else if (la->kind == 124 /* "andthen" */) {
			Get();
			op = AST::AndThenOp; 
		} else SynErr(316);
}

void Parser::LogicalORExpression(AST::Expression *e) {
		printv(2, "LogicalOR"); 
		AST::Expression *e2; 
		LogicalANDExpression(e);
		while (la->kind == 125 /* "or" */ || la->kind == 126 /* "orelse" */) {
			LogicalOROp(op);
			LogicalANDExpression(e2);
			e = new AST::BinaryOp(e, op, e2); 
		}
}

void Parser::LogicalOROp(AST::op_type &op) {
		if (la->kind == 125 /* "or" */) {
			Get();
			op = AST::OrOp; 
		} else if (la->kind == 126 /* "orelse" */) {
			Get();
			op = AST::OrElseOp; 
		} else SynErr(317);
}

void Parser::MalformedToken() {
		printv(3, "MalformedToken"); 
		if (la->kind == _stringError) {
			Get();
			Err(L"Malformed string literal"); 
		} else if (la->kind == _versionError) {
			Get();
			Err(L"Malformed version literal"); 
		} else SynErr(318);
}

void Parser::MethodCallStatement(AST::Statement *s) {
		printv(3, "MethodCallStatement"); 
		if (la->kind == _objectIdentifier) {
			Get();
		} else if (la->kind == _boxedIdentifier) {
			Get();
		} else SynErr(319);
		if (la->kind == _dot) {
			Get();
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
				if (la->kind == _plainIdentifier) {
					Get();
				} else {
					Get();
				}
				if (la->kind == 128 /* "?" */) {
					Get();
				} else if (StartOf(25)) {
					AssignmentOp(op);
					Expression(e);
				} else if (la->kind == _equals) {
					Get();
					Expression(e);
				} else if (la->kind == _leftParen) {
					Get();
					if (StartOf(23)) {
						ArgumentList();
					}
					Expect(_rightParen);
				} else SynErr(320);
			} else if (la->kind == _objectIdentifier || la->kind == _boxedIdentifier) {
				MethodCallStatement(s);
			} else SynErr(321);
		} else if (StartOf(53)) {
			if (la->kind == 128 /* "?" */) {
				Get();
			}
			Expression(e);
		} else SynErr(322);
}

void Parser::PowerExpression(AST::Expression *e) {
		printv(2, "Power"); 
		AST::Expression *e2; 
		UnaryExpression(e);
		while (la->kind == 137 /* "^" */) {
			Get();
			UnaryExpression(e2);
			e = new BinaryOp(e, AST::PowOp, e2); 
		}
}

void Parser::MultiplicativeOp(AST::op_type &op) {
		if (la->kind == 129 /* "*" */) {
			Get();
			op = AST::MulOp; 
		} else if (la->kind == 130 /* "/" */) {
			Get();
			op = AST::DivOp; 
		} else if (la->kind == 131 /* "mod" */) {
			Get();
			op = AST::ModOp; 
		} else if (la->kind == 132 /* "rem" */) {
			Get();
			op = AST::RemOp; 
		} else SynErr(323);
}

void Parser::NarrowDeclaration() {
		printv(3, "NarrowDeclaration"); 
		if (la->kind == _plainIdentifier) {
			Get();
			DataTypeClause();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else SynErr(324);
}

void Parser::ObjectInitializerStatement(AST::Statement *s) {
		printv(3, "ObjectInitializerStatement"); 
		ClassType();
		Expect(_objectIdentifier);
		if (wcscmp(t->val, L"#base") == 0) Err(L"#BASE cannot be redefined"); 
		if (la->kind == _comma) {
			ExpectWeak(_comma, 38);
			ArgumentList();
		}
}

void Parser::NewStatementMember(AST::Begin *s) {
		printv(3, "NewStatementMember"); 
		AST::Statement *s2; 
		if (la->kind == 133 /* "new" */) {
			NewStatement(s2);
		} else if (IsObjectInitializer()) {
			ObjectInitializerStatement(s2);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(325); Get();}
			Newline();
		} else if (StartOf(23)) {
			ArgumentList();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(326); Get();}
			Newline();
		} else SynErr(327);
}

void Parser::ObjectExpression(AST::Expression *e) {
		printv(2, "Object"); 
		if (StartOf(54)) {
			if (la->kind == _objectIdentifier) {
				Get();
			} else if (la->kind == _boxedIdentifier) {
				Get();
			} else if (la->kind == _hashBASE) {
				Get();
			} else {
				Get();
			}
			while (la->kind == _dot) {
				Get();
				DeclaredName();
				if (la->kind == 128 /* "?" */) {
					Get();
				} else if (la->kind == _leftParen) {
					Get();
					if (StartOf(23)) {
						ArgumentList();
					}
					Expect(_rightParen);
				} else SynErr(328);
			}
			if (la->kind == _bang) {
				Get();
				DeclaredName();
			}
		} else if (la->kind == _hashNULL) {
			Get();
		} else if (la->kind == _nullAlias) {
			Get();
		} else SynErr(329);
}

void Parser::UnaryExpression(AST::Expression *e) {
		printv(2, "Unary"); 
		AST::op_type op = -1; 
		if (la->kind == 92 /* "+" */ || la->kind == 93 /* "-" */ || la->kind == 169 /* "not" */) {
			if (la->kind == 92 /* "+" */) {
				Get();
			} else if (la->kind == 93 /* "-" */) {
				Get();
				op = AST::NegOp; 
			} else {
				Get();
				op = AST::NotOp; 
			}
		}
		PrimaryExpression(e);
		if (op != -1) e = new AST::UnaryOp(op, e); 
}

void Parser::PrimaryExpression(AST::Expression *e) {
		printv(2, "Primary"); 
		switch (la->kind) {
		case _plainIdentifier: case _typedIdentifier: {
			IdentifierExpression(e);
			break;
		}
		case _binaryLiteral: case _octalLiteral: case _decimalLiteral: case _hexadecimalLiteral: case _realLiteral: case _dateLiteral: case _timeLiteral: case _characterLiteral: {
			Number(e);
			break;
		}
		case _objectIdentifier: case _boxedIdentifier: case _nullAlias: case _hashBASE: case _hashNULL: case _hashSELF: {
			ObjectExpression(e);
			break;
		}
		case _stringLiteral: case _metastring: {
			String(e);
			break;
		}
		case _leftParen: {
			Get();
			EnclosedExpression(e);
			Expect(_rightParen);
			break;
		}
		case _CLASS: case 138 /* "boolean" */: case 139 /* "tiny" */: case 140 /* "byte" */: case 141 /* "char" */: case 142 /* "string" */: case 143 /* "short" */: case 144 /* "ushort" */: case 145 /* "integer" */: case 146 /* "uinteger" */: case 147 /* "single" */: case 148 /* "int" */: case 149 /* "uint" */: case 150 /* "long" */: case 151 /* "ulong" */: case 152 /* "date" */: case 153 /* "double" */: case 154 /* "xfp" */: case 155 /* "huge" */: case 156 /* "uhuge" */: case 157 /* "quad" */: {
			TypeCast(e);
			break;
		}
		case _versionError: case _stringError: {
			MalformedToken();
			break;
		}
		default: SynErr(330); break;
		}
}

void Parser::String(AST::Expression *e) {
		printv(2, "String"); 
		int prev; 
		if (la->kind == _stringLiteral) {
			Get();
			if (t->val[0] == L'_') Err(L"String continuation without start"); 
		} else if (la->kind == _metastring) {
			Get();
		} else SynErr(331);
		prev = t->kind; 
		while (la->kind == _stringLiteral || la->kind == _metastring) {
			if (la->kind == _stringLiteral) {
				Get();
				if (prev == _stringLiteral && t->val[0] != L'_') Err(L"Expected string continuation"); 
			} else {
				Get();
			}
			prev = t->kind; 
		}
}

void Parser::TypeCast(AST::Expression *e) {
		printv(2, "Cast"); 
		if (StartOf(35)) {
			PrimitiveType(primitive_any, t);
		} else if (la->kind == _CLASS) {
			Get();
			ClassType();
		} else SynErr(332);
		Expect(_leftParen);
		EnclosedExpression(e);
		Expect(_rightParen);
}

void Parser::Requirement() {
		printv(3, "Requirement"); 
		Expect(_plainIdentifier);
		if (la->kind == _objectIdentifier) {
			Get();
		}
		if (la->kind == _WHERE) {
			RequirementRestriction();
		}
}

void Parser::RequirementRestriction() {
		printv(3, "RequirementRestriction"); 
		Expect(_WHERE);
		RequirementRelation();
		while (la->kind == 123 /* "and" */ || la->kind == 125 /* "or" */) {
			if (la->kind == 123 /* "and" */) {
				Get();
			} else {
				Get();
			}
			RequirementRelation();
		}
}

void Parser::RequirementRelation() {
		printv(3, "RequirementRelation"); 
		if (la->kind == _plainIdentifier) {
			Get();
			if (la->kind == _equals || la->kind == 109 /* "<>" */) {
				if (la->kind == _equals) {
					Get();
				} else {
					Get();
				}
				if (la->kind == _versionLiteral) {
					Get();
				} else if (la->kind == _stringLiteral) {
					Get();
				} else SynErr(333);
			} else if (StartOf(55)) {
				if (la->kind == 105 /* "<" */) {
					Get();
				} else if (la->kind == 106 /* "<=" */) {
					Get();
				} else if (la->kind == 107 /* ">" */) {
					Get();
				} else {
					Get();
				}
				Expect(_versionLiteral);
			} else SynErr(334);
		} else if (la->kind == _versionLiteral) {
			Get();
			if (la->kind == 105 /* "<" */) {
				Get();
			} else if (la->kind == 106 /* "<=" */) {
				Get();
			} else SynErr(335);
			Expect(_plainIdentifier);
			if (la->kind == 105 /* "<" */) {
				Get();
			} else if (la->kind == 106 /* "<=" */) {
				Get();
			} else SynErr(336);
			Expect(_versionLiteral);
		} else SynErr(337);
}

void Parser::ReturnStatement(AST::Statement *s) {
		printv(3, "ReturnStatement"); 
		AST::Expression *e = NULL; 
		Expect(162 /* "return" */);
		if (OutOfContext(_FUNCTION) && OutOfContext(_METHOD) && OutOfContext(_SUB) && OutOfContext(_PROPERTY))
		Err(L"RETURN outside procedure"); 
		if (StartOf(26)) {
			Expression(e);
		}
		s = new AST::Return(e); 
}

void Parser::ThrowStatement(AST::Statement *s) {
		printv(3, "ThrowStatement"); 
		AST::Expression *e; 
		Expect(166 /* "throw" */);
		Expression(e);
		s = new AST::Throw(e); 
}

void Parser::WaitStatement(AST::Statement *s) {
		printv(3, "WaitStatement"); 
		Expect(170 /* "wait" */);
		s = new AST::Wait(); 
}

void Parser::UnitAlias() {
		printv(3, "UnitAlias"); 
		Expect(_plainIdentifier);
		if (la->kind == _leftParen) {
			Get();
			Expect(_plainIdentifier);
			Expect(_rightParen);
		}
		if (la->kind == 130 /* "/" */) {
			Get();
			Expect(_plainIdentifier);
		}
}

void Parser::UnitAliases() {
		printv(3, "UnitAliases"); 
		UnitAlias();
		while (la->kind == _plainIdentifier) {
			UnitAlias();
		}
}

void Parser::UnitParameter() {
		printv(3, "UnitParameter"); 
		Expect(_leftBracket);
		if (la->kind == _plainIdentifier && scanner->Peek()->kind == _IN) {
			Expect(_plainIdentifier);
			Expect(_IN);
		}
		Unit();
		Expect(_rightBracket);
}

void Parser::UserModuleDeclaration(AST::Statement *s) {
		printv(3, "UserModuleDeclaration"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractClass(s);
			break;
		}
		case 95 /* "base" */: {
			BaseUnitDefinition(s);
			break;
		}
		case _CLASS: {
			ClassDefinition(s, 0);
			break;
		}
		case 114 /* "dim" */: case 115 /* "var" */: {
			DimStatement(s);
			break;
		}
		case 116 /* "enum" */: {
			EnumDefinition(s);
			break;
		}
		case _EVENT: {
			EventDefinition(s);
			break;
		}
		case 118 /* "flags" */: {
			FlagsDefinition(s);
			break;
		}
		case 133 /* "new" */: {
			NewStatement(s);
			break;
		}
		case 135 /* "object" */: {
			ObjectDefinition(s);
			break;
		}
		case 164 /* "struct" */: {
			StructDefinition(s);
			break;
		}
		case 168 /* "trait" */: {
			TraitDefinition(s);
			break;
		}
		case _UNIT: {
			UnitDefinition(s);
			break;
		}
		default: SynErr(338); break;
		}
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	VF1();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 172;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[56][174] = {
		{T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,T, x,T,x,x, T,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,T,x,x, T,T,x,T, x,x,x,T, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,x,T,x, T,x,T,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,T,T, x,T,T,T, T,T,x,T, T,T,T,x, T,T,T,T, T,T,T,T, T,x,x,T, x,x,T,T, T,x,x,T, x,T,T,x, x,x,T,T, x,x,x,x, T,T,T,T, T,x,x,x, x,x,x,x, x,x,T,T, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,T, T,x,x,T, x,T,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,T,x,x, T,x,x,T, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,T,T, x,T,T,T, T,T,x,T, T,T,T,x, T,T,T,T, T,T,T,T, T,x,x,T, x,x,T,x, x,x,x,T, x,T,T,x, x,x,T,T, x,x,x,x, T,T,T,T, T,x,x,x, x,x,x,x, x,x,T,T, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,T, x,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,T, T,T,T,T, T,x,x,T, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,T,T, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,T, T,T,T,T, T,x,T,T, x,T,x,T, T,T,x,T, x,T,x,x, T,T,T,T, x,T,T,T, T,T,T,T, x,x,T,T, T,T,T,T, T,x,T,T, x,x,T,x, x,x,T,x, T,T,T,T, T,x,x,x, x,x,T,T, x,T,T,T, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,T,x,x, T,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x},
		{x,T,T,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, T,x,x,x, T,x,T,T, x,T,T,T, T,T,x,T, x,T,x,x, T,T,T,T, x,T,T,T, x,T,T,x, x,x,T,x, x,T,x,x, T,x,T,T, x,x,T,x, x,x,T,x, T,T,T,T, T,x,x,x, x,x,T,x, x,x,T,T, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,T, x,T,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,T, T,T,x,T, T,x,x,T, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,T,x, x,x},
		{x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x},
		{T,T,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,T,T,x, x,x,T,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,T, x,x,x,x, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, T,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,T, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,x, x,T,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x},
		{x,T,T,x, x,x,T,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,T,T, T,T,x,x, T,T,x,T, x,x,x,x, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, T,x,x,T, T,T,T,T, x,T,T,T, T,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,T, x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"newline expected"); break;
			case 2: s = coco_string_create(L"comma expected"); break;
			case 3: s = coco_string_create(L"dot expected"); break;
			case 4: s = coco_string_create(L"bang expected"); break;
			case 5: s = coco_string_create(L"leftParen expected"); break;
			case 6: s = coco_string_create(L"rightParen expected"); break;
			case 7: s = coco_string_create(L"leftBracket expected"); break;
			case 8: s = coco_string_create(L"rightBracket expected"); break;
			case 9: s = coco_string_create(L"leftBrace expected"); break;
			case 10: s = coco_string_create(L"rightBrace expected"); break;
			case 11: s = coco_string_create(L"equals expected"); break;
			case 12: s = coco_string_create(L"assignOp expected"); break;
			case 13: s = coco_string_create(L"concatAssign expected"); break;
			case 14: s = coco_string_create(L"andAssign expected"); break;
			case 15: s = coco_string_create(L"orAssign expected"); break;
			case 16: s = coco_string_create(L"xorAssign expected"); break;
			case 17: s = coco_string_create(L"shlAssign expected"); break;
			case 18: s = coco_string_create(L"shrAssign expected"); break;
			case 19: s = coco_string_create(L"addAssign expected"); break;
			case 20: s = coco_string_create(L"subAssign expected"); break;
			case 21: s = coco_string_create(L"mulAssign expected"); break;
			case 22: s = coco_string_create(L"divAssign expected"); break;
			case 23: s = coco_string_create(L"modAssign expected"); break;
			case 24: s = coco_string_create(L"remAssign expected"); break;
			case 25: s = coco_string_create(L"concatOp expected"); break;
			case 26: s = coco_string_create(L"binaryLiteral expected"); break;
			case 27: s = coco_string_create(L"octalLiteral expected"); break;
			case 28: s = coco_string_create(L"decimalLiteral expected"); break;
			case 29: s = coco_string_create(L"hexadecimalLiteral expected"); break;
			case 30: s = coco_string_create(L"versionLiteral expected"); break;
			case 31: s = coco_string_create(L"versionError expected"); break;
			case 32: s = coco_string_create(L"realLiteral expected"); break;
			case 33: s = coco_string_create(L"dateLiteral expected"); break;
			case 34: s = coco_string_create(L"timeLiteral expected"); break;
			case 35: s = coco_string_create(L"characterLiteral expected"); break;
			case 36: s = coco_string_create(L"stringLiteral expected"); break;
			case 37: s = coco_string_create(L"stringError expected"); break;
			case 38: s = coco_string_create(L"metastring expected"); break;
			case 39: s = coco_string_create(L"plainIdentifier expected"); break;
			case 40: s = coco_string_create(L"typedIdentifier expected"); break;
			case 41: s = coco_string_create(L"objectIdentifier expected"); break;
			case 42: s = coco_string_create(L"boxedIdentifier expected"); break;
			case 43: s = coco_string_create(L"nullAlias expected"); break;
			case 44: s = coco_string_create(L"hashBASE expected"); break;
			case 45: s = coco_string_create(L"hashNULL expected"); break;
			case 46: s = coco_string_create(L"hashSELF expected"); break;
			case 47: s = coco_string_create(L"ABSTRACT expected"); break;
			case 48: s = coco_string_create(L"CASE expected"); break;
			case 49: s = coco_string_create(L"CLASS expected"); break;
			case 50: s = coco_string_create(L"CONSTRUCTOR expected"); break;
			case 51: s = coco_string_create(L"DESTRUCTOR expected"); break;
			case 52: s = coco_string_create(L"DO expected"); break;
			case 53: s = coco_string_create(L"DOES expected"); break;
			case 54: s = coco_string_create(L"ELSE expected"); break;
			case 55: s = coco_string_create(L"ELSEIF expected"); break;
			case 56: s = coco_string_create(L"End expected"); break;
			case 57: s = coco_string_create(L"EndOfInitializer expected"); break;
			case 58: s = coco_string_create(L"END_CLASS expected"); break;
			case 59: s = coco_string_create(L"END_CONSTRUCTOR expected"); break;
			case 60: s = coco_string_create(L"END_DESTRUCTOR expected"); break;
			case 61: s = coco_string_create(L"END_ENUM expected"); break;
			case 62: s = coco_string_create(L"END_FLAGS expected"); break;
			case 63: s = coco_string_create(L"END_FOR expected"); break;
			case 64: s = coco_string_create(L"END_FUNCTION expected"); break;
			case 65: s = coco_string_create(L"END_IF expected"); break;
			case 66: s = coco_string_create(L"END_METHOD expected"); break;
			case 67: s = coco_string_create(L"END_NEW expected"); break;
			case 68: s = coco_string_create(L"END_OBJECT expected"); break;
			case 69: s = coco_string_create(L"END_PROPERTY expected"); break;
			case 70: s = coco_string_create(L"END_SELECT expected"); break;
			case 71: s = coco_string_create(L"END_STRUCT expected"); break;
			case 72: s = coco_string_create(L"END_SUB expected"); break;
			case 73: s = coco_string_create(L"END_TRAIT expected"); break;
			case 74: s = coco_string_create(L"END_TRY expected"); break;
			case 75: s = coco_string_create(L"END_WHILE expected"); break;
			case 76: s = coco_string_create(L"EVENT expected"); break;
			case 77: s = coco_string_create(L"FOR expected"); break;
			case 78: s = coco_string_create(L"FOR_EACH expected"); break;
			case 79: s = coco_string_create(L"FUNCTION expected"); break;
			case 80: s = coco_string_create(L"IN expected"); break;
			case 81: s = coco_string_create(L"IS expected"); break;
			case 82: s = coco_string_create(L"LOOP expected"); break;
			case 83: s = coco_string_create(L"METHOD expected"); break;
			case 84: s = coco_string_create(L"PROPERTY expected"); break;
			case 85: s = coco_string_create(L"SELECT expected"); break;
			case 86: s = coco_string_create(L"SHARED expected"); break;
			case 87: s = coco_string_create(L"SUB expected"); break;
			case 88: s = coco_string_create(L"TRY expected"); break;
			case 89: s = coco_string_create(L"UNIT expected"); break;
			case 90: s = coco_string_create(L"WHERE expected"); break;
			case 91: s = coco_string_create(L"WHILE expected"); break;
			case 92: s = coco_string_create(L"\"+\" expected"); break;
			case 93: s = coco_string_create(L"\"-\" expected"); break;
			case 94: s = coco_string_create(L"\"afterward\" expected"); break;
			case 95: s = coco_string_create(L"\"base\" expected"); break;
			case 96: s = coco_string_create(L"\"shl\" expected"); break;
			case 97: s = coco_string_create(L"\"shr\" expected"); break;
			case 98: s = coco_string_create(L"\"call\" expected"); break;
			case 99: s = coco_string_create(L"\"to\" expected"); break;
			case 100: s = coco_string_create(L"\"catch\" expected"); break;
			case 101: s = coco_string_create(L"\"finally\" expected"); break;
			case 102: s = coco_string_create(L"\"optional\" expected"); break;
			case 103: s = coco_string_create(L"\"otherwise\" expected"); break;
			case 104: s = coco_string_create(L"\"then\" expected"); break;
			case 105: s = coco_string_create(L"\"<\" expected"); break;
			case 106: s = coco_string_create(L"\"<=\" expected"); break;
			case 107: s = coco_string_create(L"\">\" expected"); break;
			case 108: s = coco_string_create(L"\">=\" expected"); break;
			case 109: s = coco_string_create(L"\"<>\" expected"); break;
			case 110: s = coco_string_create(L"\"if\" expected"); break;
			case 111: s = coco_string_create(L"\"ctor\" expected"); break;
			case 112: s = coco_string_create(L"\"as\" expected"); break;
			case 113: s = coco_string_create(L"\"dtor\" expected"); break;
			case 114: s = coco_string_create(L"\"dim\" expected"); break;
			case 115: s = coco_string_create(L"\"var\" expected"); break;
			case 116: s = coco_string_create(L"\"enum\" expected"); break;
			case 117: s = coco_string_create(L"\"exit\" expected"); break;
			case 118: s = coco_string_create(L"\"flags\" expected"); break;
			case 119: s = coco_string_create(L"\"byref\" expected"); break;
			case 120: s = coco_string_create(L"\"step\" expected"); break;
			case 121: s = coco_string_create(L"\"goto\" expected"); break;
			case 122: s = coco_string_create(L"\"library\" expected"); break;
			case 123: s = coco_string_create(L"\"and\" expected"); break;
			case 124: s = coco_string_create(L"\"andthen\" expected"); break;
			case 125: s = coco_string_create(L"\"or\" expected"); break;
			case 126: s = coco_string_create(L"\"orelse\" expected"); break;
			case 127: s = coco_string_create(L"\"xor\" expected"); break;
			case 128: s = coco_string_create(L"\"?\" expected"); break;
			case 129: s = coco_string_create(L"\"*\" expected"); break;
			case 130: s = coco_string_create(L"\"/\" expected"); break;
			case 131: s = coco_string_create(L"\"mod\" expected"); break;
			case 132: s = coco_string_create(L"\"rem\" expected"); break;
			case 133: s = coco_string_create(L"\"new\" expected"); break;
			case 134: s = coco_string_create(L"\"begin\" expected"); break;
			case 135: s = coco_string_create(L"\"object\" expected"); break;
			case 136: s = coco_string_create(L"\"override\" expected"); break;
			case 137: s = coco_string_create(L"\"^\" expected"); break;
			case 138: s = coco_string_create(L"\"boolean\" expected"); break;
			case 139: s = coco_string_create(L"\"tiny\" expected"); break;
			case 140: s = coco_string_create(L"\"byte\" expected"); break;
			case 141: s = coco_string_create(L"\"char\" expected"); break;
			case 142: s = coco_string_create(L"\"string\" expected"); break;
			case 143: s = coco_string_create(L"\"short\" expected"); break;
			case 144: s = coco_string_create(L"\"ushort\" expected"); break;
			case 145: s = coco_string_create(L"\"integer\" expected"); break;
			case 146: s = coco_string_create(L"\"uinteger\" expected"); break;
			case 147: s = coco_string_create(L"\"single\" expected"); break;
			case 148: s = coco_string_create(L"\"int\" expected"); break;
			case 149: s = coco_string_create(L"\"uint\" expected"); break;
			case 150: s = coco_string_create(L"\"long\" expected"); break;
			case 151: s = coco_string_create(L"\"ulong\" expected"); break;
			case 152: s = coco_string_create(L"\"date\" expected"); break;
			case 153: s = coco_string_create(L"\"double\" expected"); break;
			case 154: s = coco_string_create(L"\"xfp\" expected"); break;
			case 155: s = coco_string_create(L"\"huge\" expected"); break;
			case 156: s = coco_string_create(L"\"uhuge\" expected"); break;
			case 157: s = coco_string_create(L"\"quad\" expected"); break;
			case 158: s = coco_string_create(L"\"on\" expected"); break;
			case 159: s = coco_string_create(L"\"get\" expected"); break;
			case 160: s = coco_string_create(L"\"set\" expected"); break;
			case 161: s = coco_string_create(L"\"require\" expected"); break;
			case 162: s = coco_string_create(L"\"return\" expected"); break;
			case 163: s = coco_string_create(L"\"let\" expected"); break;
			case 164: s = coco_string_create(L"\"struct\" expected"); break;
			case 165: s = coco_string_create(L"\"handles\" expected"); break;
			case 166: s = coco_string_create(L"\"throw\" expected"); break;
			case 167: s = coco_string_create(L"\"tol\" expected"); break;
			case 168: s = coco_string_create(L"\"trait\" expected"); break;
			case 169: s = coco_string_create(L"\"not\" expected"); break;
			case 170: s = coco_string_create(L"\"wait\" expected"); break;
			case 171: s = coco_string_create(L"\"until\" expected"); break;
			case 172: s = coco_string_create(L"??? expected"); break;
			case 173: s = coco_string_create(L"invalid VF1"); break;
			case 174: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 175: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 176: s = coco_string_create(L"this symbol not expected in UserModule"); break;
			case 177: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 178: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 179: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 180: s = coco_string_create(L"invalid AbstractMember"); break;
			case 181: s = coco_string_create(L"invalid MethodSignature"); break;
			case 182: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 183: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 184: s = coco_string_create(L"invalid MethodSignature"); break;
			case 185: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 186: s = coco_string_create(L"invalid MethodSignature"); break;
			case 187: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 188: s = coco_string_create(L"invalid PropertySignature"); break;
			case 189: s = coco_string_create(L"this symbol not expected in PropertySignature"); break;
			case 190: s = coco_string_create(L"invalid AdditiveOp"); break;
			case 191: s = coco_string_create(L"this symbol not expected in AfterwardClause"); break;
			case 192: s = coco_string_create(L"this symbol not expected in Statement"); break;
			case 193: s = coco_string_create(L"invalid Statement"); break;
			case 194: s = coco_string_create(L"invalid Parameter"); break;
			case 195: s = coco_string_create(L"invalid Parameter"); break;
			case 196: s = coco_string_create(L"invalid Parameter"); break;
			case 197: s = coco_string_create(L"invalid Argument"); break;
			case 198: s = coco_string_create(L"invalid VariableName"); break;
			case 199: s = coco_string_create(L"invalid ArgumentList"); break;
			case 200: s = coco_string_create(L"invalid ConditionalExpression"); break;
			case 201: s = coco_string_create(L"invalid AssignmentOp"); break;
			case 202: s = coco_string_create(L"invalid AssignmentStatement"); break;
			case 203: s = coco_string_create(L"invalid Mutable"); break;
			case 204: s = coco_string_create(L"invalid Mutable"); break;
			case 205: s = coco_string_create(L"this symbol not expected in BaseUnitDefinition"); break;
			case 206: s = coco_string_create(L"invalid BitShiftOp"); break;
			case 207: s = coco_string_create(L"invalid DeclaredName"); break;
			case 208: s = coco_string_create(L"invalid CaseExpression"); break;
			case 209: s = coco_string_create(L"invalid CaseExpression"); break;
			case 210: s = coco_string_create(L"invalid ComparisonOp"); break;
			case 211: s = coco_string_create(L"invalid CaseStatement"); break;
			case 212: s = coco_string_create(L"this symbol not expected in CaseStatement"); break;
			case 213: s = coco_string_create(L"invalid ClassMember"); break;
			case 214: s = coco_string_create(L"invalid ClassMistake"); break;
			case 215: s = coco_string_create(L"invalid ConstructorDefinition"); break;
			case 216: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 217: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 218: s = coco_string_create(L"invalid DestructorDefinition"); break;
			case 219: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 220: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 221: s = coco_string_create(L"this symbol not expected in EventDefinition"); break;
			case 222: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 223: s = coco_string_create(L"this symbol not expected in MethodDefinition"); break;
			case 224: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 225: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 226: s = coco_string_create(L"invalid OverrideMember"); break;
			case 227: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 228: s = coco_string_create(L"invalid PropertyDefinition"); break;
			case 229: s = coco_string_create(L"invalid PropertyDefinition"); break;
			case 230: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 231: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 232: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 233: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 234: s = coco_string_create(L"invalid SharedMember"); break;
			case 235: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 236: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 237: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 238: s = coco_string_create(L"invalid GenericUsage"); break;
			case 239: s = coco_string_create(L"invalid GenericUsage"); break;
			case 240: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 241: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 242: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 243: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 244: s = coco_string_create(L"invalid CompoundDoStatement"); break;
			case 245: s = coco_string_create(L"invalid WhileOrUntil"); break;
			case 246: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 247: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 248: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 249: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 250: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 251: s = coco_string_create(L"invalid CompoundStatement"); break;
			case 252: s = coco_string_create(L"invalid DimStatement"); break;
			case 253: s = coco_string_create(L"this symbol not expected in DimStatement"); break;
			case 254: s = coco_string_create(L"this symbol not expected in DoStatement"); break;
			case 255: s = coco_string_create(L"invalid DoStatement"); break;
			case 256: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 257: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 258: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 259: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 260: s = coco_string_create(L"this symbol not expected in IfStatement"); break;
			case 261: s = coco_string_create(L"invalid IfStatement"); break;
			case 262: s = coco_string_create(L"invalid IfStatement"); break;
			case 263: s = coco_string_create(L"invalid NewStatement"); break;
			case 264: s = coco_string_create(L"this symbol not expected in NewStatement"); break;
			case 265: s = coco_string_create(L"this symbol not expected in NewStatement"); break;
			case 266: s = coco_string_create(L"invalid NewStatement"); break;
			case 267: s = coco_string_create(L"this symbol not expected in NewStatement"); break;
			case 268: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 269: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 270: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 271: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 272: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 273: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 274: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 275: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 276: s = coco_string_create(L"invalid ComparisonExpression"); break;
			case 277: s = coco_string_create(L"invalid Tolerance"); break;
			case 278: s = coco_string_create(L"invalid ParameterList"); break;
			case 279: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 280: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 281: s = coco_string_create(L"invalid PrimitiveType"); break;
			case 282: s = coco_string_create(L"invalid Declarator"); break;
			case 283: s = coco_string_create(L"invalid Initializer"); break;
			case 284: s = coco_string_create(L"invalid SimpleStatement"); break;
			case 285: s = coco_string_create(L"invalid DotMember"); break;
			case 286: s = coco_string_create(L"invalid EnumConstant"); break;
			case 287: s = coco_string_create(L"this symbol not expected in EnumConstant"); break;
			case 288: s = coco_string_create(L"invalid Number"); break;
			case 289: s = coco_string_create(L"invalid EnumDefinition"); break;
			case 290: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 291: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 292: s = coco_string_create(L"invalid ExitStatement"); break;
			case 293: s = coco_string_create(L"invalid FlagsDefinition"); break;
			case 294: s = coco_string_create(L"this symbol not expected in FlagsDefinition"); break;
			case 295: s = coco_string_create(L"this symbol not expected in FlagsDefinition"); break;
			case 296: s = coco_string_create(L"this symbol not expected in FunctionSignature"); break;
			case 297: s = coco_string_create(L"this symbol not expected in FunctionSignature"); break;
			case 298: s = coco_string_create(L"invalid FunctionSignature"); break;
			case 299: s = coco_string_create(L"this symbol not expected in FunctionSignature"); break;
			case 300: s = coco_string_create(L"invalid FunctionSignature"); break;
			case 301: s = coco_string_create(L"invalid ProcMistake"); break;
			case 302: s = coco_string_create(L"invalid GenericConstraint"); break;
			case 303: s = coco_string_create(L"invalid GotoStatement"); break;
			case 304: s = coco_string_create(L"invalid IdentifierExpression"); break;
			case 305: s = coco_string_create(L"invalid IdentifierExpression"); break;
			case 306: s = coco_string_create(L"invalid LibraryAttribute"); break;
			case 307: s = coco_string_create(L"this symbol not expected in RequireStatement"); break;
			case 308: s = coco_string_create(L"invalid LibraryModuleDeclaration"); break;
			case 309: s = coco_string_create(L"invalid ModuleMistake"); break;
			case 310: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 311: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 312: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 313: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 314: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 315: s = coco_string_create(L"this symbol not expected in UnitDefinition"); break;
			case 316: s = coco_string_create(L"invalid LogicalANDOp"); break;
			case 317: s = coco_string_create(L"invalid LogicalOROp"); break;
			case 318: s = coco_string_create(L"invalid MalformedToken"); break;
			case 319: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 320: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 321: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 322: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 323: s = coco_string_create(L"invalid MultiplicativeOp"); break;
			case 324: s = coco_string_create(L"invalid NarrowDeclaration"); break;
			case 325: s = coco_string_create(L"this symbol not expected in NewStatementMember"); break;
			case 326: s = coco_string_create(L"this symbol not expected in NewStatementMember"); break;
			case 327: s = coco_string_create(L"invalid NewStatementMember"); break;
			case 328: s = coco_string_create(L"invalid ObjectExpression"); break;
			case 329: s = coco_string_create(L"invalid ObjectExpression"); break;
			case 330: s = coco_string_create(L"invalid PrimaryExpression"); break;
			case 331: s = coco_string_create(L"invalid String"); break;
			case 332: s = coco_string_create(L"invalid TypeCast"); break;
			case 333: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 334: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 335: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 336: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 337: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 338: s = coco_string_create(L"invalid UserModuleDeclaration"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


