
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
if (t->kind && args.v >= 1) wprintf(L"%s%d:%s", (t->val[0]==L'@' && !iswdigit(t->val[1]))?L"":L"\n", t->kind, t->val[0]>=32?t->val:L"???");
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }
		if (la->kind == _atCONST) {
				if (la->col != 1) Err(L"Directive not at start of line"); 
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
		if (la->kind == _atDEPRECATE) {
				printv(3, "@Deprecate"); 
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
		AST::Node *ast = NULL; 
		while (la->kind == _newline) {
			Get();
		}
		if (la->kind == 110 /* "library" */) {
			LibraryModule(ast);
		} else if (StartOf(1)) {
			UserModule(ast);
		} else if (la->kind == _EOF) {
			Warn(L"Module is empty."); 
		} else SynErr(156);
		Expect(_EOF);
		printf("\n-- %d %s\n", errors->count, (errors->count==1)?"error":"errors"); 
}

void Parser::LibraryModule(AST::Node *ast) {
		printv(3, "LibraryModule"); 
		Expect(110 /* "library" */);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,2) ) {
			LibraryAttribute();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(157); Get();}
		Newline();
		while (la->kind == _WHERE) {
			Get();
			LibraryAttribute();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(158); Get();}
			Newline();
		}
		while (la->kind == 145 /* "require" */) {
			RequireStatement();
		}
		LibraryModuleDeclaration();
		while (StartOf(4)) {
			if (StartOf(5)) {
				LibraryModuleDeclaration();
			} else {
				ModuleMistake();
			}
		}
}

void Parser::UserModule(AST::Node *ast) {
		printv(3, "UserModule"); 
		while (la->kind == 145 /* "require" */) {
			RequireStatement();
		}
		while (StartOf(6)) {
			UserModuleDeclaration();
		}
		while (StartOf(7)) {
			Statement();
		}
		Expect(_End);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(159); Get();}
		Newline();
		while (StartOf(8)) {
			if (StartOf(6)) {
				UserModuleDeclaration();
			} else if (la->kind == _FUNCTION) {
				FunctionDefinition();
			} else if (la->kind == _SUB) {
				SubDefinition();
			} else {
				ModuleMistake();
			}
		}
}

void Parser::AbstractClass() {
		printv(3, "AbstractClass"); 
		Expect(_ABSTRACT);
		ClassDefinition(_ABSTRACT);
}

void Parser::ClassDefinition(int abstract) {
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
		if (la->kind == 108 /* "does" */) {
			Traits();
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(160); Get();}
		Newline();
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(161); Get();}
			Newline();
		}
		if (StartOf(9)) {
			ClassMember(elems);
			while (StartOf(10)) {
				if (StartOf(9)) {
					ClassMember(elems);
				} else {
					ClassMistake();
				}
			}
		}
		Expect(_END_CLASS);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(162); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::AbstractMember() {
		printv(3, "AbstractMember"); 
		Expect(_ABSTRACT);
		if (la->kind == _CLASS) {
			ClassDefinition(_ABSTRACT);
		} else if (la->kind == _METHOD) {
			MethodSignature();
		} else if (la->kind == _PROPERTY) {
			PropertySignature();
		} else SynErr(163);
}

void Parser::MethodSignature() {
		printv(3, "MethodSignature"); 
		bool isGeneric = false; 
		Expect(_METHOD);
		if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
			DeclaredName();
			if (la->kind == _leftBracket) {
				GenericDefinition();
				isGeneric = true; 
			}
			if (StartOf(11)) {
				if (la->kind == _leftParen) {
					FormalParamsEnclosed();
					if (la->kind == _IN || la->kind == 99 /* "as" */) {
						DataTypeClause();
					}
				} else if (StartOf(12)) {
					FormalParamsUnenclosed();
				} else {
					DataTypeClause();
				}
			}
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(164); Get();}
			Newline();
		} else if (StartOf(13)) {
			if (la->kind == _leftBracket) {
				GenericDefinition();
				isGeneric = true; 
			}
			if (la->kind == _leftParen) {
				AnonFormalParameter();
			} else if (la->kind == _IN || la->kind == 99 /* "as" */) {
				DataTypeClause();
			} else SynErr(165);
			if (la->kind == _WHERE) {
				GenericConstraints(isGeneric);
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(166); Get();}
			Newline();
		} else SynErr(167);
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(168); Get();}
			Expect(_newline);
		}
}

void Parser::PropertySignature() {
		printv(3, "PropertySignature"); 
		bool isGeneric = true; 
		Expect(_PROPERTY);
		if (la->kind == _bang) {
			Get();
		}
		DeclaratorList(isGeneric);
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(169); Get();}
		Newline();
}

void Parser::AdditiveExpression() {
		printv(2, "Additive"); 
		MultiplicativeExpression();
		while (la->kind == 78 /* "+" */ || la->kind == 79 /* "-" */) {
			if (la->kind == 78 /* "+" */) {
				Get();
			} else {
				Get();
			}
			MultiplicativeExpression();
		}
		if (la->kind == _IN) {
			Get();
			Unit();
		}
}

void Parser::MultiplicativeExpression() {
		printv(2, "Multiplicative"); 
		PowerExpression();
		while (StartOf(14)) {
			if (la->kind == 105 /* "*" */) {
				Get();
			} else if (la->kind == 117 /* "/" */) {
				Get();
			} else if (la->kind == 118 /* "mod" */) {
				Get();
			} else {
				Get();
			}
			PowerExpression();
		}
		if (la->kind == _plainIdentifier) {
			Unit();
		}
}

void Parser::Unit() {
		printv(3, "Unit"); 
		Expect(_plainIdentifier);
		if (la->kind == 117 /* "/" */) {
			Get();
			Expect(_plainIdentifier);
		}
}

void Parser::AfterwardClause() {
		printv(3, "AfterwardClause"); 
		Expect(80 /* "afterward" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(170); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
}

void Parser::Newline() {
		printv(3, "Newline"); 
		Expect(_newline);
		while (la->kind == _newline) {
			Get();
		}
}

void Parser::Statement() {
		printv(3, "Statement"); 
		if (StartOf(15)) {
			SimpleStatement();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(171); Get();}
			Newline();
		} else if (StartOf(16)) {
			CompoundStatement();
		} else SynErr(172);
}

void Parser::AnonFormalParameter() {
		printv(3, "AnonFormalParameter"); 
		opt_param opt = opt_no; 
		pass_by by = by_any; 
		Expect(_leftParen);
		FormalParameter(opt, by);
		Expect(_rightParen);
}

void Parser::FormalParameter(opt_param &opt, pass_by &by) {
		printv(3, "FormalParameter"); 
		if (la->kind == 94 /* "optional" */) {
			Get();
			if (opt == opt_no) Err(L"Parameter cannot be optional"); 
			else if (opt == opt_warn) Warn(L"Ignoring redundant OPTIONAL modifier"); 
			opt = opt_yes; 
		} else if (StartOf(17)) {
			opt = opt_no; 
		} else SynErr(173);
		if (la->kind == 107 /* "byref" */) {
			Get();
			if (by == by_val) Err(L"BYREF is not allowed here"); 
			by = by_ref; 
		} else if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _objectIdentifier) {
			if (by == by_ref) Err(L"BYREF is required here"); 
			by = by_val; 
		} else SynErr(174);
		if (la->kind == _typedIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				Get();
				Expect(_rightParen);
			}
		} else if (la->kind == _plainIdentifier) {
			Get();
			if (StartOf(18)) {
				if (la->kind == _IN || la->kind == 99 /* "as" */) {
					DataTypeClause();
				} else if (la->kind == _leftParen) {
					Get();
					Expect(_rightParen);
					if (la->kind == _IN || la->kind == 99 /* "as" */) {
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
		} else SynErr(175);
}

void Parser::Argument(arg_kind &prev) {
		printv(3, "Argument"); 
		if (StartOf(19)) {
			if (prev == arg_named) Err(L"Positional arguments cannot follow named arguments"); 
			prev = arg_pos; 
			EnclosedExpression();
		} else if (la->kind == _leftBracket) {
			if (prev == arg_none) Warn(L"Ignoring empty arguments before named argument"); 
			prev = arg_named; 
			Get();
			VariableName();
			Expect(_rightBracket);
			if (la->kind == _equals) {
				Get();
			}
			EnclosedExpression();
		} else SynErr(176);
}

void Parser::EnclosedExpression() {
		printv(2, "Enclosed"); 
		LogicalXORExpression();
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
		} else SynErr(177);
}

void Parser::ArgumentList() {
		printv(3, "ArgumentList"); 
		arg_kind prev = arg_any; 
		Argument(prev);
		while (WeakSeparator(_comma,21,20) ) {
			if (StartOf(22)) {
				Argument(prev);
			} else if (StartOf(23)) {
				if (prev == arg_named) Err(L"Positional arguments cannot follow named arguments"); 
				prev = arg_none; 
			} else SynErr(178);
		}
}

void Parser::AssignmentExpression() {
		printv(2, "Assignment"); 
		ConditionalExpression();
		while (la->kind == _assignOp) {
			Get();
			ConditionalExpression();
		}
}

void Parser::ConditionalExpression() {
		printv(2, "Conditional"); 
		if (StartOf(19)) {
			EnclosedExpression();
		} else if (la->kind == 97 /* "if" */) {
			Get();
			EnclosedExpression();
			Expect(96 /* "then" */);
			EnclosedExpression();
			Expect(_ELSE);
			EnclosedExpression();
		} else SynErr(179);
}

void Parser::AssignmentStatement() {
		printv(3, "AssignmentStatement"); 
		Mutable();
		if (la->kind == _equals) {
			Get();
			Expression();
		} else if (la->kind == _assignOp) {
			Get();
			Expression();
		} else SynErr(180);
}

void Parser::Mutable() {
		printv(3, "Mutable"); 
		if (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			IdentifierExpression();
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
					} else SynErr(181);
				}
			}
		} else SynErr(182);
}

void Parser::Expression() {
		printv(3, "Expression"); 
		AssignmentExpression();
		if (la->kind == _rightParen) { Get(); Err(L"Mismatched parentheses"); } 
}

void Parser::BaseUnitDefinition() {
		printv(3, "BaseUnitDefinition"); 
		Expect(81 /* "base" */);
		Expect(_UNIT);
		Expect(_plainIdentifier);
		Expect(_IN);
		Unit();
		if (la->kind == _equals) {
			Get();
			MultiplicativeExpression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(183); Get();}
		Newline();
}

void Parser::BeginStatement() {
		printv(3, "BeginStatement"); 
		Expect(82 /* "begin" */);
		if (la->kind == _SHARED) {
			Get();
		}
		ClassType();
		if (la->kind == _objectIdentifier) {
			Get();
		}
		if (la->kind == _comma) {
			ExpectWeak(_comma, 24);
			ArgumentList();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(184); Get();}
		Newline();
		BeginStatementMember();
		while (StartOf(25)) {
			BeginStatementMember();
		}
		Expect(_EndOfInitializer);
		Expect(_plainIdentifier);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(185); Get();}
		Newline();
}

void Parser::ClassType() {
		printv(3, "ClassType"); 
		Expect(_plainIdentifier);
		if (la->kind == _leftBracket) {
			GenericUsage();
		}
}

void Parser::BeginStatementMember() {
		printv(3, "BeginStatementMember"); 
		if (la->kind == 82 /* "begin" */) {
			BeginStatement();
		} else if (IsObjectInitializer()) {
			ObjectInitializerStatement();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(186); Get();}
			Newline();
		} else if (StartOf(22)) {
			ArgumentList();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(187); Get();}
			Newline();
		} else SynErr(188);
}

void Parser::ObjectInitializerStatement() {
		printv(3, "ObjectInitializerStatement"); 
		ClassType();
		Expect(_objectIdentifier);
		if (wcscmp(t->val, L"#base") == 0) Err(L"#BASE cannot be redefined"); 
		if (la->kind == _comma) {
			ExpectWeak(_comma, 24);
			ArgumentList();
		}
}

void Parser::BitShiftExpression() {
		printv(2, "BitShift"); 
		ConcatenativeExpression();
		while (la->kind == 83 /* "shl" */ || la->kind == 84 /* "shr" */) {
			if (la->kind == 83 /* "shl" */) {
				Get();
			} else {
				Get();
			}
			ConcatenativeExpression();
		}
}

void Parser::ConcatenativeExpression() {
		printv(2, "Concatenative"); 
		AdditiveExpression();
		while (la->kind == _concatOp) {
			Get();
			AdditiveExpression();
		}
}

void Parser::CallStatement() {
		printv(3, "CallStatement"); 
		Expect(85 /* "call" */);
		DeclaredName();
		if (StartOf(22)) {
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
		} else SynErr(189);
}

void Parser::CaseExpression() {
		printv(3, "CaseExpression"); 
		if (StartOf(26)) {
			Expression();
			if (la->kind == 86 /* "to" */) {
				Get();
				Expression();
			}
		} else if (la->kind == _IS) {
			Get();
			switch (la->kind) {
			case 87 /* "<" */: {
				Get();
				break;
			}
			case 88 /* "<=" */: {
				Get();
				break;
			}
			case _equals: {
				Get();
				break;
			}
			case 89 /* "<>" */: {
				Get();
				break;
			}
			case 90 /* ">=" */: {
				Get();
				break;
			}
			case 91 /* ">" */: {
				Get();
				break;
			}
			default: SynErr(190); break;
			}
			Expression();
		} else SynErr(191);
}

void Parser::CaseStatement(int &bCaseElse, int &line, int &col) {
		printv(3, "CaseStatement"); 
		Expect(_CASE);
		line = t->line; col = t->col; 
		if (StartOf(27)) {
			CaseExpression();
			while (WeakSeparator(_comma,27,2) ) {
				CaseExpression();
			}
		} else if (la->kind == _ELSE) {
			Get();
			bCaseElse = true; 
		} else SynErr(192);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(193); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
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
		Expect(108 /* "does" */);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,29) ) {
			Expect(_plainIdentifier);
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

void Parser::ClassMember(int &elems) {
		printv(3, "ClassMember"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractMember();
			break;
		}
		case _CLASS: {
			ClassDefinition(0);
			break;
		}
		case _CONSTRUCTOR: case 98 /* "ctor" */: {
			ConstructorDefinition(elems);
			break;
		}
		case _DESTRUCTOR: case 100 /* "dtor" */: {
			DestructorDefinition(elems);
			break;
		}
		case _EVENT: {
			EventDefinition();
			break;
		}
		case _FUNCTION: {
			FunctionDefinition();
			elems |= elem_body; 
			break;
		}
		case _METHOD: {
			MethodDefinition();
			elems |=  elem_body; 
			break;
		}
		case 121 /* "object" */: {
			ObjectDefinition();
			break;
		}
		case 122 /* "override" */: {
			OverrideMember();
			break;
		}
		case _PROPERTY: {
			PropertyDefinition();
			break;
		}
		case _SHARED: {
			SharedMember(elems);
			break;
		}
		case _plainIdentifier: case _typedIdentifier: case _objectIdentifier: case _boxedIdentifier: case _DO: case _FOR: case _FOR_EACH: case _SELECT: case _TRY: case _WHILE: case 82 /* "begin" */: case 85 /* "call" */: case 97 /* "if" */: case 101 /* "dim" */: case 102 /* "var" */: case 106 /* "exit" */: case 109 /* "goto" */: case 120 /* "new" */: case 146 /* "return" */: case 147 /* "let" */: case 149 /* "throw" */: case 153 /* "wait" */: {
			Statement();
			break;
		}
		case _SUB: {
			SubDefinition();
			elems |= elem_body; 
			break;
		}
		default: SynErr(194); break;
		}
}

void Parser::ClassMistake() {
		printv(3, "ClassMistake"); 
		switch (la->kind) {
		case 80 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 92 /* "catch" */: {
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
		case 93 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 94 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 95 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 96 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(195); break;
		}
}

void Parser::ConstructorDefinition(int &elems) {
		printv(3, "ConstructorDefinition"); 
		Context foo(_CONSTRUCTOR); 
		if (la->kind == _CONSTRUCTOR) {
			Get();
		} else if (la->kind == 98 /* "ctor" */) {
			Get();
		} else SynErr(196);
		if (elems & elem_body) Err(L"Constructors and destructor must come before other procedures"); 
		if (StartOf(30)) {
			FormalParameters();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(197); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		Expect(_END_CONSTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(198); Get();}
		Newline();
		elems |= elem_head; 
}

void Parser::DestructorDefinition(int &elems) {
		printv(3, "DestructorDefinition"); 
		Context foo(_DESTRUCTOR); 
		if (la->kind == _DESTRUCTOR) {
			Get();
		} else if (la->kind == 100 /* "dtor" */) {
			Get();
		} else SynErr(199);
		if (elems & elem_dtor) Err(L"Class cannot have multiple destructors"); 
		else if (elems & elem_body) Err(L"Constructors and destructor must come before other procedures"); 
		if (la->kind == _leftParen) {
			Get();
			Expect(_rightParen);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(200); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		Expect(_END_DESTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(201); Get();}
		Newline();
		elems |= elem_head | elem_dtor; 
}

void Parser::EventDefinition() {
		printv(3, "EventDefinition"); 
		Context foo(_EVENT); 
		Expect(_EVENT);
		Expect(_plainIdentifier);
		if (StartOf(30)) {
			FormalParameters();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(202); Get();}
		Newline();
}

void Parser::FunctionDefinition() {
		printv(3, "FunctionDefinition"); 
		Context foo(_FUNCTION); 
		int elems = 0; bool isGeneric = false; 
		Expect(_FUNCTION);
		DeclaredName();
		if (la->kind == _leftBracket) {
			GenericDefinition();
			isGeneric = true; 
		}
		if (StartOf(11)) {
			if (la->kind == _leftParen) {
				FormalParamsEnclosed();
				if (la->kind == _IN || la->kind == 99 /* "as" */) {
					DataTypeClause();
				}
			} else if (StartOf(12)) {
				FormalParamsUnenclosed();
			} else {
				DataTypeClause();
			}
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(203); Get();}
		Newline();
		while (la->kind == 94 /* "optional" */) {
			OptionalParameters();
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(204); Get();}
			Expect(_newline);
		}
		if (StartOf(7)) {
			Statement();
			while (StartOf(31)) {
				if (StartOf(7)) {
					Statement();
				} else {
					ProcMistake();
				}
			}
		}
		Expect(_END_FUNCTION);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(205); Get();}
		Newline();
}

void Parser::MethodDefinition() {
		printv(3, "MethodDefinition"); 
		Context foo(_METHOD); 
		MethodSignature();
		while (la->kind == 94 /* "optional" */) {
			OptionalParameters();
		}
		if (StartOf(7)) {
			Statement();
			while (StartOf(31)) {
				if (StartOf(7)) {
					Statement();
				} else {
					ProcMistake();
				}
			}
		}
		Expect(_END_METHOD);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(206); Get();}
		Newline();
}

void Parser::ObjectDefinition() {
		printv(3, "ObjectDefinition"); 
		Context *subclass = NULL; 
		Expect(121 /* "object" */);
		Expect(_objectIdentifier);
		if (la->kind == _IS) {
			Inheritance();
			subclass = new Context(_IS); 
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(207); Get();}
		Newline();
		while (StartOf(32)) {
			switch (la->kind) {
			case _ABSTRACT: {
				AbstractClass();
				break;
			}
			case _CLASS: {
				ClassDefinition(0);
				break;
			}
			case _FUNCTION: {
				FunctionDefinition();
				break;
			}
			case _METHOD: {
				MethodDefinition();
				break;
			}
			case 121 /* "object" */: {
				ObjectDefinition();
				break;
			}
			case _PROPERTY: {
				PropertyDefinition();
				break;
			}
			case _plainIdentifier: case _typedIdentifier: case _objectIdentifier: case _boxedIdentifier: case _DO: case _FOR: case _FOR_EACH: case _SELECT: case _TRY: case _WHILE: case 82 /* "begin" */: case 85 /* "call" */: case 97 /* "if" */: case 101 /* "dim" */: case 102 /* "var" */: case 106 /* "exit" */: case 109 /* "goto" */: case 120 /* "new" */: case 146 /* "return" */: case 147 /* "let" */: case 149 /* "throw" */: case 153 /* "wait" */: {
				Statement();
				break;
			}
			case _SUB: {
				SubDefinition();
				break;
			}
			}
		}
		Expect(_END_OBJECT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(208); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::OverrideMember() {
		printv(3, "OverrideMember"); 
		Expect(122 /* "override" */);
		if (la->kind == _METHOD) {
			MethodDefinition();
		} else if (la->kind == _PROPERTY) {
			PropertyDefinition();
		} else SynErr(209);
}

void Parser::PropertyDefinition() {
		printv(3, "PropertyDefinition"); 
		PropertySignature();
		if (la->kind == _END_PROPERTY || la->kind == 94 /* "optional" */ || la->kind == 144 /* "on" */) {
			Context foo(_PROPERTY); 
			if (la->kind == 94 /* "optional" */) {
				opt_param opt = opt_no; pass_by by = by_val; 
				Get();
				Expect(_leftParen);
				FormalParameter(opt, by);
				while (WeakSeparator(_comma,12,33) ) {
					opt = opt_no; by = by_val; 
					FormalParameter(opt, by);
				}
				Expect(_rightParen);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(210); Get();}
				Newline();
			}
			bool fGetter = false, fSetter = false; 
			while (la->kind == 144 /* "on" */) {
				Get();
				Expect(_plainIdentifier);
				if (StartOf(30)) {
					if (fSetter) Err(L"Property already has a Setter"); else fSetter = true; 
					opt_param opt = opt_no; pass_by by = by_val; 
					if (StartOf(12)) {
						FormalParameter(opt, by);
					} else {
						Get();
						FormalParameter(opt, by);
						Expect(_rightParen);
					}
				} else if (la->kind == _newline) {
					if (fGetter) Err(L"Property already has a getter"); else fGetter = true; 
				} else SynErr(211);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(212); Get();}
				Newline();
				while (StartOf(7)) {
					Statement();
				}
			}
			Expect(_END_PROPERTY);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(213); Get();}
			Newline();
		}
}

void Parser::SharedMember(int &elems) {
		printv(3, "SharedMember"); 
		bool f = false; 
		Expect(_SHARED);
		if (StartOf(34)) {
			elems |= elem_head; 
			if (la->kind == 82 /* "begin" */) {
				BeginStatement();
			} else if (la->kind == 101 /* "dim" */ || la->kind == 102 /* "var" */) {
				DimStatement();
			} else if (la->kind == 120 /* "new" */) {
				NewStatement();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(214); Get();}
				Newline();
			} else if (la->kind == 121 /* "object" */) {
				ObjectDefinition();
			} else if (la->kind == _PROPERTY) {
				PropertyDefinition();
			} else if (IsObjectInitializer()) {
				ObjectInitializerStatement();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(215); Get();}
				Newline();
			} else {
				DeclaratorList(f);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(216); Get();}
				Newline();
			}
		} else if (la->kind == _FUNCTION || la->kind == _METHOD || la->kind == _SUB) {
			elems |= elem_body; 
			if (la->kind == _FUNCTION) {
				FunctionDefinition();
			} else if (la->kind == _METHOD) {
				MethodDefinition();
			} else {
				SubDefinition();
			}
		} else SynErr(217);
}

void Parser::SubDefinition() {
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
			FormalParameters();
		}
		if (la->kind == 108 /* "does" */) {
			Get();
			Expect(_plainIdentifier);
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(218); Get();}
		Newline();
		while (la->kind == 94 /* "optional" */) {
			OptionalParameters();
		}
		if (la->kind == _WHERE) {
			GenericConstraints(isGeneric);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(219); Get();}
			Expect(_newline);
		}
		if (StartOf(7)) {
			Statement();
			while (StartOf(31)) {
				if (StartOf(7)) {
					Statement();
				} else {
					ProcMistake();
				}
			}
		}
		Expect(_END_SUB);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(220); Get();}
		Newline();
}

void Parser::GenericUsage() {
		printv(3, "GenericUsage"); 
		Expect(_leftBracket);
		if (la->kind == _plainIdentifier) {
			ClassType();
		} else if (StartOf(35)) {
			PrimitiveType();
		} else SynErr(221);
		while (WeakSeparator(_comma,36,28) ) {
			if (la->kind == _plainIdentifier) {
				ClassType();
			} else if (StartOf(35)) {
				PrimitiveType();
			} else SynErr(222);
		}
		Expect(_rightBracket);
}

void Parser::CompoundDoStatement() {
		printv(3, "CompoundDoStatement"); 
		if (la->kind == _WHILE || la->kind == 154 /* "until" */) {
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(223); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
			if (la->kind == 80 /* "afterward" */) {
				AfterwardClause();
			}
			if (la->kind == 95 /* "otherwise" */) {
				OtherwiseClause();
			}
			Expect(_LOOP);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(224); Get();}
			Newline();
		} else if (la->kind == _newline) {
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(225); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
			if (la->kind == 80 /* "afterward" */) {
				AfterwardClause();
			}
			if (la->kind == 95 /* "otherwise" */) {
				Get();
				Err(L"OTHERWISE not allowed in post-conditional DO"); 
				while (StartOf(37)) {
					Get();
				}
			}
			Expect(_LOOP);
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(226); Get();}
			Newline();
		} else SynErr(227);
}

void Parser::WhileOrUntil() {
		if (la->kind == _WHILE) {
			printv(3, "WhileOrUntil"); 
			Get();
		} else if (la->kind == 154 /* "until" */) {
			Get();
		} else SynErr(228);
}

void Parser::OtherwiseClause() {
		printv(3, "OtherwiseClause"); 
		Expect(95 /* "otherwise" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(229); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 80 /* "afterward" */) {
			Get();
			Err(L"AFTERWARD must precede OTHERWISE"); 
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(230); Get();}
			Newline();
		}
}

void Parser::CompoundIfStatement() {
		printv(3, "CompoundIfStatement"); 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(231); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		while (la->kind == _ELSEIF) {
			Get();
			Expression();
			if (la->kind == 96 /* "then" */) {
				Get();
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(232); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		if (la->kind == _ELSE) {
			Get();
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		Expect(_END_IF);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(233); Get();}
		Newline();
}

void Parser::CompoundStatement() {
		printv(3, "CompoundStatement"); 
		switch (la->kind) {
		case 82 /* "begin" */: {
			BeginStatement();
			break;
		}
		case 101 /* "dim" */: case 102 /* "var" */: {
			DimStatement();
			break;
		}
		case _DO: {
			DoStatement();
			break;
		}
		case _FOR_EACH: {
			ForEachStatement();
			break;
		}
		case _FOR: {
			ForStatement();
			break;
		}
		case 97 /* "if" */: {
			IfStatement();
			break;
		}
		case _SELECT: {
			SelectStatement();
			break;
		}
		case _TRY: {
			TryStatement();
			break;
		}
		case _WHILE: {
			WhileStatement();
			break;
		}
		default: SynErr(234); break;
		}
}

void Parser::DimStatement() {
		printv(3, "DimStatement"); 
		bool f = false; 
		if (la->kind == 101 /* "dim" */) {
			Get();
		} else if (la->kind == 102 /* "var" */) {
			Get();
		} else SynErr(235);
		if (la->kind == _SHARED) {
			Get();
		}
		DeclaratorList(f);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(236); Get();}
		Newline();
}

void Parser::DoStatement() {
		printv(3, "DoStatement"); 
		Context foo(_DO); 
		Expect(_DO);
		if (la->kind == _newline || la->kind == _WHILE || la->kind == 154 /* "until" */) {
			CompoundDoStatement();
		} else if (StartOf(15)) {
			SimpleStatement();
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(237); Get();}
			Newline();
		} else SynErr(238);
}

void Parser::ForEachStatement() {
		printv(3, "ForEachStatement"); 
		Context foo(_FOR); 
		Expect(_FOR_EACH);
		DeclaredName();
		Expect(_IN);
		Initializer();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(239); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 80 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 95 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_FOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(240); Get();}
		Newline();
}

void Parser::ForStatement() {
		printv(3, "ForStatement"); 
		Context foo(_FOR); 
		Expect(_FOR);
		Expect(_plainIdentifier);
		Expect(_equals);
		Expression();
		Expect(86 /* "to" */);
		Expression();
		if (la->kind == 104 /* "step" */) {
			Get();
			Expression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(241); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 80 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 95 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_FOR);
		if (la->kind == _plainIdentifier) {
			Get();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(242); Get();}
		Newline();
}

void Parser::IfStatement() {
		printv(3, "IfStatement"); 
		Expect(97 /* "if" */);
		Expression();
		if (la->kind == 96 /* "then" */) {
			Get();
			if (StartOf(15)) {
				SimpleStatement();
				if (la->kind == _ELSE) {
					Get();
					SimpleStatement();
				}
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(243); Get();}
				Newline();
			} else if (la->kind == _newline) {
				CompoundIfStatement();
			} else SynErr(244);
		} else if (la->kind == _newline) {
			CompoundIfStatement();
		} else SynErr(245);
}

void Parser::SelectStatement() {
		printv(3, "SelectStatement"); 
		int bCaseElse = false, line, col, cases = 0; 
		Context foo(_SELECT); 
		Expect(_SELECT);
		if (la->kind == _CASE) {
			Get();
		}
		AdditiveExpression();
		if (la->kind == 150 /* "tol" */) {
			Tolerance();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(246); Get();}
		Newline();
		while (la->kind == _CASE) {
			CaseStatement(bCaseElse, line, col);
			CheckCase(bCaseElse, line, col, cases); 
		}
		if (cases == 0) Err(L"SELECT requires one or more CASEs"); 
		Expect(_END_SELECT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(247); Get();}
		Newline();
}

void Parser::TryStatement() {
		printv(3, "TryStatement"); 
		Context foo(_TRY); 
		Expect(_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(248); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		while (la->kind == 92 /* "catch" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(249); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		if (la->kind == 93 /* "finally" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(250); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		Expect(_END_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(251); Get();}
		Newline();
}

void Parser::WhileStatement() {
		printv(3, "WhileStatement"); 
		Context foo(_WHILE); 
		Expect(_WHILE);
		Expression();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(252); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 80 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 95 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_WHILE);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(253); Get();}
		Newline();
}

void Parser::ComparisonExpression() {
		printv(2, "Comparison"); 
		unsigned int less = 0, notLess = 0; 
		BitShiftExpression();
		if (StartOf(38)) {
			while (StartOf(39)) {
				switch (la->kind) {
				case 87 /* "<" */: {
					Get();
					less++; 
					break;
				}
				case 88 /* "<=" */: {
					Get();
					less++; 
					break;
				}
				case 91 /* ">" */: {
					Get();
					notLess++; 
					break;
				}
				case 90 /* ">=" */: {
					Get();
					notLess++; 
					break;
				}
				case _equals: {
					Get();
					notLess++; 
					break;
				}
				case 89 /* "<>" */: {
					Get();
					notLess++; 
					break;
				}
				}
				BitShiftExpression();
			}
			if (la->kind == 150 /* "tol" */) {
				Tolerance();
			}
		} else if (la->kind == _IS) {
			Inheritance();
		} else SynErr(254);
}

void Parser::Tolerance() {
		printv(3, "Tolerance"); 
		Expect(150 /* "tol" */);
		if (StartOf(40)) {
			Number();
		} else if (la->kind == _plainIdentifier) {
			Get();
		} else SynErr(255);
}

void Parser::FormalParameters() {
		printv(3, "FormalParameters"); 
		if (la->kind == _leftParen) {
			FormalParamsEnclosed();
		} else if (StartOf(12)) {
			FormalParamsUnenclosed();
		} else SynErr(256);
}

void Parser::DataTypeClause() {
		printv(3, "DataTypeClause"); 
		if (la->kind == 99 /* "as" */) {
			Get();
			if (la->kind == _plainIdentifier) {
				ClassType();
			} else if (StartOf(35)) {
				PrimitiveType();
			} else SynErr(257);
		} else if (la->kind == _IN) {
			Get();
			Unit();
		} else SynErr(258);
}

void Parser::PrimitiveType() {
		printv(3, "PrimitiveType"); 
		switch (la->kind) {
		case 124 /* "boolean" */: {
			Get();
			break;
		}
		case 125 /* "tiny" */: {
			Get();
			break;
		}
		case 126 /* "byte" */: {
			Get();
			break;
		}
		case 127 /* "char" */: {
			Get();
			break;
		}
		case 128 /* "string" */: {
			Get();
			break;
		}
		case 129 /* "short" */: {
			Get();
			break;
		}
		case 130 /* "ushort" */: {
			Get();
			break;
		}
		case 131 /* "integer" */: {
			Get();
			break;
		}
		case 132 /* "uinteger" */: {
			Get();
			break;
		}
		case 133 /* "single" */: {
			Get();
			break;
		}
		case 134 /* "int" */: {
			Get();
			break;
		}
		case 135 /* "uint" */: {
			Get();
			break;
		}
		case 136 /* "long" */: {
			Get();
			break;
		}
		case 137 /* "ulong" */: {
			Get();
			break;
		}
		case 138 /* "date" */: {
			Get();
			break;
		}
		case 139 /* "double" */: {
			Get();
			break;
		}
		case 140 /* "xfp" */: {
			Get();
			break;
		}
		case 141 /* "huge" */: {
			Get();
			break;
		}
		case 142 /* "uhuge" */: {
			Get();
			break;
		}
		case 143 /* "quad" */: {
			Get();
			break;
		}
		default: SynErr(259); break;
		}
}

void Parser::Declarator(bool &isGeneric) {
		printv(3, "Declarator"); 
		DeclaredName();
		if (la->kind == _leftBracket) {
			if (!isGeneric) Err(L"Declarator cannot be generic"); 
			GenericDefinition();
			isGeneric = true; 
		} else if (StartOf(41)) {
			isGeneric = false; 
		} else SynErr(260);
		if (la->kind == _leftParen) {
			Get();
			if (StartOf(19)) {
				EnclosedExpression();
				while (WeakSeparator(_comma,19,33) ) {
					EnclosedExpression();
				}
			}
			Expect(_rightParen);
		}
		if (la->kind == _IN || la->kind == 99 /* "as" */) {
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
			Expression();
		} else if (la->kind == _leftBrace) {
			Get();
			Initializer();
			while (WeakSeparator(_comma,43,42) ) {
				Initializer();
			}
			Expect(_rightBrace);
		} else SynErr(261);
}

void Parser::DeclaratorList(bool &isGeneric) {
		printv(3, "DeclaratorList"); 
		bool f = false; 
		Declarator(isGeneric);
		while (WeakSeparator(_comma,44,29) ) {
			Declarator(f);
		}
}

void Parser::SimpleStatement() {
		printv(3, "SimpleStatement"); 
		if (la->kind == 85 /* "call" */) {
			CallStatement();
		} else if (la->kind == 109 /* "goto" */) {
			GotoStatement();
		} else if (la->kind == 106 /* "exit" */) {
			ExitStatement();
		} else if (la->kind == 147 /* "let" */) {
			Get();
			AssignmentStatement();
		} else if (la->kind == 120 /* "new" */) {
			NewStatement();
		} else if (la->kind == 146 /* "return" */) {
			ReturnStatement();
		} else if (la->kind == 149 /* "throw" */) {
			ThrowStatement();
		} else if (la->kind == 153 /* "wait" */) {
			WaitStatement();
		} else if (IsObjectInitializer()) {
			ObjectInitializerStatement();
		} else if (IsMethodCall()) {
			MethodCallStatement();
		} else if (StartOf(45)) {
			AssignmentStatement();
		} else SynErr(262);
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
		} else SynErr(263);
}

void Parser::LogicalXORExpression() {
		printv(2, "LogicalXOR"); 
		LogicalORExpression();
		while (la->kind == 115 /* "xor" */) {
			Get();
			LogicalORExpression();
		}
}

void Parser::EnumConstant() {
		printv(3, "EnumConstant"); 
		Expect(_plainIdentifier);
		if (la->kind == _equals) {
			Get();
			Number();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(264); Get();}
		Newline();
}

void Parser::Number() {
		printv(2, "Number"); 
		switch (la->kind) {
		case _characterLiteral: {
			Get();
			break;
		}
		case _binaryLiteral: {
			Get();
			break;
		}
		case _octalLiteral: {
			Get();
			break;
		}
		case _decimalLiteral: {
			Get();
			break;
		}
		case _hexadecimalLiteral: {
			Get();
			break;
		}
		case _realLiteral: {
			Get();
			break;
		}
		case _dateLiteral: {
			Get();
			if (la->kind == _timeLiteral) {
				Get();
			}
			break;
		}
		case _timeLiteral: {
			Get();
			break;
		}
		default: SynErr(265); break;
		}
}

void Parser::EnumDefinition() {
		printv(3, "EnumDefinition"); 
		Expect(103 /* "enum" */);
		if (la->kind == _plainIdentifier) {
			Get();
		}
		if (la->kind == 104 /* "step" */) {
			Get();
			if (la->kind == 78 /* "+" */ || la->kind == 105 /* "*" */) {
				if (la->kind == 78 /* "+" */) {
					Get();
				} else {
					Get();
				}
			}
			Expect(_decimalLiteral);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(266); Get();}
		Newline();
		EnumConstant();
		while (la->kind == _plainIdentifier) {
			EnumConstant();
		}
		Expect(_END_ENUM);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(267); Get();}
		Newline();
}

void Parser::ExitStatement() {
		printv(3, "ExitStatement"); 
		Expect(106 /* "exit" */);
		switch (la->kind) {
		case _DO: {
			Get();
			MustBeIn(_DO, L"EXIT DO outside DO"); 
			break;
		}
		case _FOR: {
			Get();
			MustBeIn(_FOR, L"EXIT FOR outside FOR"); 
			break;
		}
		case _FUNCTION: {
			Get();
			MustBeIn(_FUNCTION, L"EXIT FUNCTION outside FUNCTION"); 
			break;
		}
		case _METHOD: {
			Get();
			MustBeIn(_METHOD, L"EXIT METHOD outside METHOD"); 
			break;
		}
		case _PROPERTY: {
			Get();
			MustBeIn(_PROPERTY, L"EXIT PROPERTY outside PROPERTY"); 
			break;
		}
		case _SUB: {
			Get();
			MustBeIn(_SUB, L"EXIT SUB outside SUB"); 
			break;
		}
		case _WHILE: {
			Get();
			MustBeIn(_WHILE, L"EXIT WHILE outside WHILE"); 
			break;
		}
		default: SynErr(268); break;
		}
}

void Parser::FormalParamsEnclosed() {
		printv(3, "FormalParamsEnclosed"); 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		Expect(_leftParen);
		if (StartOf(12)) {
			FormalParameter(opt, by);
			while (WeakSeparator(_comma,12,33) ) {
				if (opt == opt_yes) opt = opt_warn; 
				by = by_any; 
				FormalParameter(opt, by);
			}
		}
		Expect(_rightParen);
}

void Parser::FormalParamsUnenclosed() {
		printv(3, "FormalParamsUnenclosed"); 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		FormalParameter(opt, by);
		while (WeakSeparator(_comma,12,46) ) {
			if (opt == opt_yes) opt = opt_warn; 
			by = by_any; 
			FormalParameter(opt, by);
		}
}

void Parser::OptionalParameters() {
		printv(3, "OptionalParameters"); 
		Expect(94 /* "optional" */);
		Expect(_leftParen);
		OptionalParameter();
		while (WeakSeparator(_comma,12,33) ) {
			OptionalParameter();
		}
		Expect(_rightParen);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(269); Get();}
		Newline();
}

void Parser::ProcMistake() {
		printv(3, "ProcMistake"); 
		switch (la->kind) {
		case 80 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 92 /* "catch" */: {
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
		case 93 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 94 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 95 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 96 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(270); break;
		}
		while (StartOf(47)) {
			Get();
		}
		Newline();
}

void Parser::GenericConstraint() {
		printv(3, "GenericConstraint"); 
		Expect(_plainIdentifier);
		if (la->kind == 87 /* "<" */ || la->kind == 88 /* "<=" */) {
			if (la->kind == 87 /* "<" */) {
				Get();
			} else {
				Get();
			}
			Expect(_plainIdentifier);
			if (la->kind == 87 /* "<" */ || la->kind == 88 /* "<=" */) {
				if (la->kind == 87 /* "<" */) {
					Get();
				} else {
					Get();
				}
				Expect(_plainIdentifier);
			}
			if (la->kind == 108 /* "does" */) {
				Get();
				Expect(_plainIdentifier);
			}
		} else if (la->kind == 108 /* "does" */) {
			Get();
			Expect(_plainIdentifier);
		} else SynErr(271);
}

void Parser::GotoStatement() {
		printv(3, "GotoStatement"); 
		Expect(109 /* "goto" */);
		MustBeIn(_SELECT, L"GOTO without SELECT"); 
		Expect(_CASE);
		if (StartOf(26)) {
			Expression();
		} else if (la->kind == _ELSE) {
			Get();
		} else SynErr(272);
}

void Parser::IdentifierExpression() {
		printv(2, "Identifier"); 
		if (la->kind == _plainIdentifier) {
			Get();
			if (la->kind == _dot || la->kind == _leftParen) {
				if (la->kind == _dot) {
					DotMember();
				} else if (IsVariable(t->val)) {
					Subscript();
				} else {
					Get();
					if (StartOf(22)) {
						ArgumentList();
					}
					Expect(_rightParen);
				}
			}
		} else if (la->kind == _typedIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				if (IsVariable(t->val)) {
					Subscript();
				} else {
					Get();
					if (StartOf(22)) {
						ArgumentList();
					}
					Expect(_rightParen);
				}
			}
		} else SynErr(273);
}

void Parser::Subscript() {
		printv(3, "Subscript"); 
		Expect(_leftParen);
		if (StartOf(19)) {
			EnclosedExpression();
			while (WeakSeparator(_comma,19,33) ) {
				EnclosedExpression();
			}
		}
		Expect(_rightParen);
}

void Parser::LibraryAttribute() {
		printv(3, "LibraryAttribute"); 
		Expect(_plainIdentifier);
		Expect(_equals);
		if (la->kind == _versionLiteral) {
			Get();
		} else if (la->kind == _stringLiteral) {
			Get();
		} else SynErr(274);
}

void Parser::RequireStatement() {
		printv(3, "RequireStatement"); 
		Expect(145 /* "require" */);
		Requirement();
		while (WeakSeparator(_comma,3,2) ) {
			Requirement();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(275); Get();}
		Newline();
}

void Parser::LibraryModuleDeclaration() {
		printv(3, "LibraryModuleDeclaration"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractClass();
			break;
		}
		case 81 /* "base" */: {
			BaseUnitDefinition();
			break;
		}
		case 82 /* "begin" */: {
			BeginStatement();
			break;
		}
		case _CLASS: {
			ClassDefinition(0);
			break;
		}
		case 101 /* "dim" */: case 102 /* "var" */: {
			DimStatement();
			break;
		}
		case 103 /* "enum" */: {
			EnumDefinition();
			break;
		}
		case _EVENT: {
			EventDefinition();
			break;
		}
		case _FUNCTION: {
			FunctionDefinition();
			break;
		}
		case _METHOD: {
			MethodDefinition();
			break;
		}
		case 121 /* "object" */: {
			ObjectDefinition();
			break;
		}
		case _PROPERTY: {
			PropertyDefinition();
			break;
		}
		case 148 /* "struct" */: {
			StructDefinition();
			break;
		}
		case _SUB: {
			SubDefinition();
			break;
		}
		case 151 /* "trait" */: {
			TraitDefinition();
			break;
		}
		case _UNIT: {
			UnitDefinition();
			break;
		}
		default: SynErr(276); break;
		}
}

void Parser::ModuleMistake() {
		printv(3, "ModuleMistake"); 
		switch (la->kind) {
		case 80 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 92 /* "catch" */: {
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
		case 93 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 94 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 95 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 96 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(277); break;
		}
		while (StartOf(47)) {
			Get();
		}
		Newline();
}

void Parser::StructDefinition() {
		printv(3, "StructDefinition"); 
		unsigned int members = 0; 
		Expect(148 /* "struct" */);
		Expect(_plainIdentifier);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(278); Get();}
		Newline();
		while (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			NarrowDeclaration();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(279); Get();}
			Newline();
			members++; 
		}
		if (members == 0) Err(L"STRUCT requires one or more members"); 
		Expect(_END_STRUCT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(280); Get();}
		Newline();
}

void Parser::TraitDefinition() {
		printv(3, "TraitDefinition"); 
		Expect(151 /* "trait" */);
		Expect(_plainIdentifier);
		if (la->kind == 108 /* "does" */) {
			Traits();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(281); Get();}
		Newline();
		while (la->kind == _METHOD) {
			MethodDefinition();
		}
		Expect(_END_TRAIT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(282); Get();}
		Newline();
}

void Parser::UnitDefinition() {
		printv(3, "UnitDefinition"); 
		Expect(_UNIT);
		if (la->kind == _leftBracket) {
			UnitParameter();
		}
		if (la->kind == 79 /* "-" */) {
			Get();
		}
		UnitAliases();
		if (la->kind == _equals) {
			Get();
			AdditiveExpression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(283); Get();}
		Newline();
}

void Parser::LogicalANDExpression() {
		printv(2, "LogicalAND"); 
		ComparisonExpression();
		while (la->kind == 111 /* "and" */ || la->kind == 112 /* "andthen" */) {
			if (la->kind == 111 /* "and" */) {
				Get();
			} else {
				Get();
			}
			ComparisonExpression();
		}
}

void Parser::LogicalORExpression() {
		printv(2, "LogicalOR"); 
		LogicalANDExpression();
		while (la->kind == 113 /* "or" */ || la->kind == 114 /* "orelse" */) {
			if (la->kind == 113 /* "or" */) {
				Get();
			} else {
				Get();
			}
			LogicalANDExpression();
		}
}

void Parser::MalformedToken() {
		printv(3, "MalformedToken"); 
		if (la->kind == _characterError) {
			Get();
			Err(L"Malformed character literal"); 
		} else if (la->kind == _stringError) {
			Get();
			Err(L"Malformed string literal"); 
		} else if (la->kind == _versionError) {
			Get();
			Err(L"Malformed version literal"); 
		} else SynErr(284);
}

void Parser::MethodCallStatement() {
		printv(3, "MethodCallStatement"); 
		if (la->kind == _objectIdentifier) {
			Get();
		} else if (la->kind == _boxedIdentifier) {
			Get();
		} else SynErr(285);
		if (la->kind == _dot) {
			Get();
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
				if (la->kind == _plainIdentifier) {
					Get();
				} else {
					Get();
				}
				if (la->kind == 116 /* "?" */) {
					Get();
				} else if (la->kind == _assignOp) {
					Get();
					Expression();
				} else if (la->kind == _equals) {
					Get();
					Expression();
				} else if (la->kind == _leftParen) {
					Get();
					if (StartOf(22)) {
						ArgumentList();
					}
					Expect(_rightParen);
				} else SynErr(286);
			} else if (la->kind == _objectIdentifier || la->kind == _boxedIdentifier) {
				MethodCallStatement();
			} else SynErr(287);
		} else if (StartOf(48)) {
			if (la->kind == 116 /* "?" */) {
				Get();
			}
			Expression();
		} else SynErr(288);
}

void Parser::PowerExpression() {
		printv(2, "Power"); 
		UnaryExpression();
		while (la->kind == 123 /* "^" */) {
			Get();
			UnaryExpression();
		}
}

void Parser::NarrowDeclaration() {
		printv(3, "NarrowDeclaration"); 
		if (la->kind == _plainIdentifier) {
			Get();
			DataTypeClause();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else SynErr(289);
}

void Parser::NewStatement() {
		printv(3, "NewStatement"); 
		Expect(120 /* "new" */);
		if (la->kind == _SHARED) {
			Get();
		}
		if (la->kind == _plainIdentifier) {
			ObjectInitializerStatement();
		} else if (la->kind == _leftBracket || la->kind == _objectIdentifier) {
			if (OutOfContext(_IS) || OutOfContext(_CONSTRUCTOR)) Err(L"Call to base constructor outside subclass constructor"); 
			if (la->kind == _leftBracket) {
				GenericUsage();
			}
			Expect(_objectIdentifier);
			if (wcscmp(t->val, L"#base") != 0) Err(L"#BASE required in call to base constructor"); 
			if (la->kind == _comma) {
				ExpectWeak(_comma, 24);
				ArgumentList();
			}
		} else SynErr(290);
}

void Parser::ObjectExpression() {
		printv(2, "Object"); 
		if (StartOf(49)) {
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
				if (la->kind == 116 /* "?" */) {
					Get();
				} else if (la->kind == _leftParen) {
					Get();
					if (StartOf(22)) {
						ArgumentList();
					}
					Expect(_rightParen);
				} else SynErr(291);
			}
			if (la->kind == _bang) {
				Get();
				DeclaredName();
			}
		} else if (la->kind == _hashNULL) {
			Get();
		} else if (la->kind == _nullAlias) {
			Get();
		} else SynErr(292);
}

void Parser::OptionalParameter() {
		printv(3, "OptionalParameter"); 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		FormalParameter(opt, by);
		if (la->kind == _equals) {
			Get();
			PrimaryExpression();
		}
}

void Parser::PrimaryExpression() {
		printv(2, "Primary"); 
		switch (la->kind) {
		case _plainIdentifier: case _typedIdentifier: {
			IdentifierExpression();
			break;
		}
		case _binaryLiteral: case _octalLiteral: case _decimalLiteral: case _hexadecimalLiteral: case _realLiteral: case _dateLiteral: case _timeLiteral: case _characterLiteral: {
			Number();
			break;
		}
		case _objectIdentifier: case _boxedIdentifier: case _nullAlias: case _hashBASE: case _hashNULL: case _hashSELF: {
			ObjectExpression();
			break;
		}
		case _stringLiteral: case _metastring: {
			String();
			break;
		}
		case _leftParen: {
			Get();
			EnclosedExpression();
			Expect(_rightParen);
			break;
		}
		case _versionError: case _characterError: case _stringError: {
			MalformedToken();
			break;
		}
		default: SynErr(293); break;
		}
}

void Parser::UnaryExpression() {
		printv(2, "Unary"); 
		if (la->kind == 78 /* "+" */ || la->kind == 79 /* "-" */ || la->kind == 152 /* "not" */) {
			if (la->kind == 78 /* "+" */) {
				Get();
			} else if (la->kind == 79 /* "-" */) {
				Get();
			} else {
				Get();
			}
		}
		PrimaryExpression();
}

void Parser::String() {
		printv(2, "String"); 
		int prev; 
		if (la->kind == _stringLiteral) {
			Get();
			if (t->val[0] == L'_') Err(L"String continuation without start"); 
		} else if (la->kind == _metastring) {
			Get();
		} else SynErr(294);
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
		while (la->kind == 111 /* "and" */ || la->kind == 113 /* "or" */) {
			if (la->kind == 111 /* "and" */) {
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
			if (la->kind == _equals || la->kind == 89 /* "<>" */) {
				if (la->kind == _equals) {
					Get();
				} else {
					Get();
				}
				if (la->kind == _versionLiteral) {
					Get();
				} else if (la->kind == _stringLiteral) {
					Get();
				} else SynErr(295);
			} else if (StartOf(50)) {
				if (la->kind == 87 /* "<" */) {
					Get();
				} else if (la->kind == 88 /* "<=" */) {
					Get();
				} else if (la->kind == 91 /* ">" */) {
					Get();
				} else {
					Get();
				}
				Expect(_versionLiteral);
			} else SynErr(296);
		} else if (la->kind == _versionLiteral) {
			Get();
			if (la->kind == 87 /* "<" */) {
				Get();
			} else if (la->kind == 88 /* "<=" */) {
				Get();
			} else SynErr(297);
			Expect(_plainIdentifier);
			if (la->kind == 87 /* "<" */) {
				Get();
			} else if (la->kind == 88 /* "<=" */) {
				Get();
			} else SynErr(298);
			Expect(_versionLiteral);
		} else SynErr(299);
}

void Parser::ReturnStatement() {
		printv(3, "ReturnStatement"); 
		Expect(146 /* "return" */);
		if (OutOfContext(_FUNCTION) && OutOfContext(_METHOD) && OutOfContext(_SUB) && OutOfContext(_PROPERTY))
		Err(L"RETURN outside procedure"); 
		if (StartOf(26)) {
			Expression();
		}
}

void Parser::ThrowStatement() {
		printv(3, "ThrowStatement"); 
		Expect(149 /* "throw" */);
		Expression();
}

void Parser::WaitStatement() {
		printv(3, "WaitStatement"); 
		Expect(153 /* "wait" */);
}

void Parser::UnitAlias() {
		printv(3, "UnitAlias"); 
		Expect(_plainIdentifier);
		if (la->kind == _leftParen) {
			Get();
			Expect(_plainIdentifier);
			Expect(_rightParen);
		}
		if (la->kind == 117 /* "/" */) {
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

void Parser::UserModuleDeclaration() {
		printv(3, "UserModuleDeclaration"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractClass();
			break;
		}
		case 81 /* "base" */: {
			BaseUnitDefinition();
			break;
		}
		case 82 /* "begin" */: {
			BeginStatement();
			break;
		}
		case _CLASS: {
			ClassDefinition(0);
			break;
		}
		case 101 /* "dim" */: case 102 /* "var" */: {
			DimStatement();
			break;
		}
		case 103 /* "enum" */: {
			EnumDefinition();
			break;
		}
		case _EVENT: {
			EventDefinition();
			break;
		}
		case 121 /* "object" */: {
			ObjectDefinition();
			break;
		}
		case 148 /* "struct" */: {
			StructDefinition();
			break;
		}
		case 151 /* "trait" */: {
			TraitDefinition();
			break;
		}
		case _UNIT: {
			UnitDefinition();
			break;
		}
		default: SynErr(300); break;
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
	maxT = 155;

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

	static bool set[51][157] = {
		{T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, T,x,T,x, x,T,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,T, x,x,T,T, x,T,x,x, x,T,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,T,T, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,T,x,T, x,T,x,x, x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, T,T,T,x, x,T,x,T, T,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, T,T,T,T, T,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,T,x,x, x,T,T,x, x,T,x,T, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,T, x,x,T,x, x,T,x,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,T,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,T,x,x, T,x,x,x, x,T,x,T, T,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, T,T,T,T, T,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, T,x,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, x,T,T,T, T,T,T,x, x,T,x,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,T,T,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, T,T,T,T, T,T,T,T, x,T,x,T, T,T,T,x, T,x,T,T, T,T,x,T, T,T,T,T, T,T,x,x, T,T,T,T, T,T,T,x, T,T,x,x, T,x,T,x, x,T,x,x, x,x,x,x, T,T,T,T, T,T,T,x, T,T,T,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,T, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,T, x,x,T,x, x,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,T,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x},
		{x,T,T,x, x,T,T,T, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x},
		{T,T,x,x, x,T,x,T, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,T,x,T, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,T,x,x, x,T,T,T, x,T,T,T, T,T,T,x, T,x,T,T, T,T,x,T, T,T,x,T, T,x,x,x, T,x,x,T, x,x,T,x, T,T,x,x, T,x,T,x, x,T,x,x, x,x,x,x, T,T,T,T, T,T,x,x, x,T,T,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, T,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,x,x, x,T,T,T, x,T,T,x, x,T,x,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,T,x, x,x,T,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,T,x,x, x},
		{x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x},
		{x,T,T,x, x,x,T,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,T,T, T,T,T,T, x,x,x,x, T,x,x,x, x,x,x,x, T,x,x,x, x,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,T,x, x,T,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,T,x,x, x,T,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x}
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
			case 13: s = coco_string_create(L"concatOp expected"); break;
			case 14: s = coco_string_create(L"binaryLiteral expected"); break;
			case 15: s = coco_string_create(L"octalLiteral expected"); break;
			case 16: s = coco_string_create(L"decimalLiteral expected"); break;
			case 17: s = coco_string_create(L"hexadecimalLiteral expected"); break;
			case 18: s = coco_string_create(L"versionLiteral expected"); break;
			case 19: s = coco_string_create(L"versionError expected"); break;
			case 20: s = coco_string_create(L"realLiteral expected"); break;
			case 21: s = coco_string_create(L"dateLiteral expected"); break;
			case 22: s = coco_string_create(L"timeLiteral expected"); break;
			case 23: s = coco_string_create(L"characterLiteral expected"); break;
			case 24: s = coco_string_create(L"characterError expected"); break;
			case 25: s = coco_string_create(L"stringLiteral expected"); break;
			case 26: s = coco_string_create(L"stringError expected"); break;
			case 27: s = coco_string_create(L"metastring expected"); break;
			case 28: s = coco_string_create(L"plainIdentifier expected"); break;
			case 29: s = coco_string_create(L"typedIdentifier expected"); break;
			case 30: s = coco_string_create(L"objectIdentifier expected"); break;
			case 31: s = coco_string_create(L"boxedIdentifier expected"); break;
			case 32: s = coco_string_create(L"nullAlias expected"); break;
			case 33: s = coco_string_create(L"hashBASE expected"); break;
			case 34: s = coco_string_create(L"hashNULL expected"); break;
			case 35: s = coco_string_create(L"hashSELF expected"); break;
			case 36: s = coco_string_create(L"ABSTRACT expected"); break;
			case 37: s = coco_string_create(L"CASE expected"); break;
			case 38: s = coco_string_create(L"CLASS expected"); break;
			case 39: s = coco_string_create(L"CONSTRUCTOR expected"); break;
			case 40: s = coco_string_create(L"DESTRUCTOR expected"); break;
			case 41: s = coco_string_create(L"DO expected"); break;
			case 42: s = coco_string_create(L"ELSE expected"); break;
			case 43: s = coco_string_create(L"ELSEIF expected"); break;
			case 44: s = coco_string_create(L"End expected"); break;
			case 45: s = coco_string_create(L"EndOfInitializer expected"); break;
			case 46: s = coco_string_create(L"END_CLASS expected"); break;
			case 47: s = coco_string_create(L"END_CONSTRUCTOR expected"); break;
			case 48: s = coco_string_create(L"END_DESTRUCTOR expected"); break;
			case 49: s = coco_string_create(L"END_ENUM expected"); break;
			case 50: s = coco_string_create(L"END_FOR expected"); break;
			case 51: s = coco_string_create(L"END_FUNCTION expected"); break;
			case 52: s = coco_string_create(L"END_IF expected"); break;
			case 53: s = coco_string_create(L"END_METHOD expected"); break;
			case 54: s = coco_string_create(L"END_OBJECT expected"); break;
			case 55: s = coco_string_create(L"END_PROPERTY expected"); break;
			case 56: s = coco_string_create(L"END_SELECT expected"); break;
			case 57: s = coco_string_create(L"END_STRUCT expected"); break;
			case 58: s = coco_string_create(L"END_SUB expected"); break;
			case 59: s = coco_string_create(L"END_TRAIT expected"); break;
			case 60: s = coco_string_create(L"END_TRY expected"); break;
			case 61: s = coco_string_create(L"END_WHILE expected"); break;
			case 62: s = coco_string_create(L"EVENT expected"); break;
			case 63: s = coco_string_create(L"FOR expected"); break;
			case 64: s = coco_string_create(L"FOR_EACH expected"); break;
			case 65: s = coco_string_create(L"FUNCTION expected"); break;
			case 66: s = coco_string_create(L"IN expected"); break;
			case 67: s = coco_string_create(L"IS expected"); break;
			case 68: s = coco_string_create(L"LOOP expected"); break;
			case 69: s = coco_string_create(L"METHOD expected"); break;
			case 70: s = coco_string_create(L"PROPERTY expected"); break;
			case 71: s = coco_string_create(L"SELECT expected"); break;
			case 72: s = coco_string_create(L"SHARED expected"); break;
			case 73: s = coco_string_create(L"SUB expected"); break;
			case 74: s = coco_string_create(L"TRY expected"); break;
			case 75: s = coco_string_create(L"UNIT expected"); break;
			case 76: s = coco_string_create(L"WHERE expected"); break;
			case 77: s = coco_string_create(L"WHILE expected"); break;
			case 78: s = coco_string_create(L"\"+\" expected"); break;
			case 79: s = coco_string_create(L"\"-\" expected"); break;
			case 80: s = coco_string_create(L"\"afterward\" expected"); break;
			case 81: s = coco_string_create(L"\"base\" expected"); break;
			case 82: s = coco_string_create(L"\"begin\" expected"); break;
			case 83: s = coco_string_create(L"\"shl\" expected"); break;
			case 84: s = coco_string_create(L"\"shr\" expected"); break;
			case 85: s = coco_string_create(L"\"call\" expected"); break;
			case 86: s = coco_string_create(L"\"to\" expected"); break;
			case 87: s = coco_string_create(L"\"<\" expected"); break;
			case 88: s = coco_string_create(L"\"<=\" expected"); break;
			case 89: s = coco_string_create(L"\"<>\" expected"); break;
			case 90: s = coco_string_create(L"\">=\" expected"); break;
			case 91: s = coco_string_create(L"\">\" expected"); break;
			case 92: s = coco_string_create(L"\"catch\" expected"); break;
			case 93: s = coco_string_create(L"\"finally\" expected"); break;
			case 94: s = coco_string_create(L"\"optional\" expected"); break;
			case 95: s = coco_string_create(L"\"otherwise\" expected"); break;
			case 96: s = coco_string_create(L"\"then\" expected"); break;
			case 97: s = coco_string_create(L"\"if\" expected"); break;
			case 98: s = coco_string_create(L"\"ctor\" expected"); break;
			case 99: s = coco_string_create(L"\"as\" expected"); break;
			case 100: s = coco_string_create(L"\"dtor\" expected"); break;
			case 101: s = coco_string_create(L"\"dim\" expected"); break;
			case 102: s = coco_string_create(L"\"var\" expected"); break;
			case 103: s = coco_string_create(L"\"enum\" expected"); break;
			case 104: s = coco_string_create(L"\"step\" expected"); break;
			case 105: s = coco_string_create(L"\"*\" expected"); break;
			case 106: s = coco_string_create(L"\"exit\" expected"); break;
			case 107: s = coco_string_create(L"\"byref\" expected"); break;
			case 108: s = coco_string_create(L"\"does\" expected"); break;
			case 109: s = coco_string_create(L"\"goto\" expected"); break;
			case 110: s = coco_string_create(L"\"library\" expected"); break;
			case 111: s = coco_string_create(L"\"and\" expected"); break;
			case 112: s = coco_string_create(L"\"andthen\" expected"); break;
			case 113: s = coco_string_create(L"\"or\" expected"); break;
			case 114: s = coco_string_create(L"\"orelse\" expected"); break;
			case 115: s = coco_string_create(L"\"xor\" expected"); break;
			case 116: s = coco_string_create(L"\"?\" expected"); break;
			case 117: s = coco_string_create(L"\"/\" expected"); break;
			case 118: s = coco_string_create(L"\"mod\" expected"); break;
			case 119: s = coco_string_create(L"\"rem\" expected"); break;
			case 120: s = coco_string_create(L"\"new\" expected"); break;
			case 121: s = coco_string_create(L"\"object\" expected"); break;
			case 122: s = coco_string_create(L"\"override\" expected"); break;
			case 123: s = coco_string_create(L"\"^\" expected"); break;
			case 124: s = coco_string_create(L"\"boolean\" expected"); break;
			case 125: s = coco_string_create(L"\"tiny\" expected"); break;
			case 126: s = coco_string_create(L"\"byte\" expected"); break;
			case 127: s = coco_string_create(L"\"char\" expected"); break;
			case 128: s = coco_string_create(L"\"string\" expected"); break;
			case 129: s = coco_string_create(L"\"short\" expected"); break;
			case 130: s = coco_string_create(L"\"ushort\" expected"); break;
			case 131: s = coco_string_create(L"\"integer\" expected"); break;
			case 132: s = coco_string_create(L"\"uinteger\" expected"); break;
			case 133: s = coco_string_create(L"\"single\" expected"); break;
			case 134: s = coco_string_create(L"\"int\" expected"); break;
			case 135: s = coco_string_create(L"\"uint\" expected"); break;
			case 136: s = coco_string_create(L"\"long\" expected"); break;
			case 137: s = coco_string_create(L"\"ulong\" expected"); break;
			case 138: s = coco_string_create(L"\"date\" expected"); break;
			case 139: s = coco_string_create(L"\"double\" expected"); break;
			case 140: s = coco_string_create(L"\"xfp\" expected"); break;
			case 141: s = coco_string_create(L"\"huge\" expected"); break;
			case 142: s = coco_string_create(L"\"uhuge\" expected"); break;
			case 143: s = coco_string_create(L"\"quad\" expected"); break;
			case 144: s = coco_string_create(L"\"on\" expected"); break;
			case 145: s = coco_string_create(L"\"require\" expected"); break;
			case 146: s = coco_string_create(L"\"return\" expected"); break;
			case 147: s = coco_string_create(L"\"let\" expected"); break;
			case 148: s = coco_string_create(L"\"struct\" expected"); break;
			case 149: s = coco_string_create(L"\"throw\" expected"); break;
			case 150: s = coco_string_create(L"\"tol\" expected"); break;
			case 151: s = coco_string_create(L"\"trait\" expected"); break;
			case 152: s = coco_string_create(L"\"not\" expected"); break;
			case 153: s = coco_string_create(L"\"wait\" expected"); break;
			case 154: s = coco_string_create(L"\"until\" expected"); break;
			case 155: s = coco_string_create(L"??? expected"); break;
			case 156: s = coco_string_create(L"invalid VF1"); break;
			case 157: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 158: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 159: s = coco_string_create(L"this symbol not expected in UserModule"); break;
			case 160: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 161: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 162: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 163: s = coco_string_create(L"invalid AbstractMember"); break;
			case 164: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 165: s = coco_string_create(L"invalid MethodSignature"); break;
			case 166: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 167: s = coco_string_create(L"invalid MethodSignature"); break;
			case 168: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 169: s = coco_string_create(L"this symbol not expected in PropertySignature"); break;
			case 170: s = coco_string_create(L"this symbol not expected in AfterwardClause"); break;
			case 171: s = coco_string_create(L"this symbol not expected in Statement"); break;
			case 172: s = coco_string_create(L"invalid Statement"); break;
			case 173: s = coco_string_create(L"invalid FormalParameter"); break;
			case 174: s = coco_string_create(L"invalid FormalParameter"); break;
			case 175: s = coco_string_create(L"invalid FormalParameter"); break;
			case 176: s = coco_string_create(L"invalid Argument"); break;
			case 177: s = coco_string_create(L"invalid VariableName"); break;
			case 178: s = coco_string_create(L"invalid ArgumentList"); break;
			case 179: s = coco_string_create(L"invalid ConditionalExpression"); break;
			case 180: s = coco_string_create(L"invalid AssignmentStatement"); break;
			case 181: s = coco_string_create(L"invalid Mutable"); break;
			case 182: s = coco_string_create(L"invalid Mutable"); break;
			case 183: s = coco_string_create(L"this symbol not expected in BaseUnitDefinition"); break;
			case 184: s = coco_string_create(L"this symbol not expected in BeginStatement"); break;
			case 185: s = coco_string_create(L"this symbol not expected in BeginStatement"); break;
			case 186: s = coco_string_create(L"this symbol not expected in BeginStatementMember"); break;
			case 187: s = coco_string_create(L"this symbol not expected in BeginStatementMember"); break;
			case 188: s = coco_string_create(L"invalid BeginStatementMember"); break;
			case 189: s = coco_string_create(L"invalid DeclaredName"); break;
			case 190: s = coco_string_create(L"invalid CaseExpression"); break;
			case 191: s = coco_string_create(L"invalid CaseExpression"); break;
			case 192: s = coco_string_create(L"invalid CaseStatement"); break;
			case 193: s = coco_string_create(L"this symbol not expected in CaseStatement"); break;
			case 194: s = coco_string_create(L"invalid ClassMember"); break;
			case 195: s = coco_string_create(L"invalid ClassMistake"); break;
			case 196: s = coco_string_create(L"invalid ConstructorDefinition"); break;
			case 197: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 198: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 199: s = coco_string_create(L"invalid DestructorDefinition"); break;
			case 200: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 201: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 202: s = coco_string_create(L"this symbol not expected in EventDefinition"); break;
			case 203: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 204: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 205: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 206: s = coco_string_create(L"this symbol not expected in MethodDefinition"); break;
			case 207: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 208: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 209: s = coco_string_create(L"invalid OverrideMember"); break;
			case 210: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 211: s = coco_string_create(L"invalid PropertyDefinition"); break;
			case 212: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 213: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 214: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 215: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 216: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 217: s = coco_string_create(L"invalid SharedMember"); break;
			case 218: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 219: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 220: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 221: s = coco_string_create(L"invalid GenericUsage"); break;
			case 222: s = coco_string_create(L"invalid GenericUsage"); break;
			case 223: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 224: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 225: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 226: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 227: s = coco_string_create(L"invalid CompoundDoStatement"); break;
			case 228: s = coco_string_create(L"invalid WhileOrUntil"); break;
			case 229: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 230: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 231: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 232: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 233: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 234: s = coco_string_create(L"invalid CompoundStatement"); break;
			case 235: s = coco_string_create(L"invalid DimStatement"); break;
			case 236: s = coco_string_create(L"this symbol not expected in DimStatement"); break;
			case 237: s = coco_string_create(L"this symbol not expected in DoStatement"); break;
			case 238: s = coco_string_create(L"invalid DoStatement"); break;
			case 239: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 240: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 241: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 242: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 243: s = coco_string_create(L"this symbol not expected in IfStatement"); break;
			case 244: s = coco_string_create(L"invalid IfStatement"); break;
			case 245: s = coco_string_create(L"invalid IfStatement"); break;
			case 246: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 247: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 248: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 249: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 250: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 251: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 252: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 253: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 254: s = coco_string_create(L"invalid ComparisonExpression"); break;
			case 255: s = coco_string_create(L"invalid Tolerance"); break;
			case 256: s = coco_string_create(L"invalid FormalParameters"); break;
			case 257: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 258: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 259: s = coco_string_create(L"invalid PrimitiveType"); break;
			case 260: s = coco_string_create(L"invalid Declarator"); break;
			case 261: s = coco_string_create(L"invalid Initializer"); break;
			case 262: s = coco_string_create(L"invalid SimpleStatement"); break;
			case 263: s = coco_string_create(L"invalid DotMember"); break;
			case 264: s = coco_string_create(L"this symbol not expected in EnumConstant"); break;
			case 265: s = coco_string_create(L"invalid Number"); break;
			case 266: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 267: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 268: s = coco_string_create(L"invalid ExitStatement"); break;
			case 269: s = coco_string_create(L"this symbol not expected in OptionalParameters"); break;
			case 270: s = coco_string_create(L"invalid ProcMistake"); break;
			case 271: s = coco_string_create(L"invalid GenericConstraint"); break;
			case 272: s = coco_string_create(L"invalid GotoStatement"); break;
			case 273: s = coco_string_create(L"invalid IdentifierExpression"); break;
			case 274: s = coco_string_create(L"invalid LibraryAttribute"); break;
			case 275: s = coco_string_create(L"this symbol not expected in RequireStatement"); break;
			case 276: s = coco_string_create(L"invalid LibraryModuleDeclaration"); break;
			case 277: s = coco_string_create(L"invalid ModuleMistake"); break;
			case 278: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 279: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 280: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 281: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 282: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 283: s = coco_string_create(L"this symbol not expected in UnitDefinition"); break;
			case 284: s = coco_string_create(L"invalid MalformedToken"); break;
			case 285: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 286: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 287: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 288: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 289: s = coco_string_create(L"invalid NarrowDeclaration"); break;
			case 290: s = coco_string_create(L"invalid NewStatement"); break;
			case 291: s = coco_string_create(L"invalid ObjectExpression"); break;
			case 292: s = coco_string_create(L"invalid ObjectExpression"); break;
			case 293: s = coco_string_create(L"invalid PrimaryExpression"); break;
			case 294: s = coco_string_create(L"invalid String"); break;
			case 295: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 296: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 297: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 298: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 299: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 300: s = coco_string_create(L"invalid UserModuleDeclaration"); break;

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


