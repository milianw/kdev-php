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

void compareUses(Declaration* dec, QList<SimpleRange> ranges)
{
    kDebug() << "comparing uses for" << dec->toString();
    QCOMPARE(dec->uses().keys().count(), 1);
    QCOMPARE(dec->uses().values().count(), 1);
    QCOMPARE(dec->uses().values().first().count(), ranges.count());
    for (int i = 0; i < ranges.count(); ++i) {
        QCOMPARE(dec->uses().values().first().at(i), ranges.at(i));
    }
}

void compareUses(Declaration* dec, SimpleRange range)
{
    QList<SimpleRange> r;
    r << range;
    compareUses(dec, r);
}

TestUses::TestUses()
{
}

void TestUses::newObject()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class Foo {} $a = new Foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/newObject.php");
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    compareUses(top->localDeclarations().first(), SimpleRange(0, 25, 0, 28));
    QCOMPARE(top->localDeclarations().first()->uses().keys().first(), IndexedString("/usestest/newObject.php"));
}

void TestUses::functionCall()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? function foo() {} foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/functionCall.php");
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* fun = top->localDeclarations().first();
    compareUses(fun, SimpleRange(0, 21, 0, 24));
    QCOMPARE(fun->uses().keys().first(), IndexedString("/usestest/functionCall.php"));
}

void TestUses::memberFunctionCall()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function foo() {} } $a = new A(); $a->foo(); ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/memberFunctionCall.php");
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* fun = top->childContexts().first()->localDeclarations().first();
    compareUses(fun, SimpleRange(0, 51, 0, 54));
    QCOMPARE(fun->uses().keys().first(), IndexedString("/usestest/memberFunctionCall.php"));
}

void TestUses::memberVariable()
{

    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public $foo; } $a = new A(); $a->foo; ");
    TopDUContext* top = parse(method, DumpNone, "/usestest/memberVariable.php");
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    Declaration* var = top->childContexts().first()->localDeclarations().first();
    compareUses(var, SimpleRange(0, 46, 0, 49));
    QCOMPARE(var->uses().keys().first(), IndexedString("/usestest/memberVariable.php"));
}

void TestUses::variable()
{
    //                        0         1         2         3         4         5         6         7
    //                        01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<?php\nclass A { public $foo; } $a = new A(); $a; $a->foo; foo($a); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    QList<SimpleRange> ranges;
    ranges << SimpleRange(1, 42 - 3, 1, 44 - 3) << SimpleRange(1, 46 - 3, 1, 48 - 3) << SimpleRange(1, 59 - 3, 1, 61 - 3);
    compareUses(top->localDeclarations().at(1), ranges);
}

void TestUses::varInString()
{

    //                  0         1         2         3         4         5         6         7
    //                  01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<?php $a=0; \"$a {$a}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());
    QList<SimpleRange> ranges;
    ranges << SimpleRange(0, 13, 0, 15) << SimpleRange(0, 17, 0, 19);
    compareUses(top->localDeclarations().at(0), ranges);
}

void TestUses::memberVarInString()
{

    //                 0         1         2         3         4          5         6          7
    //                 01234567890123456789012345678901234567890123456789 01234567890123 4567890123456789
    QByteArray method("<?php class A { public $v=0; } $a=new A(); $a->v; \"$a->v {$a->v}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    QList<SimpleRange> ranges;
    ranges << SimpleRange(0, 43, 0, 45) << SimpleRange(0, 51, 0, 53) << SimpleRange(0, 58, 0, 60);
    compareUses(top->localDeclarations().at(1), ranges);

    ranges.clear();
    ranges << SimpleRange(0, 47, 0, 48) << SimpleRange(0, 55, 0, 56) << SimpleRange(0, 62, 0, 63);
    compareUses(top->childContexts().first()->localDeclarations().first(), ranges);
}

void TestUses::memberFunctionInString()
{

    //                 0         1         2         3         4          5          6         7
    //                 012345678901234567890123456789012345678901234567 890123456789 01234567890123456789
    QByteArray method("<?php class A { function foo() {} } $a=new A(); \"{$a->foo()}\"; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    //$a
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 50, 0, 52));

    //foo
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 54, 0, 57));
}

void TestUses::variableTwoDeclarations()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } $a = 'a'; $a; $a = 0; $a; $a = 'x'; $a; ");
    //                                15        25  29      37  41        51
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(1), SimpleRange(0, 25, 0, 27));

    //$a = 0 is a new declaration!
    compareUses(top->localDeclarations().at(2), SimpleRange(0, 37, 0, 39));
}

void TestUses::variableTwoDeclarationsInFunction()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? function foo() { $a='a'; $a; $a=0; $a; $a=false; }");
    //                                     20      28  32    38  42

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->childContexts().at(1)->localDeclarations().at(0), SimpleRange(0, 28, 0, 30));

    //$a = 0 is a new declaration!
    compareUses(top->childContexts().at(1)->localDeclarations().at(1), SimpleRange(0, 38, 0, 40));
}

void TestUses::classExtends()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } class B extends A { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(0), SimpleRange(0, 31, 0, 32));
}

void TestUses::classImplements()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } class B implements A { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(0), SimpleRange(0, 38, 0, 39));
}

void TestUses::classImplementsMultiple()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B { } class C implements A, B { } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(0), SimpleRange(0, 54, 0, 55));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 57, 0, 58));
}

void TestUses::interfaceExtends()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B extends A { }");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(0), SimpleRange(0, 39, 0, 40));
}

void TestUses::interfaceExtendsMultiple()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface A { } interface B { } interface C extends A, B { }");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().at(0), SimpleRange(0, 55, 0, 56));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 58, 0, 59));
}

void TestUses::staticMemberFunctionCall()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public static function foo() {} } A::foo(); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 47, 0, 48));
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 50, 0, 53));
}

void TestUses::staticMemberVariable()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { public static $foo; } $foo=0; A::$foo; $foo;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 43, 0, 44));
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 46, 0, 50));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 52, 0, 56));
}

void TestUses::constant()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? define('A', 'foo'); echo A;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 28, 0, 29));
}

void TestUses::classConstant()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { const FOO = 'abc'; } echo A::FOO;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 39, 0, 40));
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 42, 0, 45));
}

void TestUses::classParent()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function x() {} } class B extends A { function x() { parent::x(); }} ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    QList<SimpleRange> range;
    range << SimpleRange(0, 47, 0, 48);
    range << SimpleRange(0, 66, 0, 72);
    compareUses(top->localDeclarations().first(), range);

    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 74, 0, 75));
}

void TestUses::classSelf()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function x() { self::x(); } } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 28, 0, 32));
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 34, 0, 35));
}
void TestUses::classThis()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { function x() { $this->x(); } } ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 28, 0, 33));
    compareUses(top->childContexts().first()->localDeclarations().first(), SimpleRange(0, 35, 0, 36));
}

void TestUses::objectWithClassName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class Aa { public static $i; const j=0; public $k; } $Aa = new Aa; $Aa->k; Aa::j; Aa::$i;");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    QList<SimpleRange> ranges;
    ranges << SimpleRange(0, 66, 0, 66 + 2);
    ranges << SimpleRange(0, 78, 0, 78 + 2);
    ranges << SimpleRange(0, 85, 0, 85 + 2);
    compareUses(top->localDeclarations().first(), ranges);

    compareUses(top->localDeclarations().at(1), SimpleRange(0, 70, 0, 70 + 3));
}

void TestUses::classAndConstWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } define('A', 0); A; new A; define('B', 0); class B { } new B; B; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 38, 0, 39));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 31, 0, 32));
    compareUses(top->localDeclarations().at(2), SimpleRange(0, 76, 0, 77));
    compareUses(top->localDeclarations().at(3), SimpleRange(0, 73, 0, 74));
}


void TestUses::classAndFunctionWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? class A { } function A() {} new A; A(); ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 35, 0, 36));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 38, 0, 39));
}

void TestUses::constAndVariableWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? $A = 0; define('A', 0); A; $A; ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    compareUses(top->localDeclarations().first(), SimpleRange(0, 30, 0, 32));
    compareUses(top->localDeclarations().at(1), SimpleRange(0, 27, 0, 28));
}

void TestUses::functionAndClassWithSameName()
{
    //                 0         1         2         3         4         5         6         7         8
    //                 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? interface foo { function asdf(); } class asdf {} class bar extends asdf implements foo {} ");
    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* fnAsdf = top->childContexts().first()->localDeclarations().first();
    QCOMPARE(fnAsdf->uses().keys().count(), 0);

    compareUses(top->localDeclarations().at(1), SimpleRange(0, 70, 0, 74));
}

void TestUses::constantInClassMember()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? define('TEST', 1); class A { var $a = TEST; var $b = array( TEST ); } TEST;");

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration* constant = top->findDeclarations(Identifier("TEST")).first();

    QList<SimpleRange> uses;
    uses << SimpleRange(0, 41, 0, 45);
    uses << SimpleRange(0, 63, 0, 67);
    uses << SimpleRange(0, 73, 0, 77);
    compareUses(constant, uses);
}

void TestUses::useInAsignment()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? $a = 0; $b = $a; ");

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration *d = top->localDeclarations().first();
    compareUses(d, SimpleRange(0, 16, 0, 18));
}

void TestUses::foreachArray()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? $a = array(1); foreach($a as $k=>$i) { var_dump($k, $i); } ");

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    // $a, $k, $i
    QCOMPARE(top->localDeclarations().size(), 3);

    // $a
    Declaration *d = top->localDeclarations().at(0);
    compareUses(d, SimpleRange(0, 26, 0, 28));

    // $k
    d = top->localDeclarations().at(1);
    compareUses(d, SimpleRange(0, 51, 0, 53));

    // $i
    d = top->localDeclarations().at(2);
    compareUses(d, SimpleRange(0, 55, 0, 57));
}

void TestUses::assignmentToMemberArray()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<?php class x { var $y; function z($a) { $b = $a; $this->y[$a] = true; } }");

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    // class x
    Declaration *x = top->localDeclarations().first();
    QVERIFY(x);

    // $this
    compareUses(x, SimpleRange(0, 50, 0, 55));

    // var $y
    Declaration *y = x->logicalInternalContext(top)->findDeclarations(Identifier("y")).first();
    QVERIFY(y);

    // $this->y
    compareUses(y, SimpleRange(0, 57, 0, 58));

    // function z
    Declaration *z = x->logicalInternalContext(top)->findDeclarations(Identifier("z")).first();
    QVERIFY(z);

    // $a
    Declaration *a = z->logicalInternalContext(top)->findDeclarations(Identifier("a")).first();
    QVERIFY(a);
    compareUses(a, QList<SimpleRange>()
                // $b = $a
                << SimpleRange(0, 46, 0, 48)
                // $this->y[$a]
                << SimpleRange(0, 59, 0, 61)
               );
}

void TestUses::functionParamNewDeclaration()
{
    //                 0         1         2         3         4         5         6         7
    //                 01234567890123456789012345678901234567890123456789012345678901234567890123456789
    QByteArray method("<? function foo($a) { $a; $a = 0; }");

    TopDUContext* top = parse(method, DumpAll);
    DUChainReleaser releaseTop(top);
    DUChainWriteLocker lock(DUChain::lock());

    Declaration *d = top->childContexts().first()->localDeclarations().first();
    compareUses(d, SimpleRange(0, 22, 0, 24));
}

}


#include "test_uses.moc"
