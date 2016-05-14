
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

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include <stdio.h>
#include "Args.h"
#include "Context.h"
#include "CodeGen.h"

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
private:
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
		_concatAssign=13,
		_andAssign=14,
		_orAssign=15,
		_xorAssign=16,
		_shlAssign=17,
		_shrAssign=18,
		_addAssign=19,
		_subAssign=20,
		_mulAssign=21,
		_divAssign=22,
		_modAssign=23,
		_remAssign=24,
		_concatOp=25,
		_binaryLiteral=26,
		_octalLiteral=27,
		_decimalLiteral=28,
		_hexadecimalLiteral=29,
		_versionLiteral=30,
		_versionError=31,
		_realLiteral=32,
		_dateLiteral=33,
		_timeLiteral=34,
		_characterLiteral=35,
		_stringLiteral=36,
		_stringError=37,
		_metastring=38,
		_plainIdentifier=39,
		_typedIdentifier=40,
		_objectIdentifier=41,
		_boxedIdentifier=42,
		_nullAlias=43,
		_hashBASE=44,
		_hashNULL=45,
		_hashSELF=46,
		_ABSTRACT=47,
		_CASE=48,
		_CLASS=49,
		_CONSTRUCTOR=50,
		_DESTRUCTOR=51,
		_DO=52,
		_DOES=53,
		_ELSE=54,
		_ELSEIF=55,
		_End=56,
		_EndOfInitializer=57,
		_END_CLASS=58,
		_END_CONSTRUCTOR=59,
		_END_DESTRUCTOR=60,
		_END_ENUM=61,
		_END_FLAGS=62,
		_END_FOR=63,
		_END_FUNCTION=64,
		_END_IF=65,
		_END_METHOD=66,
		_END_NEW=67,
		_END_OBJECT=68,
		_END_PROPERTY=69,
		_END_SELECT=70,
		_END_STRUCT=71,
		_END_SUB=72,
		_END_TRAIT=73,
		_END_TRY=74,
		_END_WHILE=75,
		_EVENT=76,
		_FOR=77,
		_FOR_EACH=78,
		_FUNCTION=79,
		_IN=80,
		_IS=81,
		_LOOP=82,
		_METHOD=83,
		_PROPERTY=84,
		_SELECT=85,
		_SHARED=86,
		_SUB=87,
		_TRY=88,
		_UNIT=89,
		_WHERE=90,
		_WHILE=91,
		_atCONST=173,
		_atDEPRECATE=174,
		_atIF=175,
		_atELSE=176,
		_atENDIF=177,
		_atERROR=178,
		_atWARN=179,
		_continuation=180,
		_comment=181
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
enum primitive_limit { primitive_any, primitive_char, primitive_fp, primitive_int, primitive_misc };

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
		return (next == _dot || ((next < _beginAssignOps || next > _endAssignOps) && next != _bang));
	}
	return false;
}

void CheckCase(int bCaseElse, int line, int col, int &cases) {
// Checks the validity of Cases, making sure there's at least one testable Case
// and that any Case Else comes last.
// 'line' and 'col' are the position of the preceding Case keyword.
// The 'cases' var must be initialized to zero.
	if (cases == -1) { // A Case after Case Else
		SynErr(_END_SELECT); // Expected End Select.
	}
	else if (bCaseElse) { // Case Else
		if (cases == 0) {
			errors->Error(line, col, L"Case Else cannot be first Case");
			cases++;
		}
		else cases = -1; // After Case Else, don't expect any more.
	}
	else cases++;
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
	~Parser();
	void SemErr(const wchar_t* msg);

	void VF1();
	void LibraryModule(AST::Library *lib);
	void UserModule(AST::UserModule *um);
	void AbstractClass(AST::Class *s);
	void ClassDefinition(AST::Class *block, int abstract);
	void AbstractMember(AST::Statement *s);
	void MethodSignature(bool &isBodiless);
	void PropertySignature();
	void AdditiveExpression(AST::Expression *e);
	void MultiplicativeExpression(AST::Expression *e);
	void AdditiveOp(AST::op_type &op);
	void Unit();
	void AfterwardClause(AST::Block *block);
	void Newline();
	void Statement(AST::Statement *s);
	void AnonMethodParameter(AST::Parameter *p);
	void Parameter(AST::Parameter *p, opt_param &opt, pass_by &by);
	void Argument(arg_kind &prev);
	void EnclosedExpression(AST::Expression *e);
	void VariableName();
	void ArgumentList();
	void AssignmentExpression(AST::Expression *e);
	void ConditionalExpression(AST::Expression *e);
	void AssignmentOp(AST::op_type &op);
	void AssignmentStatement(AST::Statement *s);
	void Mutable(AST::Symbol *s);
	void Expression(AST::Expression *e);
	void BaseUnitDefinition(AST::Statement *s);
	void BitShiftExpression(AST::Expression *e);
	void ConcatenativeExpression(AST::Expression *e);
	void BitShiftOp(AST::op_type &op);
	void CallStatement(AST::Statement *s);
	void DeclaredName();
	void CaseExpression(AST::CaseExpression *ce);
	void ComparisonOp(AST::op_type &op);
	void CaseStatement(AST::Statement *block, int &bCaseElse, int &line, int &col);
	void GenericDefinition();
	void Inheritance();
	void Traits();
	void GenericConstraints(bool isGeneric);
	void ClassMember(AST::Class *s, int &elems);
	void ClassMistake();
	void ConstructorDefinition(AST::Class *s, int &elems);
	void DestructorDefinition(AST::Class *s, int &elems);
	void EventDefinition(AST::Statement *s);
	void FunctionDefinition(AST::Statement *s);
	void MethodDefinition(AST::Statement *s);
	void ObjectDefinition(AST::Statement *s);
	void OverrideMember(AST::Statement *s);
	void PropertyDefinition(AST::Statement *s);
	void SharedMember(AST::Statement *s, int &elems);
	void SubDefinition(AST::Statement *s);
	void ClassType();
	void GenericUsage();
	void CompoundDoStatement(AST::Do *block);
	void WhileOrUntil(bool &until);
	void OtherwiseClause(AST::Block *block);
	void CompoundIfStatement(AST::If *block);
	void CompoundStatement(AST::Statement *stmt);
	void DimStatement(AST::Statement *s);
	void DoStatement(AST::Do *block);
	void ForEachStatement(AST::ForEach *block);
	void ForStatement(AST::For *block);
	void IfStatement(AST::If *block);
	void NewStatement(AST::Statement *s);
	void SelectStatement(AST::Statement *s);
	void TryStatement(AST::Try *block);
	void WhileStatement(AST::Statement *b);
	void ComparisonExpression(AST::Expression *e);
	void Tolerance(AST::ComparisonOp *e);
	void ParameterList(AST::ParameterList *p);
	void DataTypeClause();
	void PrimitiveType(primitive_limit lim, Type::Type *type);
	void Declarator(bool &isGeneric);
	void Initializer();
	void DeclaratorList(bool &isGeneric);
	void SimpleStatement(AST::Statement *s);
	void DotMember();
	void LogicalXORExpression(AST::Expression *e);
	void EnumConstant(AST::Enum *s);
	void Number(AST::Expression *e);
	void EnumDefinition(AST::Statement *s);
	void ExitStatement(AST::Statement *s);
	void FlagsDefinition(AST::Statement *s);
	void ParameterListEnclosed(AST::ParameterList *pl);
	void ParameterListUnenclosed(AST::ParameterList *pl);
	void FunctionSignature(bool &isBodiless);
	void ProcMistake();
	void GenericConstraint();
	void GotoStatement(AST::Statement *s);
	void IdentifierExpression(AST::Expression *e);
	void Subscript();
	void TypedIdentifier(AST::Expression *e);
	void LibraryAttribute();
	void RequireStatement(AST::Statement *s);
	void LibraryModuleDeclaration(AST::Statement *s);
	void ModuleMistake();
	void StructDefinition(AST::Statement *s);
	void TraitDefinition(AST::Statement *s);
	void UnitDefinition(AST::Statement *s);
	void LogicalANDExpression(AST::Expression *e);
	void LogicalANDOp(AST::op_type &op);
	void LogicalORExpression(AST::Expression *e);
	void LogicalOROp(AST::op_type &op);
	void MalformedToken();
	void MethodCallStatement(AST::Statement *s);
	void PowerExpression(AST::Expression *e);
	void MultiplicativeOp(AST::op_type &op);
	void NarrowDeclaration();
	void ObjectInitializerStatement(AST::Statement *s);
	void NewStatementMember(AST::Begin *s);
	void ObjectExpression(AST::Expression *e);
	void UnaryExpression(AST::Expression *e);
	void PrimaryExpression(AST::Expression *e);
	void String(AST::Expression *e);
	void TypeCast(AST::Expression *e);
	void Requirement();
	void RequirementRestriction();
	void RequirementRelation();
	void ReturnStatement(AST::Statement *s);
	void ThrowStatement(AST::Statement *s);
	void WaitStatement(AST::Statement *s);
	void UnitAlias();
	void UnitAliases();
	void UnitParameter();
	void UserModuleDeclaration(AST::Statement *s);

	void Parse();

}; // end Parser



#endif

