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

#include "AST.h"

namespace AST {

void AfterwardOtherwise::Accept(Visitor *v) { v->Visit(this); }
void Assignment::Accept(Visitor *v) { v->Visit(this); }
void BinaryOp::Accept(Visitor *v) { v->Visit(this); }
void Block::Accept(Visitor *v) { v->Visit(this); }
void Case::Accept(Visitor *v) { v->Visit(this); }
void CaseExpression::Accept(Visitor *v) { v->Visit(this); }
void CaseIs::Accept(Visitor *v) { v->Visit(this); }
void CaseTo::Accept(Visitor *v) { v->Visit(this); }
void Do::Accept(Visitor *v) { v->Visit(this); }
void ElseIf::Accept(Visitor *v) { v->Visit(this); }
void Expression::Accept(Visitor *v) { v->Visit(this); }
void For::Accept(Visitor *v) { v->Visit(this); }
void ForEach::Accept(Visitor *v) { v->Visit(this); }
void If::Accept(Visitor *v) { v->Visit(this); }
void IfOp::Accept(Visitor *v) { v->Visit(this); }
void Select::Accept(Visitor *v) { v->Visit(this); }
void Statement::Accept(Visitor *v) { v->Visit(this); }
void Symbol::Accept(Visitor *v) { v->Visit(this); }
void UnaryOp::Accept(Visitor *v) { v->Visit(this); }
void While::Accept(Visitor *v) { v->Visit(this); }

} // namespace AST
