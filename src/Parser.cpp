
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
		if (la->kind == 102 /* "library" */) {
			LibraryModule(ast);
		} else if (StartOf(1)) {
			UserModule(ast);
		} else if (la->kind == _EOF) {
			Warn(L"Module is empty."); 
		} else SynErr(151);
		Expect(_EOF);
		printf("\n-- %d %s\n", errors->count, (errors->count==1)?"error":"errors"); 
}

void Parser::LibraryModule(AST::Node *ast) {
		printv(3, "LibraryModule"); 
		Expect(102 /* "library" */);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,2) ) {
			LibraryAttribute();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(152); Get();}
		Newline();
		while (la->kind == _WHERE) {
			Get();
			LibraryAttribute();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(153); Get();}
			Newline();
		}
		while (la->kind == 140 /* "require" */) {
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
		while (la->kind == 140 /* "require" */) {
			RequireStatement();
		}
		while (StartOf(6)) {
			UserModuleDeclaration();
		}
		while (StartOf(7)) {
			Statement();
		}
		Expect(_End);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(154); Get();}
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
		if (la->kind == 100 /* "does" */) {
			Traits();
		}
		if (la->kind == _WHERE || la->kind == _newline && scanner->Peek()->kind == _WHERE) {
			GenericConstraints();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(155); Get();}
		Newline();
		int blocksSeen = 0; 
		while (StartOf(9)) {
			if (la->kind == _SHARED) {
				SharedMember(blocksSeen);
			} else if (la->kind == _PROPERTY) {
				PropertyDefinition();
			} else {
				Statement();
			}
		}
		while (la->kind == _CONSTRUCTOR || la->kind == 89 /* "ctor" */) {
			ConstructorDefinition(blocksSeen);
		}
		if (la->kind == _DESTRUCTOR || la->kind == 91 /* "dtor" */) {
			DestructorDefinition(blocksSeen);
		}
		while (StartOf(10)) {
			switch (la->kind) {
			case _ABSTRACT: {
				AbstractMember();
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
			case 117 /* "override" */: {
				OverrideMember();
				break;
			}
			case _PROPERTY: {
				PropertyDefinition();
				break;
			}
			case _SHARED: {
				SharedProcedure(blocksSeen);
				break;
			}
			case _SUB: {
				SubDefinition();
				break;
			}
			}
		}
		Expect(_END_CLASS);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(156); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::AbstractMember() {
		printv(3, "AbstractMember"); 
		Expect(_ABSTRACT);
		if (la->kind == _METHOD) {
			MethodSignature();
		} else if (la->kind == _PROPERTY) {
			PropertySignature();
		} else SynErr(157);
}

void Parser::MethodSignature() {
		printv(3, "MethodSignature"); 
		Expect(_METHOD);
		if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _plainHandle) {
			FunctionName();
			if (la->kind == _leftBracket) {
				GenericDefinition();
			}
			if (StartOf(11)) {
				if (la->kind == _leftParen) {
					FormalParamsEnclosed();
					if (la->kind == _IN || la->kind == 90 /* "as" */) {
						DataTypeClause();
					}
				} else if (StartOf(12)) {
					FormalParamsUnenclosed();
				} else {
					DataTypeClause();
				}
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(158); Get();}
			Newline();
			while (la->kind == 98 /* "optional" */) {
				OptionalParameters();
			}
		} else if (la->kind == _leftParen || la->kind == _leftBracket) {
			if (la->kind == _leftBracket) {
				GenericDefinition();
			}
			AnonFormalParameter();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(159); Get();}
			Newline();
		} else SynErr(160);
		if (la->kind == _newline || la->kind == _WHERE) {
			GenericConstraints();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(161); Get();}
			Expect(_newline);
		}
}

void Parser::PropertySignature() {
		printv(3, "PropertySignature"); 
		Expect(_PROPERTY);
		if (la->kind == _leftBracket) {
			GenericDefinition();
		}
		DimVariables();
		if (la->kind == _WHERE || la->kind == _newline && scanner->Peek()->kind == _WHERE) {
			GenericConstraints();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(162); Get();}
		Newline();
}

void Parser::ActualParameter() {
		printv(3, "ActualParameter"); 
		EnclosedExpression();
}

void Parser::EnclosedExpression() {
		printv(2, "Enclosed"); 
		LogicalXORExpression();
}

void Parser::ActualParameters() {
		printv(3, "ActualParameters"); 
		ActualParameter();
		while (WeakSeparator(_comma,14,13) ) {
			ActualParameter();
		}
}

void Parser::AdditiveExpression() {
		printv(2, "Additive"); 
		MultiplicativeExpression();
		while (la->kind == 72 /* "+" */ || la->kind == 73 /* "-" */) {
			if (la->kind == 72 /* "+" */) {
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
		while (StartOf(15)) {
			if (la->kind == 96 /* "*" */) {
				Get();
			} else if (la->kind == 112 /* "/" */) {
				Get();
			} else if (la->kind == 113 /* "mod" */) {
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
		if (la->kind == 112 /* "/" */) {
			Get();
			Expect(_plainIdentifier);
		}
}

void Parser::AfterwardClause() {
		printv(3, "AfterwardClause"); 
		Expect(74 /* "afterward" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(163); Get();}
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
		if (StartOf(16)) {
			SimpleStatement();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(164); Get();}
			Newline();
		} else if (StartOf(17)) {
			CompoundStatement();
		} else SynErr(165);
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
		if (la->kind == 98 /* "optional" */) {
			Get();
			if (opt == opt_no) Err(L"Parameter cannot be optional"); 
			else if (opt == opt_warn) Warn(L"OPTIONAL is redundant here"); 
			opt = opt_yes; 
		} else if (StartOf(18)) {
			opt = opt_no; 
		} else SynErr(166);
		if (la->kind == 99 /* "byref" */) {
			Get();
			if (by == by_val) Err(L"BYREF is not allowed here."); 
			by = by_ref; 
		} else if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _plainHandle) {
			if (by == by_ref) Err(L"BYREF is required here."); 
			by = by_val; 
		} else SynErr(167);
		if (la->kind == _typedIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				Get();
				Expect(_rightParen);
			}
		} else if (la->kind == _plainIdentifier) {
			Get();
			if (StartOf(19)) {
				if (la->kind == _IN || la->kind == 90 /* "as" */) {
					DataTypeClause();
				} else if (la->kind == _leftParen) {
					Get();
					Expect(_rightParen);
					if (la->kind == _IN || la->kind == 90 /* "as" */) {
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
					Expect(_plainHandle);
					if (la->kind == _leftParen) {
						Get();
						Expect(_rightParen);
					}
				}
			}
		} else if (la->kind == _plainHandle) {
			Get();
			if (la->kind == _leftParen) {
				Get();
				Expect(_rightParen);
			}
		} else SynErr(168);
}

void Parser::AnonMethodCall() {
		printv(3, "AnonMethodCall"); 
		#if 0 
		if (StartOf(20)) {
			Number();
		} else if (la->kind == _metastring) {
			Get();
		} else if (la->kind == _stringLiteral) {
			Get();
		} else if (la->kind == _leftParen) {
			Get();
		} else SynErr(169);
		#endif 
		Expression();
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
			break;
		}
		default: SynErr(170); break;
		}
}

void Parser::Expression() {
		printv(3, "Expression"); 
		AssignmentExpression();
		if (la->kind == _rightParen) { Get(); Err(L"Mismatched parentheses"); } 
}

void Parser::ArrayInitializer() {
		printv(3, "ArrayInitializer"); 
		if (StartOf(21)) {
			Expression();
		} else if (la->kind == _leftBrace) {
			Get();
			ArrayInitializer();
			while (WeakSeparator(_comma,23,22) ) {
				ArrayInitializer();
			}
			Expect(_rightBrace);
		} else SynErr(171);
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
		if (StartOf(14)) {
			EnclosedExpression();
		} else if (la->kind == 88 /* "if" */) {
			Get();
			EnclosedExpression();
			Expect(87 /* "then" */);
			EnclosedExpression();
			Expect(_ELSE);
			EnclosedExpression();
		} else SynErr(172);
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
		} else SynErr(173);
}

void Parser::Mutable() {
		printv(3, "Mutable"); 
		if (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			IdentifierExpression();
		} else if (la->kind == _plainHandle) {
			Get();
			if (la->kind == _dot || la->kind == _leftParen) {
				Subscript();
			}
		} else SynErr(174);
}

void Parser::BaseUnitDefinition() {
		printv(3, "BaseUnitDefinition"); 
		Expect(75 /* "base" */);
		Expect(_UNIT);
		Expect(_plainIdentifier);
		Expect(_IN);
		Unit();
		if (la->kind == _equals) {
			Get();
			MultiplicativeExpression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(175); Get();}
		Newline();
}

void Parser::BeginStatement() {
		printv(3, "BeginStatement"); 
		Expect(76 /* "begin" */);
		if (la->kind == _SHARED) {
			Get();
		}
		Expect(_plainIdentifier);
		Expect(_plainHandle);
		if (la->kind == _comma) {
			ExpectWeak(_comma, 24);
			ActualParameters();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(176); Get();}
		Newline();
		BeginStatementArray();
		Expect(_EndOfInitializer);
		Expect(_plainIdentifier);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(177); Get();}
		Newline();
}

void Parser::BeginStatementArray() {
		printv(3, "BeginStatementArray"); 
		if (IsObjectInitializer()) {
			ObjectInitializerStatement();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(178); Get();}
			Newline();
		} else if (StartOf(14)) {
			ActualParameters();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(179); Get();}
			Newline();
		} else SynErr(180);
		while (StartOf(14)) {
			if (IsObjectInitializer()) {
				ObjectInitializerStatement();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(181); Get();}
				Newline();
			} else {
				ActualParameters();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(182); Get();}
				Newline();
			}
		}
}

void Parser::ObjectInitializerStatement() {
		printv(3, "ObjectInitializerStatement"); 
		ClassType();
		Expect(_plainHandle);
		if (wcscmp(t->val, L"#base") == 0) Err(L"#BASE cannot be redefined. Perhaps you meant 'NEW #BASE'."); 
		if (la->kind == _comma) {
			ExpectWeak(_comma, 24);
			ActualParameters();
		}
}

void Parser::BitShiftExpression() {
		printv(2, "BitShift"); 
		ConcatenativeExpression();
		while (la->kind == 77 /* "shl" */ || la->kind == 78 /* "shr" */) {
			if (la->kind == 77 /* "shl" */) {
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
		Expect(79 /* "call" */);
		FunctionName();
		if (StartOf(14)) {
			ActualParameters();
		}
}

void Parser::FunctionName() {
		if (la->kind == _plainIdentifier) {
			printv(3, "FunctionName"); 
			Get();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else if (la->kind == _plainHandle) {
			Get();
		} else SynErr(183);
}

void Parser::CaseExpression() {
		printv(3, "CaseExpression"); 
		if (StartOf(21)) {
			Expression();
			if (la->kind == 80 /* "to" */) {
				Get();
				Expression();
			}
		} else if (la->kind == _IS) {
			Get();
			switch (la->kind) {
			case 81 /* "<" */: {
				Get();
				break;
			}
			case 82 /* "<=" */: {
				Get();
				break;
			}
			case _equals: {
				Get();
				break;
			}
			case 83 /* "<>" */: {
				Get();
				break;
			}
			case 84 /* ">=" */: {
				Get();
				break;
			}
			case 85 /* ">" */: {
				Get();
				break;
			}
			default: SynErr(184); break;
			}
			Expression();
		} else SynErr(185);
}

void Parser::CaseStatement(int &bCaseElse, int &line, int &col) {
		printv(3, "CaseStatement"); 
		Expect(_CASE);
		line = t->line; col = t->col; 
		if (StartOf(25)) {
			CaseExpression();
			while (WeakSeparator(_comma,25,2) ) {
				CaseExpression();
			}
		} else if (la->kind == _ELSE) {
			Get();
			bCaseElse = true; 
		} else SynErr(186);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(187); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
}

void Parser::GenericDefinition() {
		printv(3, "GenericDefinition"); 
		Expect(_leftBracket);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,26) ) {
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
		Expect(100 /* "does" */);
		Expect(_plainIdentifier);
		while (WeakSeparator(_comma,3,27) ) {
			Expect(_plainIdentifier);
		}
}

void Parser::GenericConstraints() {
		printv(3, "GenericConstraints"); 
		if (la->kind == _newline) {
			Get();
		}
		Expect(_WHERE);
		GenericConstraint();
		while (WeakSeparator(_comma,3,2) ) {
			GenericConstraint();
		}
}

void Parser::SharedMember(int &blocksSeen) {
		printv(3, "SharedMember"); 
		Expect(_SHARED);
		if (StartOf(28)) {
			blocksSeen |= seenSharedData; 
			if (la->kind == 76 /* "begin" */) {
				BeginStatement();
			} else if (la->kind == 92 /* "dim" */ || la->kind == 93 /* "var" */) {
				DimStatement();
			} else if (la->kind == 115 /* "new" */) {
				NewStatement();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(188); Get();}
				Newline();
			} else if (la->kind == 116 /* "object" */) {
				ObjectDefinition();
			} else if (IsObjectInitializer()) {
				ObjectInitializerStatement();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(189); Get();}
				Newline();
			} else {
				DimVariables();
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(190); Get();}
				Newline();
			}
		} else if (StartOf(29)) {
			blocksSeen |= seenSharedProc; 
			if (la->kind == _FUNCTION) {
				FunctionDefinition();
			} else if (la->kind == _METHOD) {
				MethodDefinition();
			} else if (la->kind == _PROPERTY) {
				PropertyDefinition();
			} else {
				SubDefinition();
			}
		} else SynErr(191);
}

void Parser::PropertyDefinition() {
		printv(3, "PropertyDefinition"); 
		PropertySignature();
		if (la->kind == _END_PROPERTY || la->kind == 139 /* "on" */) {
			Context foo(_PROPERTY); 
			bool fGetter = false, fSetter = false; 
			while (la->kind == 139 /* "on" */) {
				Get();
				Expect(_plainIdentifier);
				if (StartOf(30)) {
					if (fSetter) Err(L"Property already has a Setter."); else fSetter = true; 
					opt_param opt = opt_no; pass_by by = by_val; 
					if (StartOf(12)) {
						FormalParameter(opt, by);
					} else {
						Get();
						FormalParameter(opt, by);
						Expect(_rightParen);
					}
				} else if (la->kind == _newline) {
					if (fGetter) Err(L"Property already has a getter."); else fGetter = true; 
				} else SynErr(192);
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(193); Get();}
				Newline();
				while (StartOf(7)) {
					Statement();
				}
			}
			Expect(_END_PROPERTY);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(194); Get();}
			Newline();
		}
}

void Parser::ConstructorDefinition(int &blocksSeen) {
		printv(3, "ConstructorDefinition"); 
		Context foo(_CONSTRUCTOR); 
		if (la->kind == _CONSTRUCTOR) {
			Get();
		} else if (la->kind == 89 /* "ctor" */) {
			Get();
		} else SynErr(195);
		if ((blocksSeen & (seenSharedProc | seenCtorOrDtor)) == seenSharedProc) Err(L"Constructors and destructor must come before SHARED procedures."); 
		if (StartOf(30)) {
			FormalParameters();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(196); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		Expect(_END_CONSTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(197); Get();}
		Newline();
		blocksSeen |= seenCtorOrDtor; 
}

void Parser::DestructorDefinition(int &blocksSeen) {
		printv(3, "DestructorDefinition"); 
		Context foo(_DESTRUCTOR); 
		if (la->kind == _DESTRUCTOR) {
			Get();
		} else if (la->kind == 91 /* "dtor" */) {
			Get();
		} else SynErr(198);
		if ((blocksSeen & (seenSharedProc | seenCtorOrDtor)) == seenSharedProc) Err(L"Destructor must come before SHARED procedures."); 
		if (la->kind == _leftParen) {
			Get();
			Expect(_rightParen);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(199); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		Expect(_END_DESTRUCTOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(200); Get();}
		Newline();
		blocksSeen |= seenCtorOrDtor; 
}

void Parser::FunctionDefinition() {
		printv(3, "FunctionDefinition"); 
		Context foo(_FUNCTION); 
		Expect(_FUNCTION);
		FunctionName();
		if (la->kind == _leftBracket) {
			GenericDefinition();
		}
		if (StartOf(11)) {
			if (la->kind == _leftParen) {
				FormalParamsEnclosed();
				if (la->kind == _IN || la->kind == 90 /* "as" */) {
					DataTypeClause();
				}
			} else if (StartOf(12)) {
				FormalParamsUnenclosed();
			} else {
				DataTypeClause();
			}
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(201); Get();}
		Newline();
		while (la->kind == 98 /* "optional" */) {
			OptionalParameters();
		}
		if (la->kind == _newline || la->kind == _WHERE) {
			GenericConstraints();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(202); Get();}
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
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(203); Get();}
		Newline();
}

void Parser::MethodDefinition() {
		printv(3, "MethodDefinition"); 
		Context foo(_METHOD); 
		MethodSignature();
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
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(204); Get();}
		Newline();
}

void Parser::OverrideMember() {
		printv(3, "OverrideMember"); 
		Expect(117 /* "override" */);
		if (la->kind == _METHOD) {
			MethodDefinition();
		} else if (la->kind == _PROPERTY) {
			PropertyDefinition();
		} else SynErr(205);
}

void Parser::SharedProcedure(int &blocksSeen) {
		printv(3, "SharedProcedure"); 
		blocksSeen |= seenSharedProc; 
		Expect(_SHARED);
		if (la->kind == _FUNCTION) {
			FunctionDefinition();
		} else if (la->kind == _METHOD) {
			MethodDefinition();
		} else if (la->kind == _PROPERTY) {
			PropertyDefinition();
		} else if (la->kind == _SUB) {
			SubDefinition();
		} else SynErr(206);
}

void Parser::SubDefinition() {
		printv(3, "SubDefinition"); 
		Context foo(_SUB); 
		Expect(_SUB);
		FunctionName();
		if (la->kind == _leftBracket) {
			GenericDefinition();
		}
		if (StartOf(30)) {
			FormalParameters();
		}
		if (la->kind == 100 /* "does" */) {
			Get();
			Expect(_plainIdentifier);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(207); Get();}
		Newline();
		while (la->kind == 98 /* "optional" */) {
			OptionalParameters();
		}
		if (la->kind == _newline || la->kind == _WHERE) {
			GenericConstraints();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(208); Get();}
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
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(209); Get();}
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
		Expect(_plainIdentifier);
		while (la->kind == _comma) {
			Get();
			Expect(_plainIdentifier);
		}
		Expect(_rightBracket);
}

void Parser::CompoundDoStatement() {
		printv(3, "CompoundDoStatement"); 
		if (la->kind == _WHILE || la->kind == 149 /* "until" */) {
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(210); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
			if (la->kind == 74 /* "afterward" */) {
				AfterwardClause();
			}
			if (la->kind == 86 /* "otherwise" */) {
				OtherwiseClause();
			}
			Expect(_LOOP);
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(211); Get();}
			Newline();
		} else if (la->kind == _newline) {
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(212); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
			if (la->kind == 74 /* "afterward" */) {
				AfterwardClause();
			}
			if (la->kind == 86 /* "otherwise" */) {
				Get();
				Err(L"OTHERWISE not allowed in post-conditional DO"); 
				while (StartOf(32)) {
					Get();
				}
			}
			Expect(_LOOP);
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(213); Get();}
			Newline();
		} else SynErr(214);
}

void Parser::WhileOrUntil() {
		if (la->kind == _WHILE) {
			printv(3, "WhileOrUntil"); 
			Get();
		} else if (la->kind == 149 /* "until" */) {
			Get();
		} else SynErr(215);
}

void Parser::OtherwiseClause() {
		printv(3, "OtherwiseClause"); 
		Expect(86 /* "otherwise" */);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(216); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 74 /* "afterward" */) {
			Get();
			Err(L"AFTERWARD must precede OTHERWISE"); 
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(217); Get();}
			Newline();
		}
}

void Parser::CompoundIfStatement() {
		printv(3, "CompoundIfStatement"); 
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(218); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		while (la->kind == _ELSEIF) {
			Get();
			Expression();
			if (la->kind == 87 /* "then" */) {
				Get();
			}
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(219); Get();}
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
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(220); Get();}
		Newline();
}

void Parser::CompoundStatement() {
		printv(3, "CompoundStatement"); 
		switch (la->kind) {
		case 76 /* "begin" */: {
			BeginStatement();
			break;
		}
		case 92 /* "dim" */: case 93 /* "var" */: {
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
		case 88 /* "if" */: {
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
		default: SynErr(221); break;
		}
}

void Parser::DimStatement() {
		printv(3, "DimStatement"); 
		if (la->kind == 92 /* "dim" */) {
			Get();
		} else if (la->kind == 93 /* "var" */) {
			Get();
		} else SynErr(222);
		if (la->kind == _SHARED) {
			Get();
		}
		DimVariables();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(223); Get();}
		Newline();
}

void Parser::DoStatement() {
		printv(3, "DoStatement"); 
		Context foo(_DO); 
		Expect(_DO);
		if (la->kind == _newline || la->kind == _WHILE || la->kind == 149 /* "until" */) {
			CompoundDoStatement();
		} else if (StartOf(16)) {
			SimpleStatement();
			WhileOrUntil();
			Expression();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(224); Get();}
			Newline();
		} else SynErr(225);
}

void Parser::ForEachStatement() {
		printv(3, "ForEachStatement"); 
		Context foo(_FOR); 
		Expect(_FOR_EACH);
		if (la->kind == _plainIdentifier) {
			Get();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else if (la->kind == _plainHandle) {
			Get();
		} else SynErr(226);
		Expect(_IN);
		Expression();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(227); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 74 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 86 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_FOR);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(228); Get();}
		Newline();
}

void Parser::ForStatement() {
		printv(3, "ForStatement"); 
		Context foo(_FOR); 
		Expect(_FOR);
		Expect(_plainIdentifier);
		Expect(_equals);
		Expression();
		Expect(80 /* "to" */);
		Expression();
		if (la->kind == 95 /* "step" */) {
			Get();
			Expression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(229); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 74 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 86 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_FOR);
		if (la->kind == _plainIdentifier) {
			Get();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(230); Get();}
		Newline();
}

void Parser::IfStatement() {
		printv(3, "IfStatement"); 
		Expect(88 /* "if" */);
		Expression();
		if (la->kind == 87 /* "then" */) {
			Get();
			if (StartOf(16)) {
				SimpleStatement();
				if (la->kind == _ELSE) {
					Get();
					SimpleStatement();
				}
				while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(231); Get();}
				Newline();
			} else if (la->kind == _newline) {
				CompoundIfStatement();
			} else SynErr(232);
		} else if (la->kind == _newline) {
			CompoundIfStatement();
		} else SynErr(233);
}

void Parser::SelectStatement() {
		int bCaseElse = false, line, col, count = 1; 
		printv(3, "SelectStatement"); 
		Context foo(_SELECT); 
		Expect(_SELECT);
		if (la->kind == _CASE) {
			Get();
		}
		AdditiveExpression();
		if (la->kind == 145 /* "tol" */) {
			Tolerance();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(234); Get();}
		Newline();
		CaseStatement(bCaseElse, line, col);
		CheckCase(bCaseElse, line, col, count); 
		while (la->kind == _CASE) {
			CaseStatement(bCaseElse, line, col);
			CheckCase(bCaseElse, line, col, count); 
		}
		Expect(_END_SELECT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(235); Get();}
		Newline();
}

void Parser::TryStatement() {
		printv(3, "TryStatement"); 
		Context foo(_TRY); 
		Expect(_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(236); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		while (la->kind == 110 /* "catch" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(237); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		if (la->kind == 111 /* "finally" */) {
			Get();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(238); Get();}
			Newline();
			while (StartOf(7)) {
				Statement();
			}
		}
		Expect(_END_TRY);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(239); Get();}
		Newline();
}

void Parser::WhileStatement() {
		printv(3, "WhileStatement"); 
		Context foo(_WHILE); 
		Expect(_WHILE);
		Expression();
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(240); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		if (la->kind == 74 /* "afterward" */) {
			AfterwardClause();
		}
		if (la->kind == 86 /* "otherwise" */) {
			OtherwiseClause();
		}
		Expect(_END_WHILE);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(241); Get();}
		Newline();
}

void Parser::ComparisonExpression() {
		printv(2, "Comparison"); 
		unsigned int less = 0, notLess = 0; 
		BitShiftExpression();
		if (StartOf(33)) {
			while (StartOf(34)) {
				switch (la->kind) {
				case 81 /* "<" */: {
					Get();
					less++; 
					break;
				}
				case 82 /* "<=" */: {
					Get();
					less++; 
					break;
				}
				case 85 /* ">" */: {
					Get();
					notLess++; 
					break;
				}
				case 84 /* ">=" */: {
					Get();
					notLess++; 
					break;
				}
				case _equals: {
					Get();
					notLess++; 
					break;
				}
				case 83 /* "<>" */: {
					Get();
					notLess++; 
					break;
				}
				}
				BitShiftExpression();
			}
			if (la->kind == 145 /* "tol" */) {
				Tolerance();
			}
		} else if (la->kind == _IS) {
			Inheritance();
		} else SynErr(242);
}

void Parser::Tolerance() {
		printv(3, "Tolerance"); 
		Expect(145 /* "tol" */);
		if (StartOf(20)) {
			Number();
		} else if (la->kind == _plainIdentifier) {
			Get();
		} else SynErr(243);
}

void Parser::FormalParameters() {
		printv(3, "FormalParameters"); 
		if (la->kind == _leftParen) {
			FormalParamsEnclosed();
		} else if (StartOf(12)) {
			FormalParamsUnenclosed();
		} else SynErr(244);
}

void Parser::DataTypeClause() {
		printv(3, "DataTypeClause"); 
		if (la->kind == 90 /* "as" */) {
			Get();
			if (la->kind == _plainIdentifier) {
				ClassType();
			} else if (StartOf(35)) {
				PrimitiveType();
			} else SynErr(245);
		} else if (la->kind == _IN) {
			Get();
			Unit();
		} else SynErr(246);
}

void Parser::PrimitiveType() {
		printv(3, "PrimitiveType"); 
		switch (la->kind) {
		case 119 /* "boolean" */: {
			Get();
			break;
		}
		case 120 /* "tiny" */: {
			Get();
			break;
		}
		case 121 /* "byte" */: {
			Get();
			break;
		}
		case 122 /* "char" */: {
			Get();
			break;
		}
		case 123 /* "string" */: {
			Get();
			break;
		}
		case 124 /* "short" */: {
			Get();
			break;
		}
		case 125 /* "ushort" */: {
			Get();
			break;
		}
		case 126 /* "integer" */: {
			Get();
			break;
		}
		case 127 /* "uinteger" */: {
			Get();
			break;
		}
		case 128 /* "single" */: {
			Get();
			break;
		}
		case 129 /* "int" */: {
			Get();
			break;
		}
		case 130 /* "uint" */: {
			Get();
			break;
		}
		case 131 /* "long" */: {
			Get();
			break;
		}
		case 132 /* "ulong" */: {
			Get();
			break;
		}
		case 133 /* "date" */: {
			Get();
			break;
		}
		case 134 /* "double" */: {
			Get();
			break;
		}
		case 135 /* "xfp" */: {
			Get();
			break;
		}
		case 136 /* "huge" */: {
			Get();
			break;
		}
		case 137 /* "uhuge" */: {
			Get();
			break;
		}
		case 138 /* "quad" */: {
			Get();
			break;
		}
		default: SynErr(247); break;
		}
}

void Parser::DimVariables() {
		printv(3, "DimVariables"); 
		DimVariable();
		while (WeakSeparator(_comma,36,27) ) {
			DimVariable();
		}
}

void Parser::DimVariable() {
		printv(3, "DimVariable"); 
		if (la->kind == _plainIdentifier) {
			Get();
		} else if (la->kind == _typedIdentifier) {
			Get();
		} else if (la->kind == _plainHandle) {
			Get();
		} else SynErr(248);
		if (la->kind == _leftParen) {
			Get();
			if (StartOf(14)) {
				EnclosedExpression();
				while (la->kind == _comma) {
					Get();
					EnclosedExpression();
				}
			}
			Expect(_rightParen);
		}
		if (la->kind == _IN || la->kind == 90 /* "as" */) {
			DataTypeClause();
		}
		if (la->kind == _equals) {
			Get();
			ArrayInitializer();
		}
}

void Parser::SimpleStatement() {
		printv(3, "SimpleStatement"); 
		if (la->kind == 79 /* "call" */) {
			CallStatement();
		} else if (la->kind == 101 /* "goto" */) {
			GotoStatement();
		} else if (la->kind == 97 /* "exit" */) {
			ExitStatement();
		} else if (la->kind == 142 /* "let" */) {
			Get();
			AssignmentStatement();
		} else if (la->kind == 115 /* "new" */) {
			NewStatement();
		} else if (la->kind == 141 /* "return" */) {
			ReturnStatement();
		} else if (la->kind == 144 /* "throw" */) {
			ThrowStatement();
		} else if (la->kind == 148 /* "wait" */) {
			WaitStatement();
		} else if (IsObjectInitializer()) {
			ObjectInitializerStatement();
		} else if (IsMethodCall()) {
			MethodCallStatement();
		} else if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _plainHandle) {
			AssignmentStatement();
		} else SynErr(249);
}

void Parser::DotSubscript() {
		printv(3, "DotSubscript"); 
		Expect(_dot);
		Expect(_plainIdentifier);
		while (la->kind == _dot) {
			Get();
			Expect(_plainIdentifier);
		}
}

void Parser::LogicalXORExpression() {
		printv(2, "LogicalXOR"); 
		LogicalORExpression();
		while (la->kind == 107 /* "xor" */) {
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
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(250); Get();}
		Newline();
}

void Parser::EnumDefinition() {
		printv(3, "EnumDefinition"); 
		Expect(94 /* "enum" */);
		if (la->kind == _plainIdentifier) {
			Get();
		}
		if (la->kind == 95 /* "step" */) {
			Get();
			if (la->kind == 72 /* "+" */ || la->kind == 96 /* "*" */) {
				if (la->kind == 72 /* "+" */) {
					Get();
				} else {
					Get();
				}
			}
			Expect(_decimalLiteral);
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(251); Get();}
		Newline();
		EnumConstant();
		while (la->kind == _plainIdentifier) {
			EnumConstant();
		}
		Expect(_END_ENUM);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(252); Get();}
		Newline();
}

void Parser::EventDefinition() {
		printv(3, "EventDefinition"); 
		Context foo(_EVENT); 
		Expect(_EVENT);
		Expect(_plainIdentifier);
		if (StartOf(30)) {
			FormalParameters();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(253); Get();}
		Newline();
}

void Parser::ExitStatement() {
		printv(3, "ExitStatement"); 
		Expect(97 /* "exit" */);
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
		default: SynErr(254); break;
		}
}

void Parser::FormalParamsEnclosed() {
		printv(3, "FormalParamsEnclosed"); 
		opt_param opt = opt_yes; 
		pass_by by = by_any; 
		Expect(_leftParen);
		if (StartOf(12)) {
			FormalParameter(opt, by);
			while (WeakSeparator(_comma,12,37) ) {
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
		while (WeakSeparator(_comma,12,38) ) {
			if (opt == opt_yes) opt = opt_warn; 
			by = by_any; 
			FormalParameter(opt, by);
		}
}

void Parser::OptionalParameters() {
		printv(3, "OptionalParameters"); 
		Expect(98 /* "optional" */);
		Expect(_leftParen);
		OptionalParameter();
		while (WeakSeparator(_comma,12,37) ) {
			OptionalParameter();
		}
		Expect(_rightParen);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(255); Get();}
		Newline();
}

void Parser::ProcMistake() {
		printv(3, "ProcMistake"); 
		switch (la->kind) {
		case 74 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 110 /* "catch" */: {
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
		case 111 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 98 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 86 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 87 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(256); break;
		}
		while (StartOf(39)) {
			Get();
		}
		Newline();
}

void Parser::GenericConstraint() {
		printv(3, "GenericConstraint"); 
		Expect(_plainIdentifier);
		if (la->kind == 81 /* "<" */ || la->kind == 82 /* "<=" */) {
			if (la->kind == 81 /* "<" */) {
				Get();
			} else {
				Get();
			}
			Expect(_plainIdentifier);
			if (la->kind == 81 /* "<" */ || la->kind == 82 /* "<=" */) {
				if (la->kind == 81 /* "<" */) {
					Get();
				} else {
					Get();
				}
				Expect(_plainIdentifier);
			}
			if (la->kind == 100 /* "does" */) {
				Get();
				Expect(_plainIdentifier);
			}
		} else if (la->kind == 100 /* "does" */) {
			Get();
			Expect(_plainIdentifier);
		} else SynErr(257);
}

void Parser::GotoStatement() {
		printv(3, "GotoStatement"); 
		Expect(101 /* "goto" */);
		MustBeIn(_SELECT, L"GOTO without SELECT"); 
		Expect(_CASE);
		if (StartOf(21)) {
			Expression();
		} else if (la->kind == _ELSE) {
			Get();
		} else SynErr(258);
}

void Parser::IdentifierExpression() {
		printv(2, "Identifier"); 
		if (la->kind == _plainIdentifier) {
			Get();
			if (la->kind == _dot || la->kind == _leftParen) {
				if (IsVariable(t->val)) {
					Subscript();
				} else {
					Get();
					if (StartOf(14)) {
						ActualParameters();
					}
					Expect(_rightParen);
				}
			}
		} else if (la->kind == _typedIdentifier) {
			Get();
			if (la->kind == _leftParen) {
				if (IsVariable(t->val)) {
					ParenSubscript();
				} else {
					Get();
					if (StartOf(14)) {
						ActualParameters();
					}
					Expect(_rightParen);
				}
			}
		} else SynErr(259);
}

void Parser::Subscript() {
		if (la->kind == _dot) {
			printv(3, "Subscript"); 
			DotSubscript();
		} else if (la->kind == _leftParen) {
			ParenSubscript();
		} else SynErr(260);
}

void Parser::ParenSubscript() {
		printv(3, "ParenSubscript"); 
		Expect(_leftParen);
		if (StartOf(14)) {
			EnclosedExpression();
			while (WeakSeparator(_comma,14,37) ) {
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
		} else SynErr(261);
}

void Parser::RequireStatement() {
		printv(3, "RequireStatement"); 
		Expect(140 /* "require" */);
		Requirement();
		while (WeakSeparator(_comma,3,2) ) {
			Requirement();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(262); Get();}
		Newline();
}

void Parser::LibraryModuleDeclaration() {
		printv(3, "LibraryModuleDeclaration"); 
		switch (la->kind) {
		case _ABSTRACT: {
			AbstractClass();
			break;
		}
		case 75 /* "base" */: {
			BaseUnitDefinition();
			break;
		}
		case 76 /* "begin" */: {
			BeginStatement();
			break;
		}
		case _CLASS: {
			ClassDefinition(0);
			break;
		}
		case 92 /* "dim" */: case 93 /* "var" */: {
			DimStatement();
			break;
		}
		case 94 /* "enum" */: {
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
		case 116 /* "object" */: {
			ObjectDefinition();
			break;
		}
		case _PROPERTY: {
			PropertyDefinition();
			break;
		}
		case 143 /* "struct" */: {
			StructDefinition();
			break;
		}
		case _SUB: {
			SubDefinition();
			break;
		}
		case 146 /* "trait" */: {
			TraitDefinition();
			break;
		}
		case _UNIT: {
			UnitDefinition();
			break;
		}
		default: SynErr(263); break;
		}
}

void Parser::ModuleMistake() {
		printv(3, "ModuleMistake"); 
		switch (la->kind) {
		case 74 /* "afterward" */: {
			Get();
			Err(L"AFTERWARD without loop"); 
			break;
		}
		case _CASE: {
			Get();
			Err(L"CASE without SELECT"); 
			break;
		}
		case 110 /* "catch" */: {
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
		case 111 /* "finally" */: {
			Get();
			Err(L"FINALLY without TRY"); 
			break;
		}
		case _LOOP: {
			Get();
			Err(L"LOOP without DO"); 
			break;
		}
		case 98 /* "optional" */: {
			Get();
			Err(L"OPTIONAL not allowed here"); 
			break;
		}
		case 86 /* "otherwise" */: {
			Get();
			Err(L"OTHERWISE without loop"); 
			break;
		}
		case 87 /* "then" */: {
			Get();
			Err(L"THEN without IF"); 
			break;
		}
		case _WHERE: {
			Get();
			Err(L"WHERE not allowed here"); 
			break;
		}
		default: SynErr(264); break;
		}
		while (StartOf(39)) {
			Get();
		}
		Newline();
}

void Parser::ObjectDefinition() {
		printv(3, "ObjectDefinition"); 
		Context *subclass = NULL; 
		Expect(116 /* "object" */);
		Expect(_plainHandle);
		if (la->kind == _IS) {
			Inheritance();
			subclass = new Context(_IS); 
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(265); Get();}
		Newline();
		while (StartOf(7)) {
			Statement();
		}
		while (la->kind == _FUNCTION || la->kind == _METHOD || la->kind == _SUB) {
			if (la->kind == _FUNCTION) {
				FunctionDefinition();
			} else if (la->kind == _METHOD) {
				MethodDefinition();
			} else {
				SubDefinition();
			}
		}
		Expect(_END_OBJECT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(266); Get();}
		Newline();
		if (subclass) delete subclass; 
}

void Parser::StructDefinition() {
		printv(3, "StructDefinition"); 
		Expect(143 /* "struct" */);
		Expect(_plainIdentifier);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(267); Get();}
		Newline();
		while (la->kind == _plainIdentifier || la->kind == _typedIdentifier) {
			NarrowDeclaration();
			while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(268); Get();}
			Newline();
		}
		Expect(_END_STRUCT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(269); Get();}
		Newline();
}

void Parser::TraitDefinition() {
		printv(3, "TraitDefinition"); 
		Expect(146 /* "trait" */);
		Expect(_plainIdentifier);
		if (la->kind == _IS) {
			Inheritance();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(270); Get();}
		Newline();
		while (la->kind == _METHOD) {
			MethodDefinition();
		}
		Expect(_END_TRAIT);
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(271); Get();}
		Newline();
}

void Parser::UnitDefinition() {
		printv(3, "UnitDefinition"); 
		Expect(_UNIT);
		if (la->kind == _leftBracket) {
			UnitParameter();
		}
		if (la->kind == 73 /* "-" */) {
			Get();
		}
		UnitAliases();
		if (la->kind == _equals) {
			Get();
			AdditiveExpression();
		}
		while (!(la->kind == _EOF || la->kind == _newline)) {SynErr(272); Get();}
		Newline();
}

void Parser::LogicalANDExpression() {
		printv(2, "LogicalAND"); 
		ComparisonExpression();
		while (la->kind == 103 /* "and" */ || la->kind == 104 /* "andthen" */) {
			if (la->kind == 103 /* "and" */) {
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
		while (la->kind == 105 /* "or" */ || la->kind == 106 /* "orelse" */) {
			if (la->kind == 105 /* "or" */) {
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
		} else SynErr(273);
}

void Parser::MethodCall() {
		printv(3, "MethodCall"); 
		FunctionName();
		if (la->kind == 108 /* "?" */) {
			Get();
		} else if (la->kind == _leftParen) {
			Get();
			if (StartOf(14)) {
				ActualParameters();
			}
			Expect(_rightParen);
		} else SynErr(274);
}

void Parser::MethodCallStatement() {
		printv(3, "MethodCallStatement"); 
		Expect(_plainHandle);
		if (StartOf(40)) {
			if (la->kind == 109 /* "!" */) {
				Get();
			}
			if (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _plainHandle) {
				FunctionName();
				if (la->kind == 108 /* "?" */) {
					Get();
				} else if (la->kind == _assignOp) {
					Get();
					Expression();
				} else if (la->kind == _equals) {
					Get();
					Expression();
				} else if (la->kind == _leftParen) {
					Get();
					if (StartOf(14)) {
						ActualParameters();
					}
					Expect(_rightParen);
				} else SynErr(275);
			} else if (StartOf(41)) {
				AnonMethodCall();
			} else SynErr(276);
		} else if (la->kind == 108 /* "?" */) {
			Get();
			Expression();
		} else SynErr(277);
}

void Parser::PowerExpression() {
		printv(2, "Power"); 
		UnaryExpression();
		while (la->kind == 118 /* "^" */) {
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
		} else SynErr(278);
}

void Parser::NewStatement() {
		printv(3, "NewStatement"); 
		Expect(115 /* "new" */);
		if (la->kind == _SHARED) {
			Get();
		}
		if (la->kind == _plainIdentifier) {
			ObjectInitializerStatement();
		} else if (la->kind == _leftBracket || la->kind == _plainHandle) {
			if (OutOfContext(_IS) || OutOfContext(_CONSTRUCTOR)) Err(L"Call to base constructor outside subclass constructor"); 
			if (la->kind == _leftBracket) {
				GenericUsage();
			}
			Expect(_plainHandle);
			if (wcscmp(t->val, L"#base") != 0) Err(L"#BASE expected."); 
			if (la->kind == _comma) {
				ExpectWeak(_comma, 24);
				ActualParameters();
			}
		} else SynErr(279);
}

void Parser::ObjectExpression() {
		printv(2, "Object"); 
		if (la->kind == _plainHandle) {
			Get();
			while (la->kind == _plainIdentifier || la->kind == _typedIdentifier || la->kind == _plainHandle) {
				MethodCall();
			}
		} else if (la->kind == _nullAlias) {
			Get();
		} else SynErr(280);
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
		case _binaryLiteral: case _octalLiteral: case _decimalLiteral: case _hexadecimalLiteral: case _realLiteral: case _dateLiteral: case _characterLiteral: {
			Number();
			break;
		}
		case _plainHandle: case _nullAlias: {
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
		default: SynErr(281); break;
		}
}

void Parser::UnaryExpression() {
		printv(2, "Unary"); 
		if (la->kind == 72 /* "+" */ || la->kind == 73 /* "-" */ || la->kind == 147 /* "not" */) {
			if (la->kind == 72 /* "+" */) {
				Get();
			} else if (la->kind == 73 /* "-" */) {
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
		} else SynErr(282);
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
		if (la->kind == _plainHandle) {
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
		while (la->kind == 103 /* "and" */ || la->kind == 105 /* "or" */) {
			if (la->kind == 103 /* "and" */) {
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
			if (la->kind == _equals || la->kind == 83 /* "<>" */) {
				if (la->kind == _equals) {
					Get();
				} else {
					Get();
				}
				if (la->kind == _versionLiteral) {
					Get();
				} else if (la->kind == _stringLiteral) {
					Get();
				} else SynErr(283);
			} else if (StartOf(42)) {
				if (la->kind == 81 /* "<" */) {
					Get();
				} else if (la->kind == 82 /* "<=" */) {
					Get();
				} else if (la->kind == 85 /* ">" */) {
					Get();
				} else {
					Get();
				}
				Expect(_versionLiteral);
			} else SynErr(284);
		} else if (la->kind == _versionLiteral) {
			Get();
			if (la->kind == 81 /* "<" */) {
				Get();
			} else if (la->kind == 82 /* "<=" */) {
				Get();
			} else SynErr(285);
			Expect(_plainIdentifier);
			if (la->kind == 81 /* "<" */) {
				Get();
			} else if (la->kind == 82 /* "<=" */) {
				Get();
			} else SynErr(286);
			Expect(_versionLiteral);
		} else SynErr(287);
}

void Parser::ReturnStatement() {
		printv(3, "ReturnStatement"); 
		Expect(141 /* "return" */);
		if (OutOfContext(_FUNCTION) || OutOfContext(_METHOD) || OutOfContext(_SUB) || OutOfContext(_PROPERTY))
		Err(L"RETURN outside procedure"); 
		if (StartOf(21)) {
			Expression();
		}
}

void Parser::ThrowStatement() {
		printv(3, "ThrowStatement"); 
		Expect(144 /* "throw" */);
		Expression();
}

void Parser::WaitStatement() {
		printv(3, "WaitStatement"); 
		Expect(148 /* "wait" */);
}

void Parser::UnitAlias() {
		printv(3, "UnitAlias"); 
		Expect(_plainIdentifier);
		if (la->kind == _leftParen) {
			Get();
			Expect(_plainIdentifier);
			Expect(_rightParen);
		}
		if (la->kind == 112 /* "/" */) {
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
		case 75 /* "base" */: {
			BaseUnitDefinition();
			break;
		}
		case 76 /* "begin" */: {
			BeginStatement();
			break;
		}
		case _CLASS: {
			ClassDefinition(0);
			break;
		}
		case 92 /* "dim" */: case 93 /* "var" */: {
			DimStatement();
			break;
		}
		case 94 /* "enum" */: {
			EnumDefinition();
			break;
		}
		case _EVENT: {
			EventDefinition();
			break;
		}
		case 116 /* "object" */: {
			ObjectDefinition();
			break;
		}
		case 143 /* "struct" */: {
			StructDefinition();
			break;
		}
		case 146 /* "trait" */: {
			TraitDefinition();
			break;
		}
		case _UNIT: {
			UnitDefinition();
			break;
		}
		default: SynErr(288); break;
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
	maxT = 150;

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

	static bool set[43][152] = {
		{T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,T,x, T,x,x,T, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,T,x,x, T,T,x,T, x,x,x,T, T,x,x,T, x,x,x,x, x,x,x,x, T,x,x,x, T,T,T,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,x,T,x, T,x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, T,T,x,T, T,T,T,T, T,T,T,T, T,x,T,T, T,T,T,T, T,x,x,T, x,x,T,T, T,x,x,T, x,T,T,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, T,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,x,x,T, T,x,x,T, x,T,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,T,x,x, T,x,x,T, x,x,x,x, T,x,x,T, x,x,x,x, x,x,x,x, T,x,x,x, T,T,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,x,x,x, T,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, T,T,x,T, T,T,T,T, T,T,T,T, T,x,T,T, T,T,T,T, T,x,x,T, x,x,T,x, x,x,x,T, x,T,T,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, T,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, T,T,T,x, T,x,x,T, x,x,x,x, T,x,x,T, x,x,x,x, x,x,x,x, T,x,x,x, T,T,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,x,x,x, T,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,T, T,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,x,x,x, T,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,T,x,x, T,x,x,T, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,x,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,x,x, T,x,x,x, x,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x},
		{T,T,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x},
		{x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,T, T,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,T, x,x,x,T, T,T,x,T, T,T,T,T, T,x,T,x, T,x,T,T, x,T,T,T, x,T,T,x, x,x,T,x, x,T,x,x, T,x,T,T, x,x,T,x, T,x,x,T, x,x,x,x, x,x,T,T, T,x,x,x, T,T,x,x, x,T,T,x, x,T,x,x, x,x,x,x, x,x,T,T, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,x,x,x, T,x,x,x},
		{x,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x},
		{x,T,T,x, x,T,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, T,T,T,T, T,T,x,T, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,x,x},
		{x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,x,T, T,T,x,T, x,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, T,x,x,T, T,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x}
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
			case 4: s = coco_string_create(L"leftParen expected"); break;
			case 5: s = coco_string_create(L"rightParen expected"); break;
			case 6: s = coco_string_create(L"leftBracket expected"); break;
			case 7: s = coco_string_create(L"rightBracket expected"); break;
			case 8: s = coco_string_create(L"leftBrace expected"); break;
			case 9: s = coco_string_create(L"rightBrace expected"); break;
			case 10: s = coco_string_create(L"equals expected"); break;
			case 11: s = coco_string_create(L"assignOp expected"); break;
			case 12: s = coco_string_create(L"concatOp expected"); break;
			case 13: s = coco_string_create(L"binaryLiteral expected"); break;
			case 14: s = coco_string_create(L"octalLiteral expected"); break;
			case 15: s = coco_string_create(L"decimalLiteral expected"); break;
			case 16: s = coco_string_create(L"hexadecimalLiteral expected"); break;
			case 17: s = coco_string_create(L"versionLiteral expected"); break;
			case 18: s = coco_string_create(L"versionError expected"); break;
			case 19: s = coco_string_create(L"realLiteral expected"); break;
			case 20: s = coco_string_create(L"dateLiteral expected"); break;
			case 21: s = coco_string_create(L"characterLiteral expected"); break;
			case 22: s = coco_string_create(L"characterError expected"); break;
			case 23: s = coco_string_create(L"stringLiteral expected"); break;
			case 24: s = coco_string_create(L"stringError expected"); break;
			case 25: s = coco_string_create(L"metastring expected"); break;
			case 26: s = coco_string_create(L"plainIdentifier expected"); break;
			case 27: s = coco_string_create(L"typedIdentifier expected"); break;
			case 28: s = coco_string_create(L"plainHandle expected"); break;
			case 29: s = coco_string_create(L"nullAlias expected"); break;
			case 30: s = coco_string_create(L"ABSTRACT expected"); break;
			case 31: s = coco_string_create(L"CASE expected"); break;
			case 32: s = coco_string_create(L"CLASS expected"); break;
			case 33: s = coco_string_create(L"CONSTRUCTOR expected"); break;
			case 34: s = coco_string_create(L"DESTRUCTOR expected"); break;
			case 35: s = coco_string_create(L"DO expected"); break;
			case 36: s = coco_string_create(L"ELSE expected"); break;
			case 37: s = coco_string_create(L"ELSEIF expected"); break;
			case 38: s = coco_string_create(L"End expected"); break;
			case 39: s = coco_string_create(L"EndOfInitializer expected"); break;
			case 40: s = coco_string_create(L"END_CLASS expected"); break;
			case 41: s = coco_string_create(L"END_CONSTRUCTOR expected"); break;
			case 42: s = coco_string_create(L"END_DESTRUCTOR expected"); break;
			case 43: s = coco_string_create(L"END_ENUM expected"); break;
			case 44: s = coco_string_create(L"END_FOR expected"); break;
			case 45: s = coco_string_create(L"END_FUNCTION expected"); break;
			case 46: s = coco_string_create(L"END_IF expected"); break;
			case 47: s = coco_string_create(L"END_METHOD expected"); break;
			case 48: s = coco_string_create(L"END_OBJECT expected"); break;
			case 49: s = coco_string_create(L"END_PROPERTY expected"); break;
			case 50: s = coco_string_create(L"END_SELECT expected"); break;
			case 51: s = coco_string_create(L"END_STRUCT expected"); break;
			case 52: s = coco_string_create(L"END_SUB expected"); break;
			case 53: s = coco_string_create(L"END_TRAIT expected"); break;
			case 54: s = coco_string_create(L"END_TRY expected"); break;
			case 55: s = coco_string_create(L"END_WHILE expected"); break;
			case 56: s = coco_string_create(L"EVENT expected"); break;
			case 57: s = coco_string_create(L"FOR expected"); break;
			case 58: s = coco_string_create(L"FOR_EACH expected"); break;
			case 59: s = coco_string_create(L"FUNCTION expected"); break;
			case 60: s = coco_string_create(L"IN expected"); break;
			case 61: s = coco_string_create(L"IS expected"); break;
			case 62: s = coco_string_create(L"LOOP expected"); break;
			case 63: s = coco_string_create(L"METHOD expected"); break;
			case 64: s = coco_string_create(L"PROPERTY expected"); break;
			case 65: s = coco_string_create(L"SELECT expected"); break;
			case 66: s = coco_string_create(L"SHARED expected"); break;
			case 67: s = coco_string_create(L"SUB expected"); break;
			case 68: s = coco_string_create(L"TRY expected"); break;
			case 69: s = coco_string_create(L"UNIT expected"); break;
			case 70: s = coco_string_create(L"WHERE expected"); break;
			case 71: s = coco_string_create(L"WHILE expected"); break;
			case 72: s = coco_string_create(L"\"+\" expected"); break;
			case 73: s = coco_string_create(L"\"-\" expected"); break;
			case 74: s = coco_string_create(L"\"afterward\" expected"); break;
			case 75: s = coco_string_create(L"\"base\" expected"); break;
			case 76: s = coco_string_create(L"\"begin\" expected"); break;
			case 77: s = coco_string_create(L"\"shl\" expected"); break;
			case 78: s = coco_string_create(L"\"shr\" expected"); break;
			case 79: s = coco_string_create(L"\"call\" expected"); break;
			case 80: s = coco_string_create(L"\"to\" expected"); break;
			case 81: s = coco_string_create(L"\"<\" expected"); break;
			case 82: s = coco_string_create(L"\"<=\" expected"); break;
			case 83: s = coco_string_create(L"\"<>\" expected"); break;
			case 84: s = coco_string_create(L"\">=\" expected"); break;
			case 85: s = coco_string_create(L"\">\" expected"); break;
			case 86: s = coco_string_create(L"\"otherwise\" expected"); break;
			case 87: s = coco_string_create(L"\"then\" expected"); break;
			case 88: s = coco_string_create(L"\"if\" expected"); break;
			case 89: s = coco_string_create(L"\"ctor\" expected"); break;
			case 90: s = coco_string_create(L"\"as\" expected"); break;
			case 91: s = coco_string_create(L"\"dtor\" expected"); break;
			case 92: s = coco_string_create(L"\"dim\" expected"); break;
			case 93: s = coco_string_create(L"\"var\" expected"); break;
			case 94: s = coco_string_create(L"\"enum\" expected"); break;
			case 95: s = coco_string_create(L"\"step\" expected"); break;
			case 96: s = coco_string_create(L"\"*\" expected"); break;
			case 97: s = coco_string_create(L"\"exit\" expected"); break;
			case 98: s = coco_string_create(L"\"optional\" expected"); break;
			case 99: s = coco_string_create(L"\"byref\" expected"); break;
			case 100: s = coco_string_create(L"\"does\" expected"); break;
			case 101: s = coco_string_create(L"\"goto\" expected"); break;
			case 102: s = coco_string_create(L"\"library\" expected"); break;
			case 103: s = coco_string_create(L"\"and\" expected"); break;
			case 104: s = coco_string_create(L"\"andthen\" expected"); break;
			case 105: s = coco_string_create(L"\"or\" expected"); break;
			case 106: s = coco_string_create(L"\"orelse\" expected"); break;
			case 107: s = coco_string_create(L"\"xor\" expected"); break;
			case 108: s = coco_string_create(L"\"?\" expected"); break;
			case 109: s = coco_string_create(L"\"!\" expected"); break;
			case 110: s = coco_string_create(L"\"catch\" expected"); break;
			case 111: s = coco_string_create(L"\"finally\" expected"); break;
			case 112: s = coco_string_create(L"\"/\" expected"); break;
			case 113: s = coco_string_create(L"\"mod\" expected"); break;
			case 114: s = coco_string_create(L"\"rem\" expected"); break;
			case 115: s = coco_string_create(L"\"new\" expected"); break;
			case 116: s = coco_string_create(L"\"object\" expected"); break;
			case 117: s = coco_string_create(L"\"override\" expected"); break;
			case 118: s = coco_string_create(L"\"^\" expected"); break;
			case 119: s = coco_string_create(L"\"boolean\" expected"); break;
			case 120: s = coco_string_create(L"\"tiny\" expected"); break;
			case 121: s = coco_string_create(L"\"byte\" expected"); break;
			case 122: s = coco_string_create(L"\"char\" expected"); break;
			case 123: s = coco_string_create(L"\"string\" expected"); break;
			case 124: s = coco_string_create(L"\"short\" expected"); break;
			case 125: s = coco_string_create(L"\"ushort\" expected"); break;
			case 126: s = coco_string_create(L"\"integer\" expected"); break;
			case 127: s = coco_string_create(L"\"uinteger\" expected"); break;
			case 128: s = coco_string_create(L"\"single\" expected"); break;
			case 129: s = coco_string_create(L"\"int\" expected"); break;
			case 130: s = coco_string_create(L"\"uint\" expected"); break;
			case 131: s = coco_string_create(L"\"long\" expected"); break;
			case 132: s = coco_string_create(L"\"ulong\" expected"); break;
			case 133: s = coco_string_create(L"\"date\" expected"); break;
			case 134: s = coco_string_create(L"\"double\" expected"); break;
			case 135: s = coco_string_create(L"\"xfp\" expected"); break;
			case 136: s = coco_string_create(L"\"huge\" expected"); break;
			case 137: s = coco_string_create(L"\"uhuge\" expected"); break;
			case 138: s = coco_string_create(L"\"quad\" expected"); break;
			case 139: s = coco_string_create(L"\"on\" expected"); break;
			case 140: s = coco_string_create(L"\"require\" expected"); break;
			case 141: s = coco_string_create(L"\"return\" expected"); break;
			case 142: s = coco_string_create(L"\"let\" expected"); break;
			case 143: s = coco_string_create(L"\"struct\" expected"); break;
			case 144: s = coco_string_create(L"\"throw\" expected"); break;
			case 145: s = coco_string_create(L"\"tol\" expected"); break;
			case 146: s = coco_string_create(L"\"trait\" expected"); break;
			case 147: s = coco_string_create(L"\"not\" expected"); break;
			case 148: s = coco_string_create(L"\"wait\" expected"); break;
			case 149: s = coco_string_create(L"\"until\" expected"); break;
			case 150: s = coco_string_create(L"??? expected"); break;
			case 151: s = coco_string_create(L"invalid VF1"); break;
			case 152: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 153: s = coco_string_create(L"this symbol not expected in LibraryModule"); break;
			case 154: s = coco_string_create(L"this symbol not expected in UserModule"); break;
			case 155: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 156: s = coco_string_create(L"this symbol not expected in ClassDefinition"); break;
			case 157: s = coco_string_create(L"invalid AbstractMember"); break;
			case 158: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 159: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 160: s = coco_string_create(L"invalid MethodSignature"); break;
			case 161: s = coco_string_create(L"this symbol not expected in MethodSignature"); break;
			case 162: s = coco_string_create(L"this symbol not expected in PropertySignature"); break;
			case 163: s = coco_string_create(L"this symbol not expected in AfterwardClause"); break;
			case 164: s = coco_string_create(L"this symbol not expected in Statement"); break;
			case 165: s = coco_string_create(L"invalid Statement"); break;
			case 166: s = coco_string_create(L"invalid FormalParameter"); break;
			case 167: s = coco_string_create(L"invalid FormalParameter"); break;
			case 168: s = coco_string_create(L"invalid FormalParameter"); break;
			case 169: s = coco_string_create(L"invalid AnonMethodCall"); break;
			case 170: s = coco_string_create(L"invalid Number"); break;
			case 171: s = coco_string_create(L"invalid ArrayInitializer"); break;
			case 172: s = coco_string_create(L"invalid ConditionalExpression"); break;
			case 173: s = coco_string_create(L"invalid AssignmentStatement"); break;
			case 174: s = coco_string_create(L"invalid Mutable"); break;
			case 175: s = coco_string_create(L"this symbol not expected in BaseUnitDefinition"); break;
			case 176: s = coco_string_create(L"this symbol not expected in BeginStatement"); break;
			case 177: s = coco_string_create(L"this symbol not expected in BeginStatement"); break;
			case 178: s = coco_string_create(L"this symbol not expected in BeginStatementArray"); break;
			case 179: s = coco_string_create(L"this symbol not expected in BeginStatementArray"); break;
			case 180: s = coco_string_create(L"invalid BeginStatementArray"); break;
			case 181: s = coco_string_create(L"this symbol not expected in BeginStatementArray"); break;
			case 182: s = coco_string_create(L"this symbol not expected in BeginStatementArray"); break;
			case 183: s = coco_string_create(L"invalid FunctionName"); break;
			case 184: s = coco_string_create(L"invalid CaseExpression"); break;
			case 185: s = coco_string_create(L"invalid CaseExpression"); break;
			case 186: s = coco_string_create(L"invalid CaseStatement"); break;
			case 187: s = coco_string_create(L"this symbol not expected in CaseStatement"); break;
			case 188: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 189: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 190: s = coco_string_create(L"this symbol not expected in SharedMember"); break;
			case 191: s = coco_string_create(L"invalid SharedMember"); break;
			case 192: s = coco_string_create(L"invalid PropertyDefinition"); break;
			case 193: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 194: s = coco_string_create(L"this symbol not expected in PropertyDefinition"); break;
			case 195: s = coco_string_create(L"invalid ConstructorDefinition"); break;
			case 196: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 197: s = coco_string_create(L"this symbol not expected in ConstructorDefinition"); break;
			case 198: s = coco_string_create(L"invalid DestructorDefinition"); break;
			case 199: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 200: s = coco_string_create(L"this symbol not expected in DestructorDefinition"); break;
			case 201: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 202: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 203: s = coco_string_create(L"this symbol not expected in FunctionDefinition"); break;
			case 204: s = coco_string_create(L"this symbol not expected in MethodDefinition"); break;
			case 205: s = coco_string_create(L"invalid OverrideMember"); break;
			case 206: s = coco_string_create(L"invalid SharedProcedure"); break;
			case 207: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 208: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 209: s = coco_string_create(L"this symbol not expected in SubDefinition"); break;
			case 210: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 211: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 212: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 213: s = coco_string_create(L"this symbol not expected in CompoundDoStatement"); break;
			case 214: s = coco_string_create(L"invalid CompoundDoStatement"); break;
			case 215: s = coco_string_create(L"invalid WhileOrUntil"); break;
			case 216: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 217: s = coco_string_create(L"this symbol not expected in OtherwiseClause"); break;
			case 218: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 219: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 220: s = coco_string_create(L"this symbol not expected in CompoundIfStatement"); break;
			case 221: s = coco_string_create(L"invalid CompoundStatement"); break;
			case 222: s = coco_string_create(L"invalid DimStatement"); break;
			case 223: s = coco_string_create(L"this symbol not expected in DimStatement"); break;
			case 224: s = coco_string_create(L"this symbol not expected in DoStatement"); break;
			case 225: s = coco_string_create(L"invalid DoStatement"); break;
			case 226: s = coco_string_create(L"invalid ForEachStatement"); break;
			case 227: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 228: s = coco_string_create(L"this symbol not expected in ForEachStatement"); break;
			case 229: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 230: s = coco_string_create(L"this symbol not expected in ForStatement"); break;
			case 231: s = coco_string_create(L"this symbol not expected in IfStatement"); break;
			case 232: s = coco_string_create(L"invalid IfStatement"); break;
			case 233: s = coco_string_create(L"invalid IfStatement"); break;
			case 234: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 235: s = coco_string_create(L"this symbol not expected in SelectStatement"); break;
			case 236: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 237: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 238: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 239: s = coco_string_create(L"this symbol not expected in TryStatement"); break;
			case 240: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 241: s = coco_string_create(L"this symbol not expected in WhileStatement"); break;
			case 242: s = coco_string_create(L"invalid ComparisonExpression"); break;
			case 243: s = coco_string_create(L"invalid Tolerance"); break;
			case 244: s = coco_string_create(L"invalid FormalParameters"); break;
			case 245: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 246: s = coco_string_create(L"invalid DataTypeClause"); break;
			case 247: s = coco_string_create(L"invalid PrimitiveType"); break;
			case 248: s = coco_string_create(L"invalid DimVariable"); break;
			case 249: s = coco_string_create(L"invalid SimpleStatement"); break;
			case 250: s = coco_string_create(L"this symbol not expected in EnumConstant"); break;
			case 251: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 252: s = coco_string_create(L"this symbol not expected in EnumDefinition"); break;
			case 253: s = coco_string_create(L"this symbol not expected in EventDefinition"); break;
			case 254: s = coco_string_create(L"invalid ExitStatement"); break;
			case 255: s = coco_string_create(L"this symbol not expected in OptionalParameters"); break;
			case 256: s = coco_string_create(L"invalid ProcMistake"); break;
			case 257: s = coco_string_create(L"invalid GenericConstraint"); break;
			case 258: s = coco_string_create(L"invalid GotoStatement"); break;
			case 259: s = coco_string_create(L"invalid IdentifierExpression"); break;
			case 260: s = coco_string_create(L"invalid Subscript"); break;
			case 261: s = coco_string_create(L"invalid LibraryAttribute"); break;
			case 262: s = coco_string_create(L"this symbol not expected in RequireStatement"); break;
			case 263: s = coco_string_create(L"invalid LibraryModuleDeclaration"); break;
			case 264: s = coco_string_create(L"invalid ModuleMistake"); break;
			case 265: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 266: s = coco_string_create(L"this symbol not expected in ObjectDefinition"); break;
			case 267: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 268: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 269: s = coco_string_create(L"this symbol not expected in StructDefinition"); break;
			case 270: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 271: s = coco_string_create(L"this symbol not expected in TraitDefinition"); break;
			case 272: s = coco_string_create(L"this symbol not expected in UnitDefinition"); break;
			case 273: s = coco_string_create(L"invalid MalformedToken"); break;
			case 274: s = coco_string_create(L"invalid MethodCall"); break;
			case 275: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 276: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 277: s = coco_string_create(L"invalid MethodCallStatement"); break;
			case 278: s = coco_string_create(L"invalid NarrowDeclaration"); break;
			case 279: s = coco_string_create(L"invalid NewStatement"); break;
			case 280: s = coco_string_create(L"invalid ObjectExpression"); break;
			case 281: s = coco_string_create(L"invalid PrimaryExpression"); break;
			case 282: s = coco_string_create(L"invalid String"); break;
			case 283: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 284: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 285: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 286: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 287: s = coco_string_create(L"invalid RequirementRelation"); break;
			case 288: s = coco_string_create(L"invalid UserModuleDeclaration"); break;

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


