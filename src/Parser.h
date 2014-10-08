
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
		_bang=4,
		_leftParen=5,
		_rightParen=6,
		_leftBracket=7,
		_rightBracket=8,
		_leftBrace=9,
		_rightBrace=10,
		_equals=11,
		_assignOp=12,
		_concatOp=13,
		_binaryLiteral=14,
		_octalLiteral=15,
		_decimalLiteral=16,
		_hexadecimalLiteral=17,
		_versionLiteral=18,
		_versionError=19,
		_realLiteral=20,
		_dateLiteral=21,
		_timeLiteral=22,
		_characterLiteral=23,
		_characterError=24,
		_stringLiteral=25,
		_stringError=26,
		_metastring=27,
		_plainIdentifier=28,
		_typedIdentifier=29,
		_objectIdentifier=30,
		_boxedIdentifier=31,
		_nullAlias=32,
		_ABSTRACT=33,
		_CASE=34,
		_CLASS=35,
		_CONSTRUCTOR=36,
		_DESTRUCTOR=37,
		_DO=38,
		_ELSE=39,
		_ELSEIF=40,
		_End=41,
		_EndOfInitializer=42,
		_END_CLASS=43,
		_END_CONSTRUCTOR=44,
		_END_DESTRUCTOR=45,
		_END_ENUM=46,
		_END_FOR=47,
		_END_FUNCTION=48,
		_END_IF=49,
		_END_METHOD=50,
		_END_OBJECT=51,
		_END_PROPERTY=52,
		_END_SELECT=53,
		_END_STRUCT=54,
		_END_SUB=55,
		_END_TRAIT=56,
		_END_TRY=57,
		_END_WHILE=58,
		_EVENT=59,
		_FOR=60,
		_FOR_EACH=61,
		_FUNCTION=62,
		_IN=63,
		_IS=64,
		_LOOP=65,
		_METHOD=66,
		_PROPERTY=67,
		_SELECT=68,
		_SHARED=69,
		_SUB=70,
		_TRY=71,
		_UNIT=72,
		_WHERE=73,
		_WHILE=74,
		_atCONST=153,
		_atIF=154,
		_atELSE=155,
		_atENDIF=156,
		_atERROR=157,
		_atWARN=158,
		_continuation=159,
		_comment=160,
		_atDEPRECATE=161
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

enum { elem_head = 1, elem_dtor = 2, elem_body = 4 };
enum opt_param { opt_yes, opt_no, opt_warn  };
enum pass_by { by_any, by_val, by_ref };
enum arg_kind { arg_any, arg_none, arg_pos, arg_named };

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
	return (la->kind == _plainIdentifier && ((next = scanner->Peek()->kind) == _objectIdentifier || next == _plainIdentifier || next == _leftBracket));
}

bool IsMethodCall() {
	int next = scanner->Peek()->kind;
	if (la->kind == _objectIdentifier || la->kind == _boxedIdentifier) {
		return (next == _dot || (next != _assignOp && next != _equals && next != _bang));
	}
	return false;
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
	void AbstractMember();
	void MethodSignature();
	void PropertySignature();
	void AdditiveExpression();
	void MultiplicativeExpression();
	void Unit();
	void AfterwardClause();
	void Newline();
	void Statement();
	void AnonFormalParameter();
	void FormalParameter(opt_param &opt, pass_by &by);
	void Argument(arg_kind &prev);
	void EnclosedExpression();
	void VariableName();
	void ArgumentList();
	void ArrayInitializer();
	void Expression();
	void AssignmentExpression();
	void ConditionalExpression();
	void AssignmentStatement();
	void Mutable();
	void BaseUnitDefinition();
	void BeginStatement();
	void BeginStatementMember();
	void ObjectInitializerStatement();
	void BitShiftExpression();
	void ConcatenativeExpression();
	void CallStatement();
	void DeclaredName();
	void CaseExpression();
	void CaseStatement(int &bCaseElse, int &line, int &col);
	void GenericDefinition();
	void Inheritance();
	void Traits();
	void GenericConstraints(bool isGeneric);
	void ConstructorDefinition(int &elems);
	void DestructorDefinition(int &elems);
	void FunctionDefinition();
	void MethodDefinition();
	void ObjectDefinition();
	void OverrideMember();
	void PropertyDefinition();
	void SharedMember(int &elems);
	void SubDefinition();
	void ClassMistake();
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
	void Declarator();
	void DeclaratorList();
	void SimpleStatement();
	void DotMember();
	void LogicalXORExpression();
	void EnumConstant();
	void Number();
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
	void LibraryAttribute();
	void RequireStatement();
	void LibraryModuleDeclaration();
	void ModuleMistake();
	void StructDefinition();
	void TraitDefinition();
	void UnitDefinition();
	void LogicalANDExpression();
	void LogicalORExpression();
	void MalformedToken();
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
	void UserModuleDeclaration();

	void Parse();

}; // end Parser



#endif

