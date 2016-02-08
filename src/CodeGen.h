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

#if !defined(_CODEGEN_H_)
#define _CODEGEN_H_

#include "AST.h"

using namespace AST;

class CodeGen : public Visitor {
public:
	virtual void Visit(Assignment *);
	virtual void Visit(BinaryOp *);
	virtual void Visit(Block *);
	virtual void Visit(Call *);
	virtual void Visit(Case *);
	virtual void Visit(CaseExpression *);
	virtual void Visit(CaseIs *);
	virtual void Visit(CaseTo *);
	virtual void Visit(Catch *);
	virtual void Visit(ComparisonChain *);
	virtual void Visit(ComparisonOp *);
	virtual void Visit(Do *);
	virtual void Visit(DTConst *);
	virtual void Visit(For *);
	virtual void Visit(ForEach *);
	virtual void Visit(FPConst *);
	virtual void Visit(GotoCase *);
	virtual void Visit(Exit *);
	virtual void Visit(IfOp *);
	virtual void Visit(IntConst *);
	virtual void Visit(LibraryModule *);
	virtual void Visit(Return *);
	virtual void Visit(Select *);
	virtual void Visit(Throw *);
	virtual void Visit(Try *);
	virtual void Visit(UnaryOp *);
	virtual void Visit(UserModule *);
	virtual void Visit(Wait *);
	virtual void Visit(While *);
};

#endif // _CODEGEN_H_
