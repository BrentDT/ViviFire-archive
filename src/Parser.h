
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


#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include <stdio.h>
#include "Args.h"
#include "Context.h"
#include "AST.h"

extern Args args;


#include "Scanner.h"



class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
protected:
	enum {
		_EOF=0,
		_newline=1,
		_comma=2,
		_dot=3,
		_leftParen=4,
		_rightParen=5,
		_equals=6,
		_leftBracket=7,
		_rightBracket=8,
		_assignOp=9,
		_concatOp=10,
		_binaryLiteral=11,
		_octalLiteral=12,
		_decimalLiteral=13,
		_hexadecimalLiteral=14,
		_versionLiteral=15,
		_versionError=16,
		_realLiteral=17,
		_dateLiteral=18,
		_characterLiteral=19,
		_characterError=20,
		_stringLiteral=21,
		_stringError=22,
		_stringConstant=23,
		_plainIdentifier=24,
		_typedIdentifier=25,
		_plainHandle=26,
		_nullAlias=27,
		_ABSTRACT=28,
		_CASE=29,
		_CLASS=30,
		_CONSTRUCTOR=31,
		_DESTRUCTOR=32,
		_DO=33,
		_ELSE=34,
		_ELSEIF=35,
		_End=36,
		_EndOfInitializer=37,
		_END_CLASS=38,
		_END_CONSTRUCTOR=39,
		_END_DESTRUCTOR=40,
		_END_ENUM=41,
		_END_FOR=42,
		_END_FUNCTION=43,
		_END_IF=44,
		_END_METHOD=45,
		_END_OBJECT=46,
		_END_SELECT=47,
		_END_STRUCT=48,
		_END_SUB=49,
		_END_TRAIT=50,
		_END_TRY=51,
		_END_WHILE=52,
		_EVENT=53,
		_FOR=54,
		_FOR_EACH=55,
		_FUNCTION=56,
		_IN=57,
		_IS=58,
		_LOOP=59,
		_METHOD=60,
		_PUBLIC=61,
		_SELECT=62,
		_SHARED=63,
		_SUB=64,
		_TRY=65,
		_UNIT=66,
		_WHERE=67,
		_WHILE=68,
		_atCONST=148,
		_atIF=149,
		_atELSE=150,
		_atENDIF=151,
		_atERROR=152,
		_atWARN=153,
		_continuation=154,
		_comment=155
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

enum { seenSharedData = 1, seenSharedProc = 2, seenCtorOrDtor = 4 };

inline void printv(int v, const char *s) {
	if (args.v >= v) printf(">%s", s);
}

inline void MustBeIn(int kind, const wchar_t *msg) {
	if (OutOfContext(kind)) Err(msg);
}

inline void MustNotBeIn(int kind, const wchar_t *msg) {
	if (!OutOfContext(kind)) Err(msg);
}

void Err(const wchar_t *msg) {
	errors->Error(t->line, t->col, msg);
}

void Warn(const wchar_t *msg) {
	errors->Warning(msg);
}

bool IsVariable(const wchar_t *name) {
	// Returns true if name begins w/o "f" or "#f"
	// TODO: check for real
	bool var = true;
	if (name[0] == L'#') { if (name[1] == L'f') var = false; }
	else if (name[0] == L'f') var = false;
	printv(2, var?"variable":"function");
	return var;
}

bool IsObjectInitializer() {
	int next;
	return (la->kind == _plainIdentifier && ((next = scanner->Peek()->kind) == _plainHandle || next == _plainIdentifier || next == _leftBracket));
}

bool IsMethodCall() {
	int next;
	if (la->kind == _plainHandle) {
		return ((next = scanner->Peek()->kind) != _assignOp && next != _equals); // no assignment?
	}
	else return false;
}

void CheckCase(int bCaseElse, int line, int col, int &count) {
// Checks the validity of Cases, making sure there's at least one testable Case
// and that any Case Else comes last.
// 'line' and 'col' are the position of the preceding Case keyword.
// The 'count' var must be initialized to 1.
	if (count == 0) { // A Case after Case Else
		SynErr(_END_SELECT); // Expected End Select.
	}
	else if (bCaseElse) { // Case Else
		if (count == 1) {
			errors->Error(line, col, L"Case Else cannot be first Case");
			count++;
		}
		else count = 0; // After Case Else, don't expect any more.
	}
	else count++;
}

#if 0
class CCSymTable {
};

struct PPToken {
  enum { eEOF, eNot, eAnd, eOr, eIdent, eError };
  int kind;
  wchar_t *val;
  PPToken(): kind(-1), val(NULL) {}
  friend std::wostream & operator<< (std::wostream &out, PPToken &ppt) {
    switch (ppt.kind) {
      case PPToken::eEOF: out << L"end of file"; break;
      case PPToken::eNot: out << L"Not"; break;
      case PPToken::eAnd: out << L"And"; break;
      case PPToken::eOr: out << L"Or"; break;
      case PPToken::eIdent: out << L"identifier"; break;
      case PPToken::eError: out << L"error"; break;
      default: out << L"unknown token"; break;
    }
    return out;
  }
};

class PPScanner {
  wchar_t *m_chars;
  int m_pos;
  PPToken *m_next;
  int m_char;
public:
  PPScanner(wchar_t *str): m_chars(str), m_pos(0), m_next(NULL) {
    nextCh();
  }
private:
  void nextCh() {
    if (m_chars[m_pos] >= L' ') {
      m_char = int(m_chars[m_pos]);
      ++m_pos;
    } else {
      m_char = -1;
    }
  }
  int readIdent() {
    int start = m_pos;
    do {
      nextCh();
    } while (m_char != -1 && !iswspace(m_char));

    return start;
  }
  PPToken *scan() {
    PPToken *t = new PPToken();
    switch (m_char) {
      case L'@': break;
      case -1:
        t->kind = PPToken::eEOF;
        break;
      default:
        int start = readIdent();
        size_t len = m_pos - start;
        if (len == 3 && _wcsnicmp(L"and", &m_chars[start], 3) == 0) t->kind = PPToken::eAnd;
        else if (len == 3 && _wcsnicmp(L"not", &m_chars[start], 3) == 0) t->kind = PPToken::eNot;
        else if (len == 2 && _wcsnicmp(L"or", &m_chars[start], 2) == 0) t->kind = PPToken::eOr;
        else t->kind = PPToken::eIdent;
        break;
    }
  }
};

bool PPLogicalExpression(PPScanner &scan) {
  bool e1, e2;

  e1 = PPUnaryExpression(scan);
  while (int k = scan.peek().kind, k == PPToken::eAnd || k == PPToken::eOr) {
    scan.next();
    e2 = PPUnaryExpression(scan);
    e1 = e1 || e2;
  }

  return e1;
}

bool PPUnaryExpression(PPScanner &scan) {
  if (scan.peek().kind == PPToken::eNot) {
    scan.next();
    return !PPPrimaryExpression(scan);
  }
  return PPPrimaryExpression(scan);
}

bool PPPrimaryExpression(PPScanner &scan) {
  PPToken & ppt = scan.next();
  if (ppt.kind == PPToken::eIdent) {
    return 
  } else {
  }
}
#endif



	Parser(Scanner *scanner);
	virtual ~Parser();
	void SemErr(const wchar_t* msg);

	void VF1();
	void LibraryModule(AST::Node *ast);
	void UserModule(AST::Node *ast);
	void AbstractClass();
	void ClassDefinition(int abstract);
	void AbstractMethod();
	void MethodSignature();
	void ActualParameter();
	void EnclosedExpression();
	void ActualParameters();
	void AdditiveExpression();
	void MultiplicativeExpression();
	void Unit();
	void AfterwardClause();
	void Newline();
	void Statement();
	void AnonFormalParameter();
	void FormalParameter(bool &optional);
	void AnonMethodCall();
	void Number();
	void Expression();
	void ArrayInitializer();
	void AssignmentExpression();
	void ConditionalExpression();
	void AssignmentStatement();
	void Mutable();
	void BaseUnitDefinition();
	void BeginStatement();
	void BeginStatementArray();
	void ObjectInitializerStatement();
	void BitShiftExpression();
	void ConcatenativeExpression();
	void CallStatement();
	void FunctionName();
	void CaseExpression();
	void CaseStatement(int &bCaseElse, int &line, int &col);
	void GenericDefinition();
	void Inheritance();
	void Traits();
	void GenericConstraints();
	void SharedMember(int &blocksSeen);
	void ConstructorDefinition(int &blocksSeen);
	void DestructorDefinition(int &blocksSeen);
	void FunctionDefinition();
	void MethodDefinition();
	void OverrideMethod();
	void SharedProcedure(int &blocksSeen);
	void SubDefinition();
	void ClassType();
	void GenericUsage();
	void CompoundDoStatement();
	void WhileOrUntil();
	void OtherwiseClause();
	void CompoundIfStatement();
	void CompoundStatement();
	void DimStatement();
	void DoStatement();
	void ForEachStatement();
	void ForStatement();
	void IfStatement();
	void SelectStatement();
	void TryStatement();
	void WhileStatement();
	void ComparisonExpression();
	void Tolerance();
	void FormalParameters();
	void DataTypeClause();
	void PrimitiveType();
	void DimVariable();
	void SimpleStatement();
	void DotSubscript();
	void LogicalXORExpression();
	void EnumConstant();
	void EnumDefinition();
	void EventDefinition();
	void ExitStatement();
	void FormalParamsEnclosed();
	void FormalParamsUnenclosed();
	void OptionalParameters();
	void ProcMistake();
	void GenericConstraint();
	void GotoStatement();
	void IdentifierExpression();
	void Subscript();
	void ParenSubscript();
	void LibraryAttribute();
	void RequireStatement();
	void ObjectDefinition();
	void StructDefinition();
	void TraitDefinition();
	void UnitDefinition();
	void ModuleMistake();
	void LogicalANDExpression();
	void LogicalORExpression();
	void MalformedToken();
	void MethodCall();
	void MethodCallStatement();
	void PowerExpression();
	void NarrowDeclaration();
	void NewStatement();
	void ObjectExpression();
	void OptionalParameter();
	void PrimaryExpression();
	void UnaryExpression();
	void String();
	void Requirement();
	void RequirementRestriction();
	void RequirementRelation();
	void ReturnStatement();
	void ThrowStatement();
	void WaitStatement();
	void UnitAlias();
	void UnitAliases();
	void UnitParameter();

	void Parse();

}; // end Parser



#endif
