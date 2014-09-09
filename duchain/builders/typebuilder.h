/***************************************************************************
 *   This file is part of KDevelop                                         *
 *   Copyright 2008 Niko Sams <niko.sams@gmail.com>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef TYPEBUILDER_H
#define TYPEBUILDER_H

#include "contextbuilder.h"

#include <language/duchain/builders/abstracttypebuilder.h>

#include <language/duchain/types/functiontype.h>
#include <language/duchain/declaration.h>
#include <language/duchain/identifier.h>

namespace Php
{

typedef KDevelop::AbstractTypeBuilder<AstNode, IdentifierAst, ContextBuilder> TypeBuilderBase;

/**
 * Create types from an AstNode tree.
 *
 * \note This builder overrides visitDeclarator, in order to support
 * array types; parent classes will not have
 * their visitDeclarator function called.
 */
class TypeBuilder: public TypeBuilderBase
{
public:
    TypeBuilder();
    ~TypeBuilder();

protected:
    virtual void visitClassDeclarationStatement(ClassDeclarationStatementAst* node);
    virtual void visitInterfaceDeclarationStatement(InterfaceDeclarationStatementAst* node);
    virtual void visitTraitDeclarationStatement(TraitDeclarationStatementAst* node);
    virtual void visitClassStatement(ClassStatementAst *node);
    virtual void visitClassVariable(ClassVariableAst *node);
    virtual void visitConstantDeclaration(ConstantDeclarationAst* node);
    virtual void visitParameter(ParameterAst *node);
    virtual void visitFunctionDeclarationStatement(FunctionDeclarationStatementAst* node);
    virtual void visitClosure(ClosureAst* node);

    virtual void visitStatement(StatementAst* node);
    virtual void visitAssignmentExpression(AssignmentExpressionAst* node);
    virtual void visitStaticVar(StaticVarAst *node);
    virtual void visitCatchItem(CatchItemAst *node);

    /// The declaration builder implements this and updates
    /// the type of the current declaration
    virtual void updateCurrentType();

    KDevelop::AbstractType::Ptr getTypeForNode(AstNode* node);
private:
    KDevelop::FunctionType::Ptr m_currentFunctionType;
    QList<KDevelop::AbstractType::Ptr> m_currentFunctionParams;

    bool m_gotTypeFromDocComment;
    bool m_gotReturnTypeFromDocComment;

    KDevelop::FunctionType::Ptr openFunctionType(AstNode* node);

    KDevelop::AbstractType::Ptr injectParseType(QString type, AstNode* node);
    KDevelop::AbstractType::Ptr parseType(QString type, AstNode* node);
    KDevelop::AbstractType::Ptr parseDocComment(AstNode* node, const QString& docCommentName);
    QList<KDevelop::AbstractType::Ptr> parseDocCommentParams(AstNode* node);
};

}

#endif // TYPEBUILDER_H

