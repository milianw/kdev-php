/*
 * KDevelop Php Code Completion Support
 *
 * Copyright 2010 Niko Sams <niko.sams@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef PHPCOMPLETIONCODEMODELITEM_H
#define PHPCOMPLETIONCODEMODELITEM_H

#include <language/codecompletion/codecompletionitem.h>
#include "context.h"
#include <language/duchain/codemodel.h>
#include <language/duchain/indexedstring.h>
#include <language/duchain/parsingenvironment.h>

namespace Php
{
class CodeCompletionContext;

class KDEVPHPCOMPLETION_EXPORT CodeModelCompletionItem : public KDevelop::CompletionTreeItem
{
public:
    explicit CodeModelCompletionItem(const KDevelop::ParsingEnvironmentFilePointer &, const KDevelop::CodeModelItem &item);

    virtual QVariant data(const QModelIndex& index, int role, const KDevelop::CodeCompletionModel* model) const;
    virtual KTextEditor::CodeCompletionModel::CompletionProperties completionProperties() const;
protected:
    KDevelop::CodeModelItem m_item;
    KDevelop::ParsingEnvironmentFilePointer m_env;
    mutable KDevelop::DeclarationPointer m_decl;
};

}

#endif
