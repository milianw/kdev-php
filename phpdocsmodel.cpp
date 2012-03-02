/*  This file is part of the KDevelop PHP Documentation Plugin

    Copyright 2012 Milian Wolff <mail@milianw.de>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "phpdocsmodel.h"

#include <language/duchain/duchain.h>
#include <language/duchain/declaration.h>
#include <language/duchain/duchainlock.h>

#include <language/duchain/types/structuretype.h>

#include <interfaces/icore.h>
#include <interfaces/ilanguage.h>
#include <interfaces/ilanguagecontroller.h>
#include <language/backgroundparser/backgroundparser.h>
#include <language/backgroundparser/parsejob.h>

#include <KStandardDirs>
#include <KLocalizedString>

using namespace KDevelop;

PhpDocsModel::PhpDocsModel(QObject* parent)
    : QAbstractListModel(parent), m_internalFunctionsFile(KStandardDirs::locate("data", "kdevphpsupport/phpfunctions.php"))
{
    // make sure the php plugin is loaded
    ILanguage* phpLangPlugin = ICore::self()->languageController()->language("Php");
    if ( !phpLangPlugin ) {
        kWarning() << "could not load PHP language support plugin";
        return;
    }
    fillModel();
}

PhpDocsModel::~PhpDocsModel()
{
}

const KDevelop::IndexedString& PhpDocsModel::internalFunctionFile() const
{
    return m_internalFunctionsFile;
}

void PhpDocsModel::slotParseJobFinished( ParseJob* job )
{
    if ( job->document() == m_internalFunctionsFile ) {
        disconnect(ICore::self()->languageController()->backgroundParser(), SIGNAL(parseJobFinished(KDevelop::ParseJob*)),
                   this, SLOT(slotParseJobFinished(KDevelop::ParseJob*)));
        fillModel();
    }
}

void PhpDocsModel::fillModel()
{
    DUChainReadLocker lock(DUChain::self()->lock());

    TopDUContext* top = DUChain::self()->chainForDocument(m_internalFunctionsFile);
    if ( !top ) {
        kDebug() << "could not find DUChain for internal function file, connecting to background parser";
        connect(ICore::self()->languageController()->backgroundParser(), SIGNAL(parseJobFinished(KDevelop::ParseJob*)),
                this, SLOT(slotParseJobFinished(KDevelop::ParseJob*)));
        return;
    }

    kDebug() << "filling model";
    typedef QPair<Declaration*, int> DeclDepthPair;
    foreach ( const DeclDepthPair& declpair, top->allDeclarations(top->range().end, top) ) {
        if ( declpair.first->abstractType() && declpair.first->abstractType()->modifiers() & AbstractType::ConstModifier ) {
            // filter global constants, since they are hard to find in the documentation
            continue;
        }
        m_declarations << DeclarationPointer(declpair.first);

        if ( StructureType::Ptr type = declpair.first->type<StructureType>() ) {
            foreach ( Declaration* dec, type->internalContext(top)->localDeclarations() ) {
                m_declarations << DeclarationPointer(dec);
            }
        }
    }
}

bool PhpDocsModel::hasChildren(const QModelIndex& parent) const
{
    // only the top-level has children
    return parent == QModelIndex();
}

QVariant PhpDocsModel::data(const QModelIndex& index, int role) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole: {
            DUChainReadLocker lock;
            DeclarationPointer dec = declarationForIndex(index);
            if (!dec.data()) {
                return i18n("<lost declaration>");
            }
            QString ret = dec->toString();
            if ( dec->isFunctionDeclaration() ) {
                // remove function arguments
                ret = ret.replace(QRegExp("\\(.+\\)"), "()");
                // remove return type
                ret = ret.remove(QRegExp("^[^ ]+ "));
            }
            return ret;
        }
        case DeclarationRole: {
            DeclarationPointer dec = declarationForIndex(index);
            return QVariant::fromValue<DeclarationPointer>(dec);
        }
        /*
        case Qt::ToolTipRole:
        case Qt::AccessibleTextRole:
        case Qt::AccessibleDescriptionRole:
        */
        default:
            return QVariant();
    }
}

int PhpDocsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return m_declarations.count();
}

bool PhpDocsModel::canFetchMore(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return false;
}

DeclarationPointer PhpDocsModel::declarationForIndex(const QModelIndex& index) const
{
    Q_ASSERT(m_declarations.size() > index.row());

    return m_declarations[index.row()];
}

#include "phpdocsmodel.moc"
