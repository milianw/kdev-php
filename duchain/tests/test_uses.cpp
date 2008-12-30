/* This file is part of KDevelop
    Copyright 2008 Niko Sams <niko.sams@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "test_uses.h"

#include <QtTest/QtTest>

#include <language/duchain/duchain.h>
#include <language/duchain/duchainlock.h>

#include "phpparsejob.h"
#include "../constantdeclaration.h"


using namespace KTextEditor;
using namespace KDevelop;

QTEST_MAIN(Php::TestUses)

namespace Php
{

TestUses::TestUses()
{
}

void TestUses::newObject()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class Foo {} $a = new Foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/newObject.php");
    DUChainWriteLocker lock(DUChain::lock());
    QCOMPARE(top->localDeclarations().first()->uses().keys().count(), 1);
    QCOMPARE(top->localDeclarations().first()->uses().values().count(), 1);
    QCOMPARE(top->localDeclarations().first()->uses().values().first().count(), 1);
    QCOMPARE(top->localDeclarations().first()->uses().keys().first(), IndexedString("/usestest/newObject.php"));
    QCOMPARE(top->localDeclarations().first()->uses().values().first().first(), SimpleRange(0, 25, 0, 28));
    release(top);
}

void TestUses::functionCall()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? function foo() {} foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/functionCall.php");
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* fun = top->localDeclarations().first();
    QCOMPARE(fun->uses().keys().count(), 1);
    QCOMPARE(fun->uses().values().count(), 1);
    QCOMPARE(fun->uses().values().first().count(), 1);
    QCOMPARE(fun->uses().keys().first(), IndexedString("/usestest/functionCall.php"));
    QCOMPARE(fun->uses().values().first().first(), SimpleRange(0, 21, 0, 24));
    release(top);
}

void TestUses::memberFunctionCall()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function foo() {} } $a = new A(); $a->foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/memberFunctionCall.php");
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* fun = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(fun->uses().keys().count(), 1);
    QCOMPARE(fun->uses().values().count(), 1);
    QCOMPARE(fun->uses().values().first().count(), 1);
    QCOMPARE(fun->uses().keys().first(), IndexedString("/usestest/memberFunctionCall.php"));
    QCOMPARE(fun->uses().values().first().first(), SimpleRange(0, 51, 0, 54));
    release(top);
}

void TestUses::memberVariable()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public $foo; } $a = new A(); $a->foo; ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/memberVariable.php");
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().keys().first(), IndexedString("/usestest/memberVariable.php"));
    QCOMPARE(var->uses().values().first().first(), SimpleRange(0, 46, 0, 49));
    release(top);
}

void TestUses::variable()
{
    //                        0         1         2         3         4         5         6         7
    //                        01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<?php\nclass A { public $foo; } $a = new A(); $a; $a->foo; foo($a); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->localDeclarations().at(1);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 3);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(1, 42-3, 1, 44-3));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(1, 46-3, 1, 48-3));
    QCOMPARE(var->uses().values().first().at(2), SimpleRange(1, 59-3, 1, 61-3));
    release(top);
}

void TestUses::varInString()
{

    //                  0         1         2         3         4         5         6         7
    //                  01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<?php $a=0; \"$a {$a}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->localDeclarations().at(0);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 2);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 13, 0, 15));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 17, 0, 19));
    release(top);
}

void TestUses::memberVarInString()
{

    //                 0         1         2         3         4          5         6          7
    //                 01234567890123456789012345678901234567890123456789 01234567890123 4567890123456789
    QByteArray method("<?php class A { public $v=0; } $a=new A(); $a->v; \"$a->v {$a->v}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->localDeclarations().at(1);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 3);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 43, 0, 45));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 51, 0, 53));
    QCOMPARE(var->uses().values().first().at(2), SimpleRange(0, 58, 0, 60));

    var = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 3);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 47, 0, 48));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 55, 0, 56));
    QCOMPARE(var->uses().values().first().at(2), SimpleRange(0, 62, 0, 63));
    release(top);
}

void TestUses::memberFunctionInString()
{

    //                 0         1         2         3         4          5          6         7
    //                 012345678901234567890123456789012345678901234567 890123456789 01234567890123456789
    QByteArray method("<?php class A { function foo() {} } $a=new A(); \"{$a->foo()}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->localDeclarations().at(1); //$a
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 50, 0, 52));

    var = top->childContexts().first()->localDeclarations().first(); //foo
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 54, 0, 57));
    release(top);
}

void TestUses::variableTwoDeclarations()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } $a = 'a'; $a; $a = 0; $a; $a = 'x'; $a; ");
    //                                15        25  29      37  41        51
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());
    /*
    Declaration* var = top->localDeclarations().at(1);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 3);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 29, 0, 31));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 33, 0, 35));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 41, 0, 43));

    //$a = 0 is a new declaration!
    var = top->localDeclarations().at(2);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 2);
    QCOMPARE(var->uses().values().first().at(0), SimpleRange(0, 29, 0, 31));
    //QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 33, 0, 35));
    QCOMPARE(var->uses().values().first().at(1), SimpleRange(0, 41, 0, 43));
    */
    release(top);
}
void TestUses::variableTwoDeclarationsInFunction()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? function foo() { $a='a'; $a; $a=0; $a; $a=false; }");
    //                                     20      28  32    38  42

    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    release(top);
}

void TestUses::classExtends()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } class B extends A { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* a = top->localDeclarations().at(0);
    QCOMPARE(a->uses().keys().count(), 1);
    QCOMPARE(a->uses().values().count(), 1);
    QCOMPARE(a->uses().values().first().count(), 1);
    QCOMPARE(a->uses().values().first().at(0), SimpleRange(0, 31, 0, 32));

    release(top);
}
void TestUses::classImplements()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } class B implements A { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* a = top->localDeclarations().at(0);
    QCOMPARE(a->uses().keys().count(), 1);
    QCOMPARE(a->uses().values().count(), 1);
    QCOMPARE(a->uses().values().first().count(), 1);
    QCOMPARE(a->uses().values().first().at(0), SimpleRange(0, 38, 0, 39));

    release(top);
}
void TestUses::classImplementsMultiple()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B { } class C implements A, B { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* a = top->localDeclarations().at(0);
    QCOMPARE(a->uses().keys().count(), 1);
    QCOMPARE(a->uses().values().count(), 1);
    QCOMPARE(a->uses().values().first().count(), 1);
    QCOMPARE(a->uses().values().first().at(0), SimpleRange(0, 54, 0, 55));

    Declaration* b = top->localDeclarations().at(1);
    QCOMPARE(b->uses().keys().count(), 1);
    QCOMPARE(b->uses().values().count(), 1);
    QCOMPARE(b->uses().values().first().count(), 1);
    QCOMPARE(b->uses().values().first().at(0), SimpleRange(0, 57, 0, 58));

    release(top);
}
void TestUses::interfaceExtends()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B extends A { }");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* a = top->localDeclarations().at(0);
    QCOMPARE(a->uses().keys().count(), 1);
    QCOMPARE(a->uses().values().count(), 1);
    QCOMPARE(a->uses().values().first().count(), 1);
    QCOMPARE(a->uses().values().first().at(0), SimpleRange(0, 39, 0, 40));

    release(top);
}
void TestUses::interfaceExtendsMultiple()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B { } interface C extends A, B { }");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* a = top->localDeclarations().at(0);
    QCOMPARE(a->uses().keys().count(), 1);
    QCOMPARE(a->uses().values().count(), 1);
    QCOMPARE(a->uses().values().first().count(), 1);
    QCOMPARE(a->uses().values().first().at(0), SimpleRange(0, 55, 0, 56));

    Declaration* b = top->localDeclarations().at(1);
    QCOMPARE(b->uses().keys().count(), 1);
    QCOMPARE(b->uses().values().count(), 1);
    QCOMPARE(b->uses().values().first().count(), 1);
    QCOMPARE(b->uses().values().first().at(0), SimpleRange(0, 58, 0, 59));

    release(top);
}
void TestUses::staticMemberFunctionCall()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public static function foo() {} } A::foo(); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 47, 0, 48));

    Declaration* fun = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(fun->uses().keys().count(), 1);
    QCOMPARE(fun->uses().values().count(), 1);
    QCOMPARE(fun->uses().values().first().count(), 1);
    QCOMPARE(fun->uses().values().first().first(), SimpleRange(0, 50, 0, 53));
    release(top);
}
void TestUses::staticMemberVariable()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public static $foo; } $foo=0; A::$foo; $foo;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 43, 0, 44));

    Declaration* var = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().values().first().first(), SimpleRange(0, 46, 0, 50));

    var = top->localDeclarations().at(1);
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().values().first().first(), SimpleRange(0, 52, 0, 56));

    release(top);
}
void TestUses::constant()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? define('A', 'foo'); echo A;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cnst = top->localDeclarations().first();
    QCOMPARE(cnst->uses().keys().count(), 1);
    QCOMPARE(cnst->uses().values().count(), 1);
    QCOMPARE(cnst->uses().values().first().count(), 1);
    QCOMPARE(cnst->uses().values().first().first(), SimpleRange(0, 28, 0, 29));

    release(top);
}

void TestUses::classConstant()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { const FOO = 'abc'; } echo A::FOO;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 39, 0, 40));

    Declaration* cnst = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(cnst->uses().keys().count(), 1);
    QCOMPARE(cnst->uses().values().count(), 1);
    QCOMPARE(cnst->uses().values().first().count(), 1);
    QCOMPARE(cnst->uses().values().first().first(), SimpleRange(0, 42, 0, 45));

    release(top);
}

void TestUses::classParent()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function x() {} } class B extends A { function x() { parent::x(); }} ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 2);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 47, 0, 48));
    QCOMPARE(cls->uses().values().first().at(1), SimpleRange(0, 66, 0, 72));

    Declaration* fun = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(fun->uses().keys().count(), 1);
    QCOMPARE(fun->uses().values().count(), 1);
    QCOMPARE(fun->uses().values().first().count(), 1);
    QCOMPARE(fun->uses().values().first().first(), SimpleRange(0, 74, 0, 75));

    release(top);
}

void TestUses::classSelf()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function x() { self::x(); } } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 28, 0, 32));

    Declaration* fun = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(fun->uses().keys().count(), 1);
    QCOMPARE(fun->uses().values().count(), 1);
    QCOMPARE(fun->uses().values().first().count(), 1);
    QCOMPARE(fun->uses().values().first().first(), SimpleRange(0, 34, 0, 35));

    release(top);
}

void TestUses::objectWithClassName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class Aa { public static $i; const j=0; public $k; } $Aa = new Aa; $Aa->k; Aa::j; Aa::$i;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 3);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 66, 0, 66+2));
    QCOMPARE(cls->uses().values().first().at(1), SimpleRange(0, 78, 0, 78+2));
    QCOMPARE(cls->uses().values().first().at(2), SimpleRange(0, 85, 0, 85+2));

    Declaration* obj = top->localDeclarations().at(1);
    QCOMPARE(obj->uses().keys().count(), 1);
    QCOMPARE(obj->uses().values().count(), 1);
    QCOMPARE(obj->uses().values().first().count(), 1);
    QCOMPARE(obj->uses().values().first().first(), SimpleRange(0, 70, 0, 70+3));

    release(top);
}

void TestUses::classAndConstWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } define('A', 0); A; new A; define('B', 0); class B { } new B; B; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 38, 0, 39));

    Declaration* cnst = top->localDeclarations().at(1);
    QCOMPARE(cnst->uses().keys().count(), 1);
    QCOMPARE(cnst->uses().values().count(), 1);
    QCOMPARE(cnst->uses().values().first().count(), 1);
    QCOMPARE(cnst->uses().values().first().first(), SimpleRange(0, 31, 0, 32));

    cls = top->localDeclarations().at(2);
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 76, 0, 77));

    cnst = top->localDeclarations().at(3);
    QCOMPARE(cnst->uses().keys().count(), 1);
    QCOMPARE(cnst->uses().values().count(), 1);
    QCOMPARE(cnst->uses().values().first().count(), 1);
    QCOMPARE(cnst->uses().values().first().first(), SimpleRange(0, 73, 0, 74));

    release(top);
}


void TestUses::classAndFunctionWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } function A() {} new A; A(); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* cls = top->localDeclarations().first();
    QCOMPARE(cls->uses().keys().count(), 1);
    QCOMPARE(cls->uses().values().count(), 1);
    QCOMPARE(cls->uses().values().first().count(), 1);
    QCOMPARE(cls->uses().values().first().first(), SimpleRange(0, 35, 0, 36));

    Declaration* fn = top->localDeclarations().at(1);
    QCOMPARE(fn->uses().keys().count(), 1);
    QCOMPARE(fn->uses().values().count(), 1);
    QCOMPARE(fn->uses().values().first().count(), 1);
    QCOMPARE(fn->uses().values().first().first(), SimpleRange(0, 38, 0, 39));

    release(top);
}

void TestUses::constAndVariableWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? $A = 0; define('A', 0); A; $A; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* var = top->localDeclarations().first();
    QCOMPARE(var->uses().keys().count(), 1);
    QCOMPARE(var->uses().values().count(), 1);
    QCOMPARE(var->uses().values().first().count(), 1);
    QCOMPARE(var->uses().values().first().first(), SimpleRange(0, 30, 0, 32));

    Declaration* cnst = top->localDeclarations().at(1);
    QCOMPARE(cnst->uses().keys().count(), 1);
    QCOMPARE(cnst->uses().values().count(), 1);
    QCOMPARE(cnst->uses().values().first().count(), 1);
    QCOMPARE(cnst->uses().values().first().first(), SimpleRange(0, 27, 0, 28));

    release(top);
}

}

#include "test_uses.moc"
