#include <iostream>

#include <math.h>
#include "MathMatrix.hpp"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MathMatrixTest)
{
};

TEST(MathMatrixTest, equalityTest)
{
    for(double i = 0.0; i < 10.0; i++) {

        MATH::Matrix<double> m1(100, 100, 0.5 + i);
        MATH::Matrix<double> m2(100, 100, 0.5 + i);

        CHECK_EQUAL_TEXT(true, (m1 == m1), "Matrix does not equal its self?");
        CHECK_EQUAL_TEXT(true, (m1 == m2), "Matrix does not equal same matrix data");

        MATH::Matrix<double> blank;
        blank = m2;

        CHECK_EQUAL_TEXT(true, (blank == m2), "Matrix does not equal same matrix data post =");
    }
}


TEST(MathMatrixTest, scalarOpsTestSimple)
{
    MATH::Matrix<double> start(10,10, 4.0);
    MATH::Matrix<double> result;


    MATH::Matrix<double> multTarget(10, 10, 16.0);
    result = start * 4.0;

    CHECK_EQUAL_TEXT(true, (multTarget == result), "Result matrix does not match expected output post-scalar multiplication");

    MATH::Matrix<double> addTarget(10, 10, 8.3);
    result = start + 4.3;

    CHECK_EQUAL_TEXT(true, (addTarget == result), "Result matrix does not match expected output post-scalar addition");

    MATH::Matrix<double> subTarget(10, 10, 1.0);
    result = start -3;

    CHECK_EQUAL_TEXT(true, (subTarget == result), "Result matrix does not match expected output post-scalar subtraction");


    CHECK_EQUAL_TEXT(true, 
    (MATH::Matrix<double>(10, 10, 16.0) ==
         (MATH::Matrix<double>(10, 10, 4.0) *= 4.0)
            ), "Result matrix does not match expected output post-scalar multiplication");

    CHECK_EQUAL_TEXT(true, 
    (MATH::Matrix<double>(10, 10, 8.3) == 
        (MATH::Matrix<double>(10, 10, 4.0) += 4.3)
            ), "Result matrix does not match expected output post-scalar addition");

    CHECK_EQUAL_TEXT(true, 
    (MATH::Matrix<double>(10, 10, 1.0) == 
        (MATH::Matrix<double>(10, 10, 4.0) -= 3.0)
            ), "Result matrix does not match expected output post-scalar subtraction");

}

TEST(MathMatrixTest, matOpsTestSimple)
{
    MATH::Matrix<double> start(10,10, 4.0);
    MATH::Matrix<double> result;


    MATH::Matrix<double> multTarget(10, 10, 136);

    result = start * MATH::Matrix<double>(10, 10, 3.4);

    CHECK_EQUAL_TEXT(true, (multTarget == result), "Result matrix does not match expected output post-matrix multiplication");


    MATH::Matrix<double> addTarget(10, 10, 8.3);
    result = start + MATH::Matrix<double>(10, 10, 4.3);

    CHECK_EQUAL_TEXT(true, (addTarget == result), "Result matrix does not match expected output post-matrix addition");


    MATH::Matrix<double> subTarget(10, 10, 1.0);
    result = start - MATH::Matrix<double>(10, 10, 3.0);

    CHECK_EQUAL_TEXT(true, (subTarget == result), "Result matrix does not match expected output post-matrix subtraction");


    CHECK_EQUAL_TEXT(true, 
        (MATH::Matrix<double>(10, 10, 136) == 
            (MATH::Matrix<double>(10,10, 4.0) *= MATH::Matrix<double>(10,10, 3.4))
            ), "Result matrix does not match expected output post-matrix multiplication");


    CHECK_EQUAL_TEXT(true, 
        (MATH::Matrix<double>(10, 10, 8.3) == 
            (MATH::Matrix<double>(10,10, 4.0) += MATH::Matrix<double>(10,10, 4.3))
            ), "Result matrix does not match expected output post-matrix addition");

    CHECK_EQUAL_TEXT(true, 
        (MATH::Matrix<double>(10, 10, 1.0) == 
            (MATH::Matrix<double>(10,10, 4.0) -= MATH::Matrix<double>(10,10, 3.0))
            ), "Result matrix does not match expected output post-matrix subtraction");
}


TEST(MathMatrixTest, transposeTest)
{
    for(int dim = 2; dim < 9; dim ++) {

        MATH::Matrix<double> start(dim, dim, 0.0);

        for(int i = 0; i < dim; i++) {
            start[0][i] = 1.0;
            start[i][1] = 1.0;
        }

        MATH::Matrix<double> start1 = start;

        // Manually transpose
        MATH::Matrix<double> target(dim, dim, 0.0);

        for(int i = 0; i < dim; i++) {
            target[i][0] = 1.0;
            target[1][i] = 1.0;
        }

        start.transpose();  // Initial transpose

        CHECK_EQUAL_TEXT(true, (start == target), "Transposed matrix did not transpose correctly");

        start.transpose(); // Secondary transpose should yeild original matrix

        CHECK_EQUAL_TEXT(true, (start == start1), "Transposed matrix did not transpose correctly");


        // Check non-square matrix

        MATH::Matrix<double> s1(4, dim, 0.0);

        for(int i = 0; i < dim; i++) {
            s1[2][i] = 1.0;
        }

        MATH::Matrix<double> s1Save = s1;


        MATH::Matrix<double> s1Target(dim, 4, 0.0);

        for(int i = 0; i < dim; i++) {
            s1Target[i][2] = 1.0;
        }

        s1.transpose(); // Initial transpose

        CHECK_EQUAL_TEXT(true, (s1 == s1Target), "Transposed matrix did not transpose correctly");

        s1.transpose(); // Secondary transpose should yeild original matrix

        CHECK_EQUAL_TEXT(true, (s1 == s1Save), "Transposed matrix did not transpose correctly");
    }
}

TEST(MathMatrixTest, rotationTest)
{
    MATH::Matrix<double> rz(3, 3, 0.0);

    // Rotate z by 90
    rz[0][1] = -1;
    rz[1][0] =  1;
    rz[2][2] =  1; 

    MATH::Matrix<double> colVec(3, 1, 0.0);
    colVec[0][0] = 1;

    MATH::Matrix<double> colVecRes(3, 1, 0.0);
    colVecRes[1][0] = 1;

    CHECK_EQUAL_TEXT(true, (colVecRes == rz * colVec), "Rotational check yeilded incorrect result");
}