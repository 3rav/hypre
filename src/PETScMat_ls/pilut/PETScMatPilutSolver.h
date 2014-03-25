/*BHEADER**********************************************************************
 * Copyright (c) 2008,  Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * This file is part of HYPRE.  See file COPYRIGHT for details.
 *
 * HYPRE is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (as published by the Free
 * Software Foundation) version 2.1 dated February 1999.
 *
 * $Revision$
 ***********************************************************************EHEADER*/




/******************************************************************************
 *
 * Header info for the hypre_StructSolver structures
 *
 *****************************************************************************/

#ifndef hypre_PETSC_MAT_PILUT_SOLVER_HEADER
#define hypre_PETSC_MAT_PILUT_SOLVER_HEADER

#include <HYPRE_config.h>

#include "general.h"
#include "_hypre_utilities.h"
#ifdef HYPRE_DEBUG
#include <gmalloc.h>
#endif

/* Include Petsc matrix and vector headers */
#include "mat.h"
#include "vec.h"

#include "HYPRE.h"

/* types of objects declared in this directory */
#include "HYPRE_PETScMatPilutSolver_types.h"

/* type declaration for matrix object used by the implementation in this directory */
#include "HYPRE_distributed_matrix_types.h"

/* type declaration and prototypes for solvers used in this implementation */
#include "HYPRE_DistributedMatrixPilutSolver_types.h"
#include "HYPRE_DistributedMatrixPilutSolver_protos.h"

/*--------------------------------------------------------------------------
 * hypre_PETScMatPilutSolver
 *--------------------------------------------------------------------------*/

typedef struct
{

  MPI_Comm        comm;

  /* Petsc Matrix that defines the system to be solved */
  Mat             Matrix;

  /* This solver is a wrapper for DistributedMatrixPilutSolver; */
  HYPRE_DistributedMatrixPilutSolver DistributedSolver;

} hypre_PETScMatPilutSolver;

/*--------------------------------------------------------------------------
 * Accessor macros: hypre_PETScMatPilutSolver
 *--------------------------------------------------------------------------*/

#define hypre_PETScMatPilutSolverComm(parilut_data)        ((parilut_data) -> comm)
#define hypre_PETScMatPilutSolverMatrix(parilut_data)\
                                         ((parilut_data) -> Matrix)
#define hypre_PETScMatPilutSolverDistributedSolver(parilut_data)\
                                         ((parilut_data) -> DistributedSolver)

/* Include internal prototypes */
#include "./hypre_protos.h"
#include "./internal_protos.h"


#endif
