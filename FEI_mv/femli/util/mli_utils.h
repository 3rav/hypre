/*BHEADER**********************************************************************
 * (c) 2001   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 *********************************************************************EHEADER*/

/******************************************************************************
 *
 * Utility functions 
 *
 *****************************************************************************/

#ifndef __MLIUTILS__
#define __MLIUTILS__

#include <time.h>
#include "utilities/utilities.h"
typedef struct MLI_Function_Struct MLI_Function;
#include "parcsr_mv/parcsr_mv.h"
#include "parcsr_ls/HYPRE_parcsr_ls.h"
#include "parcsr_mv/HYPRE_parcsr_mv.h"
#include "krylov/krylov.h"
#include "cintface/cmli.h"

/************************************************************************
 * place holder for function pointers
 *----------------------------------------------------------------------*/

struct MLI_Function_Struct
{
   int (*func_)();
};

/************************************************************************
 * Utility function definitions
 *----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

int    MLI_Utils_HypreParCSRMatrixGetDestroyFunc(MLI_Function *funcPtr);
int    MLI_Utils_HypreCSRMatrixGetDestroyFunc(MLI_Function *funcPtr);
int    MLI_Utils_HypreParVectorGetDestroyFunc(MLI_Function *funcPtr);
int    MLI_Utils_HypreVectorGetDestroyFunc(MLI_Function *funcPtr);
int    MLI_Utils_HypreMatrixFormJacobi(void *A, double, void **J);
int    MLI_Utils_GenPartition(MPI_Comm comm, int n, int **part);
int    MLI_Utils_ComputeSpectralRadius(hypre_ParCSRMatrix *, double *);
int    MLI_Utils_ComputeExtremeRitzValues(hypre_ParCSRMatrix *, double *, int);
int    MLI_Utils_ComputeMatrixMaxNorm(hypre_ParCSRMatrix *, double *, int);
double MLI_Utils_WTime();
int    MLI_Utils_HypreMatrixPrint(void *, char *);
int    MLI_Utils_HypreMatrixGetInfo(void *, int *, double *);
int    MLI_Utils_HypreMatrixComputeRAP(void *P, void *A, void **RAP);
int    MLI_Utils_HypreMatrixCompress(void *A, int blksize, void **A2);
int    MLI_Utils_HypreBoolMatrixDecompress(void *S, int, void **S2, void *A);
int    MLI_Utils_QR(double *Q, double *R, int nrows, int ncols);
int    MLI_Utils_HypreMatrixReadTuminFormat(char *filename, MPI_Comm comm, 
                 int blksize, void **mat, int flag, double **scaleVec);
int    MLI_Utils_HypreMatrixReadIJAFormat(char *filename, MPI_Comm comm, 
                 int blksize, void **mat, int flag, double **scaleVec);
int    MLI_Utils_HypreParMatrixReadIJAFormat(char *filename, MPI_Comm comm, 
                 void **mat, int flag, double **scaleVec);
int    MLI_Utils_DoubleVectorRead(char *, MPI_Comm, int, int, double *vec);
int    MLI_Utils_DoubleParVectorRead(char *, MPI_Comm, int, int, double *vec);
int    MLI_Utils_ParCSRMLISetup(HYPRE_Solver, HYPRE_ParCSRMatrix, 
                                HYPRE_ParVector, HYPRE_ParVector);
int    MLI_Utils_ParCSRMLISolve(HYPRE_Solver, HYPRE_ParCSRMatrix, 
                                HYPRE_ParVector, HYPRE_ParVector);
int    MLI_Utils_HyprePCGSolve(CMLI *, HYPRE_Matrix, HYPRE_Vector, 
                               HYPRE_Vector);
int    MLI_Utils_HypreGMRESSolve(CMLI *, HYPRE_Matrix, HYPRE_Vector, 
                                 HYPRE_Vector);
int    MLI_Utils_HypreBiCGSTABSolve(CMLI *, HYPRE_Matrix, HYPRE_Vector, 
                                    HYPRE_Vector);
int    MLI_Utils_BinarySearch(int, int *, int);
int    MLI_Utils_IntQSort2(int *, int *, int, int);
int    MLI_Utils_IntQSort2a(int *, double *, int, int);
int    MLI_Utils_IntMergeSort(int nlist, int *listLengs, int **lists,
                              int **list2, int *newNList, int **newList); 
int    MLI_Utils_DenseMatrixInverse(double **Amat, int ndim, double ***Bmat);
int    MLI_Utils_DenseMatvec(double **Amat, int ndim, double *x, double *Ax);

#ifdef __cplusplus
}
#endif

#endif

