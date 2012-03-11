/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Miha ?an?ula <miha.cancula@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "phpunitfindjob.h"

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iprojectcontroller.h>

#include <language/duchain/declaration.h>
#include <language/duchain/persistentsymboltable.h>
#include <language/duchain/duchainutils.h>
#include <language/duchain/duchain.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/classdeclaration.h>
#include <language/duchain/functiondeclaration.h>
#include <language/duchain/functiondefinition.h>

#include <KStandardDirs>

#include <QTimer>
#include "phpunittestsuite.h"
#include <interfaces/itestcontroller.h>
#include <interfaces/iplugincontroller.h>

PhpUnitFindJob::PhpUnitFindJob(QObject* parent): KJob(parent)
{

}

PhpUnitFindJob::~PhpUnitFindJob()
{

}

void PhpUnitFindJob::start()
{

    QString file = KStandardDirs::locate("data", "kdevphpsupport/phpunitdeclarations.php");
    DUChain::self()->updateContextForUrl(IndexedString(file), KDevelop::TopDUContext::AllDeclarationsContextsAndUses, this, -10);
}

void PhpUnitFindJob::updateReady(const IndexedString& document, const KDevelop::ReferencedTopDUContext& context)
{
    Q_UNUSED(document)

    ITestController* tc = ICore::self()->pluginController()->pluginForExtension("org.kdevelop.ITestController")->extension<ITestController>();
    DUChainReadLocker lock(DUChain::lock());

    QVector<Declaration*> decl = context->localDeclarations();
    kDebug() << "Found" << decl.size() << "declarations using the direct method";

    foreach (Declaration* p, decl)
    {
        kDebug() << "Found PHP Unit declaration" << p->identifier().toString();
        uint depth = 3;
        foreach (Declaration* d, DUChainUtils::getInheriters(p, depth))
        {
            QString name = d->identifier().toString();
            KUrl url = d->url().toUrl();
            IProject* project = ICore::self()->projectController()->findProjectForUrl(url);
            kDebug() << name << url << (project ? project->name() : "No project");
            if (!project)
            {
                continue;
            }
            QStringList testCases;
            QHash<QString, IndexedDeclaration> testCaseDeclarations;
            ClassDeclaration* classDeclaration = dynamic_cast<ClassDeclaration*>(d);
            if (classDeclaration && !classDeclaration->isAbstract())
            {
                foreach (Declaration* member, classDeclaration->internalContext()->localDeclarations())
                {
                    kDebug() << "Trying test case declaration" << member;
                    if (member->isFunctionDeclaration() && member->identifier().toString().startsWith("test"))
                    {
                        const QString caseName = member->identifier().toString();
                        testCases << caseName;
                        testCaseDeclarations.insert(caseName, IndexedDeclaration(member));
                    }
                }
                ITestSuite* existingSuite = tc->findTestSuite(project, name);
                if (existingSuite && existingSuite->cases() != testCases)
                {
                    tc->removeTestSuite(existingSuite);
                    existingSuite = 0;
                }
                if (!existingSuite)
                {
                    tc->addTestSuite(new PhpUnitTestSuite(name, url, IndexedDeclaration(classDeclaration), testCases, testCaseDeclarations, project));
                }
            }
        }
    }

    emitResult();
}
