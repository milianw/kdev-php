/***************************************************************************
 *   This file is part of KDevelop                                         *
 *   Copyright 2007 David Nolden <david.nolden.kdevelop@art-master.de>     *
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
#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include <language/duchain/duchainpointer.h>

#include "phpduchainexport.h"
#include "expressionevaluationresult.h"

namespace Php
{
class AstNode;
class EditorIntegrator;

class KDEVPHPDUCHAIN_EXPORT ExpressionParser
{
public:
    /**
    * @param strict When this is false, the expression-visitor tries to recover from problems. For example when it cannot find a matching function, it returns the first of the candidates.
    * @param debug Enables additional output
    * */
    explicit ExpressionParser(bool useCursor, bool debug = false);

    void setCreateProblems(bool v);

    ExpressionEvaluationResult evaluateType(const QByteArray& expression, KDevelop::DUContextPointer context);
    ExpressionEvaluationResult evaluateType(AstNode* ast, EditorIntegrator* editor);
private:
    bool m_useCursor;
    bool m_debug;
    bool m_createProblems;
};


}
#endif
