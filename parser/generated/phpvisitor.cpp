// THIS FILE IS GENERATED
// WARNING! All changes made in this file will be lost!

#include "phpvisitor.h"

namespace Php
{

Visitor::ParserFuncType Visitor::sParserTable[] =
{
    reinterpret_cast<ParserFuncType>(&Visitor::visitAdditiveExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAdditiveExpressionRest),
    reinterpret_cast<ParserFuncType>(&Visitor::visitArrayPairValue),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAssignmentExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAssignmentExpressionCheckIfVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAssignmentExpressionEqual),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAssignmentList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitAssignmentListElement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBaseVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBaseVariableWithFunctionCalls),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBitAndExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBitOrExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBitXorExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBooleanAndExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitBooleanOrExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCaseList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCase_item),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCatchItem),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassBody),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassConstantDeclaration),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassDeclarationStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassExtends),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassImplements),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassNameReference),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitClassVariableDeclaration),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCommonScalar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCompoundVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCompoundVariableWithSimpleIndirectReference),
    reinterpret_cast<ParserFuncType>(&Visitor::visitConditionalExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitConstantOrClassConst),
    reinterpret_cast<ParserFuncType>(&Visitor::visitCtorArguments),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDeclareItem),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDeclareStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDimListItem),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDimOffset),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDynamicClassNameReference),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDynamicClassNameVariableProperties),
    reinterpret_cast<ParserFuncType>(&Visitor::visitDynamicClassNameVariableProperty),
    reinterpret_cast<ParserFuncType>(&Visitor::visitElseSingle),
    reinterpret_cast<ParserFuncType>(&Visitor::visitElseifList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitElseifListItem),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEncaps),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEncapsList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEncapsVar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEncapsVarOffset),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEqualityExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitEqualityExpressionRest),
    reinterpret_cast<ParserFuncType>(&Visitor::visitExpr),
    reinterpret_cast<ParserFuncType>(&Visitor::visitForExpr),
    reinterpret_cast<ParserFuncType>(&Visitor::visitForStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitForeachStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitForeachVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitFunctionCall),
    reinterpret_cast<ParserFuncType>(&Visitor::visitFunctionCallParameterList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitFunctionCallParameterListElement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitFunctionDeclarationStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitGlobalVar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitIdentifier),
    reinterpret_cast<ParserFuncType>(&Visitor::visitInnerStatementList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitInterfaceDeclarationStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitLogicalAndExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitLogicalOrExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitLogicalXorExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitMethodBody),
    reinterpret_cast<ParserFuncType>(&Visitor::visitMultiplicativeExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitMultiplicativeExpressionRest),
    reinterpret_cast<ParserFuncType>(&Visitor::visitNewElseSingle),
    reinterpret_cast<ParserFuncType>(&Visitor::visitNewElseifList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitNewelseifListItem),
    reinterpret_cast<ParserFuncType>(&Visitor::visitObjectDimList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitObjectProperty),
    reinterpret_cast<ParserFuncType>(&Visitor::visitOptionalClassModifier),
    reinterpret_cast<ParserFuncType>(&Visitor::visitOptionalModifiers),
    reinterpret_cast<ParserFuncType>(&Visitor::visitParameter),
    reinterpret_cast<ParserFuncType>(&Visitor::visitParameterList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitPostprefixOperator),
    reinterpret_cast<ParserFuncType>(&Visitor::visitPrintExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitRelationalExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitRelationalExpressionRest),
    reinterpret_cast<ParserFuncType>(&Visitor::visitScalar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitSemicolonOrCloseTag),
    reinterpret_cast<ParserFuncType>(&Visitor::visitShiftExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitShiftExpressionRest),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStart),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStaticArrayPairValue),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStaticMember),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStaticScalar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitStaticVar),
    reinterpret_cast<ParserFuncType>(&Visitor::visitSwitchCaseList),
    reinterpret_cast<ParserFuncType>(&Visitor::visitTopStatement),
    reinterpret_cast<ParserFuncType>(&Visitor::visitUnaryExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitUnaryExpressionNotPlusminus),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVarExpression),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVarExpressionNewObject),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVarExpressionNormal),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVariable),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVariableIdentifier),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVariableName),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVariableProperty),
    reinterpret_cast<ParserFuncType>(&Visitor::visitVariableWithoutObjects),
    reinterpret_cast<ParserFuncType>(&Visitor::visitWhileStatement)
}; // sParserTable[]

} // end of namespace Php

