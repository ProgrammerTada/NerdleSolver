// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#include <gtest/gtest.h>
#include <string>
#include "./NerdleSolver.h"

TEST(NerdleSolver, checkSyntax) {
    NerdleSolver solver(8);
    string correctEq = "45+45=90";
    string doubleOp = "45++5=90";
    string leadingZero = "05+45=50";
    string wrongLength = "29+2=2";
    string correctEq2 = "0+10-9=1";
    string correctEqButFlipped = "1=0+10-9";
    ASSERT_EQ(solver.checkSyntax(correctEq), true);
    ASSERT_EQ(solver.checkSyntax(doubleOp), false);
    ASSERT_EQ(solver.checkSyntax(leadingZero), false);
    ASSERT_EQ(solver.checkSyntax(wrongLength), false);
    ASSERT_EQ(solver.checkSyntax(correctEq2), true);
    ASSERT_EQ(solver.checkSyntax(correctEqButFlipped), false);
}

TEST(NerdleSolver, checkCorrectEquation) {
    NerdleSolver solver(8);
    string eq = "505/92=6";
    ASSERT_EQ(solver.safeDivision(505,92), false);
    ASSERT_EQ(solver.checkCorrectEquation(eq), false);
}
