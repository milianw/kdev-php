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

#include "phpdocsplugin.h"
#include "phpdocsmodel.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KAboutData>
#include <KSettings/Dispatcher>
#include <KDebug>
#include <KComponentData>

#include <interfaces/idocumentation.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentationcontroller.h>

#include <language/duchain/duchain.h>
#include <language/duchain/declaration.h>
#include <language/duchain/duchainlock.h>

#include <language/duchain/classdeclaration.h>
#include <language/duchain/functiondeclaration.h>
#include <language/duchain/classmemberdeclaration.h>
#include <language/duchain/classfunctiondeclaration.h>

#include <QtCore/QFile>

#include "phpdocumentation.h"
#include "phpdocssettings.h"
#include "kdevphpdocsversion.h"

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(PhpDocsFactory, "kdevphpdocs.json", registerPlugin<PhpDocsPlugin>();)

PhpDocsPlugin::PhpDocsPlugin(QObject* parent, const QVariantList& args)
    : IPlugin("kdevphpdocs", parent)
    , m_model(new PhpDocsModel(this))
{
    KDEV_USE_EXTENSION_INTERFACE( KDevelop::IDocumentationProvider )
    Q_UNUSED(args);

    readConfig();

    KSettings::Dispatcher::registerComponent( "kdevphpdocs_config", this, "readConfig" );
}

PhpDocsPlugin::~PhpDocsPlugin()
{
}

QString PhpDocsPlugin::name() const
{
    return QString("PHP");
}

QIcon PhpDocsPlugin::icon() const
{
    static QIcon icon = QIcon::fromTheme("application-x-php");
    return icon;
}

void PhpDocsPlugin::readConfig()
{
    // since PhpDocsSettings pointer in this plugin is distinct from the one in the KCM
    // we have to trigger reading manually
    PhpDocsSettings::self()->readConfig();
}

///TODO: possibly return multiple filenames (i.e. fallbacks) when doing local lookups
QString PhpDocsPlugin::getDocumentationFilename( KDevelop::Declaration* dec, const bool& isLocal ) const
{
    QString fname;

    //TODO: most of the SPL stuff is not found for me in the deb package php-doc
    //      => check newer documentation or give a fallback to ref.spl.html
    if ( ClassFunctionDeclaration* fdec = dynamic_cast<ClassFunctionDeclaration*>( dec ) ) {
        // class methods -> php.net/CLASS.METHOD
        // local: either CLASS.METHOD.html or function.CLASS-METHOD.html... really sucks :-/
        //        for now, use the latter...
        if ( dec->context() && dec->context()->type() == DUContext::Class && dec->context()->owner() ) {
            QString className = dec->context()->owner()->identifier().toString();

            if ( !isLocal ) {
                fname = className + '.' + fdec->identifier().toString();
            } else {
                if ( fdec->isConstructor() ) {
                    fname = "construct";
                } else if ( fdec->isDestructor() ) {
                    fname = "destruct";
                } else {
                    fname = fdec->identifier().toString();
                }
                //TODO: CLASS.METHOD.html e.g. for xmlreader etc. pp.
                fname = "function." + className + '-' + fname;
            }
        }
    } else if ( dynamic_cast<ClassDeclaration*>(dec) ) {
        fname = "class." + dec->identifier().toString();
    } else if ( dynamic_cast<FunctionDeclaration*>(dec) ) {
        fname = "function." + dec->identifier().toString();
    }
    // check for superglobals / reserved variables
    else if ( dec->identifier() == Identifier("GLOBALS") ||
                dec->identifier() == Identifier("php_errormsg") ||
                dec->identifier() == Identifier("HTTP_RAW_POST_DATA") ||
                dec->identifier() == Identifier("http_response_header") ||
                dec->identifier() == Identifier("argc") ||
                dec->identifier() == Identifier("argv") ||
                dec->identifier() == Identifier("_GET") ||
                dec->identifier() == Identifier("_POST") ||
                dec->identifier() == Identifier("_FILES") ||
                dec->identifier() == Identifier("_REQUEST") ||
                dec->identifier() == Identifier("_SESSION") ||
                dec->identifier() == Identifier("_ENV") ||
                dec->identifier() == Identifier("_COOKIE") ) {
        if ( isLocal ) {
            fname = QString("reserved.variables.") + dec->identifier().toString().remove('_');
        } else {
            fname = dec->identifier().toString();
        }
    }

    kDebug() << fname;

    if ( !fname.isEmpty() && isLocal ) {
        fname = fname.toLower();
        fname.replace('_', '-');
        fname.append(".html");
    }

    return fname;
}

IDocumentation::Ptr PhpDocsPlugin::documentationForDeclaration( Declaration* dec ) const
{
    if ( dec ) {
        DUChainReadLocker lock( DUChain::lock() );

        // filter non global or non-php declarations
        if ( dec->topContext()->url() != m_model->internalFunctionFile() ) {
            return {};
        }

        QUrl url = PhpDocsSettings::phpDocLocation();
        kDebug() << url;

        QString file = getDocumentationFilename( dec, url.isLocalFile() );
        if ( file.isEmpty() ) {
            kDebug() << "no documentation pattern found for" << dec->toString();
            return {};
        }

        url.setPath( url.path() + '/' + file);
        if ( url.isLocalFile() && !QFile::exists( url.toLocalFile() ) ) {
            kDebug() << "bad path" << url << "for documentation of" << dec->toString() << " - aborting";
            return {};
        }

        kDebug() << "php documentation located at " << url << "for" << dec->toString();
        return documentationForUrl(url, dec->qualifiedIdentifier().toString(), dec->comment());
    }

    return {};
}

QAbstractListModel* PhpDocsPlugin::indexModel() const
{
    return m_model;
}

IDocumentation::Ptr PhpDocsPlugin::documentationForIndex(const QModelIndex& index) const
{
    return documentationForDeclaration(static_cast<Declaration*>(
        index.data(PhpDocsModel::DeclarationRole).value<DeclarationPointer>().data()
    ));
}

void PhpDocsPlugin::loadUrl(const QUrl& url) const
{
    kDebug() << "loading URL" << url;
    auto doc = documentationForUrl(url, QString());
    ICore::self()->documentationController()->showDocumentation(doc);
}

void PhpDocsPlugin::addToHistory(const QUrl& url)
{
    auto doc = documentationForUrl(url, url.toString());
    emit addHistory(doc);
}

IDocumentation::Ptr PhpDocsPlugin::documentationForUrl(const QUrl& url, const QString& name, const QByteArray& description) const
{
    return IDocumentation::Ptr(new PhpDocumentation( url, name, description, const_cast<PhpDocsPlugin*>(this)));
}

IDocumentation::Ptr PhpDocsPlugin::homePage() const
{
    QUrl url = PhpDocsSettings::phpDocLocation();
    if ( url.isLocalFile() ) {
        url.setPath(url.path() + "/index.html");
    } else {
        url.setPath(url.path() + "/manual");
    }
    return documentationForUrl(url, i18n("PHP Documentation"));
}

#include "phpdocsplugin.moc"
