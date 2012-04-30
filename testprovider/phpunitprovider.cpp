/***************************************************************************
 *   This file is part of KDevelop PHP support                             *
 *   Copyright 2012 Miha Čančula <miha@noughmad.eu>                        *
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


#include "phpunitprovider.h"
#include "phpunittestsuite.h"
#include <interfaces/icore.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iproject.h>
#include <interfaces/ilanguage.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/itestcontroller.h>
#include <project/projectmodel.h>


#include <KPluginFactory>
#include <KAboutData>
#include <KLocalizedString>
#include <KStandardDirs>
#include <QVariant>
#include <QTimer>
#include "phpunitfindjob.h"

K_PLUGIN_FACTORY(PhpUnitProviderFactory, registerPlugin<PhpUnitProvider>(); )
K_EXPORT_PLUGIN(PhpUnitProviderFactory(KAboutData("kdevphpunitprovider","kdevphpunitprovider", ki18n("PHPUnit Integration"), "0.1", ki18n("Finds and run PHPUnit tests"), KAboutData::License_GPL)))


using namespace KDevelop;

PhpUnitProvider::PhpUnitProvider(QObject* parent, const QList< QVariant >& args): IPlugin(PhpUnitProviderFactory::componentData(), parent)
{
    KDEV_USE_EXTENSION_INTERFACE(KDevelop::ITestProvider)
    Q_UNUSED(args);
}

void PhpUnitProvider::unload()
{
}

KJob* PhpUnitProvider::findTests()
{
    return new PhpUnitFindJob(this);
}
