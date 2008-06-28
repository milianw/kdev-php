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

#include "declarationbuilder.h"

#include <QByteArray>

#include <ktexteditor/smartrange.h>
#include <ktexteditor/smartinterface.h>

#include <language/duchain/functiondeclaration.h>
#include "phpast.h"
#include "parsesession.h"

using namespace KTextEditor;
using namespace KDevelop;

namespace Php {

DeclarationBuilder::DeclarationBuilder (ParseSession* session)
    : m_lastVariableDeclaration(0), m_lastVariableIdentifier(0)
{
  setEditor(session);
}

DeclarationBuilder::DeclarationBuilder (EditorIntegrator* editor)
    : m_lastVariableDeclaration(0), m_lastVariableIdentifier(0)
{
  setEditor(editor);
}

void DeclarationBuilder::closeDeclaration()
{
    if (currentDeclaration()) {
        DUChainWriteLocker lock(DUChain::lock());
        currentDeclaration()->setType(lastType());
    }

  eventuallyAssignInternalContext();

  DeclarationBuilderBase::closeDeclaration();
}

void DeclarationBuilder::visitClassDeclarationStatement(ClassDeclarationStatementAst * node)
{
    openDefinition(node->className, node, false);
    currentDeclaration()->setKind(KDevelop::Declaration::Type);

    DeclarationBuilderBase::visitClassDeclarationStatement(node);

    closeDeclaration();
}

void DeclarationBuilder::visitInterfaceDeclarationStatement(InterfaceDeclarationStatementAst *node)
{
    openDefinition(node->interfaceName, node, false);
    currentDeclaration()->setKind(KDevelop::Declaration::Type);

    DeclarationBuilderBase::visitInterfaceDeclarationStatement(node);

    closeDeclaration();
}


void DeclarationBuilder::visitClassStatement(ClassStatementAst *node)
{
    if (node->methodName) {
        //method declaration

        openDefinition(node->methodName, node, true);

        currentDeclaration()->setKind(Declaration::Type);

        DeclarationBuilderBase::visitClassStatement(node);

        closeDeclaration();
    } else {
        DeclarationBuilderBase::visitClassStatement(node);
    }
}

void DeclarationBuilder::visitClassVariable(ClassVariableAst *node)
{
    {
        DUChainWriteLocker lock(DUChain::lock());
        SimpleRange newRange = editorFindRange(node->variable, node->variable);
        openDefinition(identifierForNode(node->variable), newRange, false);
    }
    currentDeclaration()->setKind(Declaration::Instance);
    DeclarationBuilderBase::visitClassVariable(node);
    closeDeclaration();
}

void DeclarationBuilder::classTypeOpened(AbstractType::Ptr type)
{
    //We override this so we can get the class-declaration into a usable state(with filled type) earlier
    DUChainWriteLocker lock(DUChain::lock());

    IdentifiedType* idType = dynamic_cast<IdentifiedType*>(type.data());

    if( idType && idType->declaration() == 0 ) //When the given type has no declaration yet, assume we are declaring it now
        idType->setDeclaration( currentDeclaration() );

    currentDeclaration()->setType(type);
}

void DeclarationBuilder::visitParameter(ParameterAst *node)
{
    {
        DUChainWriteLocker lock(DUChain::lock());
        SimpleRange newRange = editorFindRange(node->variable, node->variable);
        openDefinition(identifierForNode(node->variable), newRange, false);
    }

    currentDeclaration()->setKind(Declaration::Instance);
    DeclarationBuilderBase::visitParameter(node);
    closeDeclaration();
}

void DeclarationBuilder::visitFunctionDeclarationStatement(FunctionDeclarationStatementAst* node)
{
    openDefinition(node->functionName, node, true);

    currentDeclaration()->setKind(Declaration::Type);

    DeclarationBuilderBase::visitFunctionDeclarationStatement(node);

    closeDeclaration();
}

void DeclarationBuilder::visitExpr(ExprAst *node)
{
    m_lastVariableDeclaration = 0;
    m_lastVariableIdentifier = 0;
    DeclarationBuilderBase::visitExpr(node);
}
void DeclarationBuilder::visitVarExpressionNewObject(VarExpressionNewObjectAst *node)
{
    DeclarationBuilderBase::visitVarExpressionNewObject(node);
    if (node->className->identifier && m_lastVariableDeclaration) {
        if(openTypeFromName(node->className->identifier, true)) {
            closeType();
            DUChainWriteLocker lock(DUChain::lock());
            m_lastVariableDeclaration->setType(lastType());
            m_lastVariableDeclaration = 0;
            m_lastVariableIdentifier = 0;
        }
    }
}

void DeclarationBuilder::visitAssignmentExpressionEqual(AssignmentExpressionEqualAst *node)
{
    //create new declaration for every assignment
    //TODO: don't create the same twice
    if (m_lastVariableIdentifier) {
        DUChainWriteLocker lock(DUChain::lock());
        SimpleRange newRange = editorFindRange(m_lastVariableIdentifier, m_lastVariableIdentifier);
        openDefinition(identifierForNode(m_lastVariableIdentifier), newRange, false);
        m_lastVariableDeclaration = currentDeclaration();
        currentDeclaration()->setKind(Declaration::Instance);
        closeDeclaration();
        m_lastVariableDeclaration ->setType(typeRepository()->integral());
    }
    DeclarationBuilderBase::visitAssignmentExpressionEqual(node);
}

void DeclarationBuilder::visitCompoundVariableWithSimpleIndirectReference(CompoundVariableWithSimpleIndirectReferenceAst *node)
{
    m_lastVariableIdentifier = node->variable;
    DeclarationBuilderBase::visitCompoundVariableWithSimpleIndirectReference(node);
}

}
