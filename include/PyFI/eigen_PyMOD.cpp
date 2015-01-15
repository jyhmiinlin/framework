/**
    \file BASIC_PYMOD.CPP
    \author Nick Zwart
    \date 2014jul29

    \brief A template module for the most common use case.

 **/

#include "PyFI/PyFI.h" /* PyFI interface, must be the first include */
using namespace PyFI; /* for PyFI::Array */
PYFI_FUNC(printmat)
{
    PYFI_START(); /* This must be the first line */

    /***** ARGS */   
    PYFI_POSARG(Array<float>, A);
    PyFEigen::PrintArrayAsEigenMat(*A);
    PYFI_END(); /* This must be the last line */
}

PYFI_FUNC(pinv)
{
    PYFI_START(); /* This must be the first line */

    /***** ARGS */   
    PYFI_POSARG(Array<float>, A);
    std::vector<uint64_t> dims = A->dimensions_vector();
    int m = dims[0];
    int n = dims[1];
    PYFI_SETOUTPUT_ALLOC(Array<float>, B, ArrayDimensions(n, m));
    PyFEigen::PseudoInverse(*A,*B);
   
    PYFI_END(); /* This must be the last line */
}

PYFI_FUNC(solve)
{
    PYFI_START(); /* This must be the first line */

    /***** POSITIONAL ARGS */   
    PYFI_POSARG(Array<float>, A); 
    std::vector<uint64_t> dims = A->dimensions_vector();
    // int m = dims[1];
    int n = dims[0];

    PYFI_POSARG(Array<float>, B); 
    dims = B->dimensions_vector();
    // int m_ = dims[1];
    int p = dims[0];

    // TODO: check here if m == m_

    PYFI_SETOUTPUT_ALLOC(Array<float>, X, ArrayDimensions(p,n));

    PyFEigen::MLDivide(*A, *B, *X);

    PYFI_END(); /* This must be the last line */
}
/* ##############################################################
 *                  MODULE DESCRIPTION
 * ############################################################## */


/* list of functions to be accessible from python */
PYFI_LIST_START_
    PYFI_DESC(printmat, "Convert to Eigen Mat and Print")
    PYFI_DESC(pinv, "PseudoInverse")
    PYFI_DESC(solve, "Least Squares Solver (SVD)")
PYFI_LIST_END_
