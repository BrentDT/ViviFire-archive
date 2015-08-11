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

#include <cstddef>
#include "CodeGen.h"

using namespace AST;

void CodeGen::Visit(AfterwardOtherwise *node) {}
void CodeGen::Visit(Assignment *node) {}
void CodeGen::Visit(BinaryOp *node) {}
void CodeGen::Visit(Case *node) {}
void CodeGen::Visit(CaseExpression *node) {}
void CodeGen::Visit(CaseIs *node) {}
void CodeGen::Visit(CaseTo *node) {}
void CodeGen::Visit(ComparisonOp *node) {}
void CodeGen::Visit(Do *node) {}
void CodeGen::Visit(For *node) {}
void CodeGen::Visit(ForEach *node) {}
void CodeGen::Visit(IfOp *node) {}
void CodeGen::Visit(Select *node) {}
void CodeGen::Visit(UnaryOp *node) {}
void CodeGen::Visit(While *node) {}

