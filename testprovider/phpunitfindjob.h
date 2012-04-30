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

#ifndef PHPUNITFINDJOB_H
#define PHPUNITFINDJOB_H

#include <KJob>

namespace KDevelop {
class IndexedString;
class ReferencedTopDUContext;
}

class PhpUnitFindJob : public KJob
{
    Q_OBJECT
public:
    PhpUnitFindJob(QObject* parent = 0);
    ~PhpUnitFindJob();

    virtual void start();

private slots:
    void updateReady(const KDevelop::IndexedString& document,
                     const KDevelop::ReferencedTopDUContext& context);
};

#endif // PHPUNITFINDJOB_H