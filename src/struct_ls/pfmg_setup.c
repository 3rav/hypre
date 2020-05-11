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

#include "_hypre_struct_ls.h"
#include "pfmg.h"

#define DEBUG 0

#define hypre_PFMGSetCIndex(cdir, cindex)       \
   {                                            \
      hypre_SetIndex(cindex, 0);                \
      hypre_IndexD(cindex, cdir) = 0;           \
   }

#define hypre_PFMGSetFIndex(cdir, findex)       \
   {                                            \
      hypre_SetIndex(findex, 0);                \
      hypre_IndexD(findex, cdir) = 1;           \
   }

#define hypre_PFMGSetStride(cdir, stride)       \
   {                                            \
      hypre_SetIndex(stride, 1);                \
      hypre_IndexD(stride, cdir) = 2;           \
   }

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

HYPRE_Int
hypre_PFMGSetup( void               *pfmg_vdata,
                 hypre_StructMatrix *A,
                 hypre_StructVector *b,
                 hypre_StructVector *x        )
{
   hypre_PFMGData       *pfmg_data = (hypre_PFMGData *) pfmg_vdata;

   MPI_Comm              comm              = (pfmg_data -> comm);
   HYPRE_Int             relax_type        = (pfmg_data -> relax_type);
   HYPRE_Int             usr_jacobi_weight = (pfmg_data -> usr_jacobi_weight);
   HYPRE_Real            jacobi_weight     = (pfmg_data -> jacobi_weight);
   HYPRE_Int             skip_relax        = (pfmg_data -> skip_relax);
   HYPRE_Real           *dxyz              = (pfmg_data -> dxyz);
   HYPRE_Int             rap_type;

   HYPRE_Int             max_iter;
   HYPRE_Int             max_levels;
   HYPRE_Int             num_levels;

   hypre_Index           cindex;
   hypre_Index           stride;
   hypre_Index           periodic;

   HYPRE_Int            *cdir_l;
   HYPRE_Int            *active_l;
   hypre_StructGrid    **grid_l;

   HYPRE_Real           *relax_weights;

   hypre_StructMatrix  **A_l;
   hypre_StructMatrix  **P_l;
   hypre_StructMatrix  **RT_l;
   hypre_StructVector  **b_l;
   hypre_StructVector  **x_l;

   /* temp vectors */
   hypre_StructVector  **tx_l;
   hypre_StructVector  **r_l;
   hypre_StructVector  **e_l;

   void                **relax_data_l;
   void                **matvec_data_l;
   void                **restrict_data_l;
   void                **interp_data_l;

   hypre_StructGrid     *grid;

   hypre_Box            *cbox;

   HYPRE_Int             cdir, cmaxsize;
   HYPRE_Int             l;
   HYPRE_Int             dxyz_flag;

   HYPRE_Int             b_num_ghost[]  = {0, 0, 0, 0, 0, 0};
   HYPRE_Int             x_num_ghost[]  = {1, 1, 1, 1, 1, 1};

#if DEBUG
   hypre_StructVector   *ones  = NULL;
   hypre_StructVector   *Pones = NULL;
   char                  filename[255];
#endif


   /*-----------------------------------------------------
    * Set up coarse grids
    *-----------------------------------------------------*/
   grid  = hypre_StructMatrixGrid(A);

   /* Initialize periodic */
   hypre_CopyIndex(hypre_StructGridPeriodic(grid), periodic);

   /* Compute a new max_levels value based on the grid */
   hypre_PFMGComputeMaxLevels(grid, &max_levels);
   if ((pfmg_data -> max_levels) > 0)
   {
      (pfmg_data -> max_levels) = hypre_min((pfmg_data -> max_levels), max_levels);
      max_levels = hypre_min((pfmg_data -> max_levels), max_levels);
   }

   /* compute dxyz */
   hypre_PFMGComputeDxyz(A, dxyz, &dxyz_flag);

   /* Run coarsening */
   cbox = hypre_BoxClone(hypre_StructGridBoundingBox(grid));
   hypre_PFMGCoarsen(cbox, periodic, max_levels, dxyz_flag, dxyz, &cdir_l,
                     &active_l, &relax_weights, &num_levels);
   cmaxsize = hypre_BoxMaxSize(cbox);
   hypre_BoxDestroy(cbox);

   /* set all levels active if skip_relax = 0 */
   if (!skip_relax)
   {
      for (l = 0; l < num_levels; l++)
      {
         active_l[l] = 1;
      }
   }

   (pfmg_data -> num_levels) = num_levels;
   (pfmg_data -> cdir_l)     = cdir_l;

   /*-----------------------------------------------------
    * Set up matrix and vector structures
    *-----------------------------------------------------*/

   /*-----------------------------------------------------
    * Modify the rap_type if red-black Gauss-Seidel is
    * used. Red-black gs is used only in the non-Galerkin
    * case.
    *-----------------------------------------------------*/
   if (relax_type == 2 || relax_type == 3)   /* red-black gs */
   {
      (pfmg_data -> rap_type)= 1;
   }
   rap_type = (pfmg_data -> rap_type);

   grid_l = hypre_TAlloc(hypre_StructGrid *, num_levels);
   A_l    = hypre_TAlloc(hypre_StructMatrix *, num_levels);
   P_l    = hypre_TAlloc(hypre_StructMatrix *, num_levels - 1);
   RT_l   = hypre_TAlloc(hypre_StructMatrix *, num_levels - 1);
   b_l    = hypre_TAlloc(hypre_StructVector *, num_levels);
   x_l    = hypre_TAlloc(hypre_StructVector *, num_levels);
   tx_l   = hypre_TAlloc(hypre_StructVector *, num_levels);
   r_l    = tx_l;
   e_l    = tx_l;

   hypre_StructGridRef(grid, &grid_l[0]);
   A_l[0] = hypre_StructMatrixRef(A);
   b_l[0] = hypre_StructVectorRef(b);
   x_l[0] = hypre_StructVectorRef(x);

   tx_l[0] = hypre_StructVectorCreate(comm, grid_l[0]);
   hypre_StructVectorSetNumGhost(tx_l[0], x_num_ghost);
   hypre_StructVectorInitialize(tx_l[0]);
   hypre_StructVectorAssemble(tx_l[0]);

//   /* RDF AP Debug */
//   hypre_StructAssumedPartitionPrint("zAP", hypre_BoxManAssumedPartition(
//                                        hypre_StructGridBoxMan(grid_l[0])));

   for (l = 0; l < (num_levels - 1); l++)
   {
      cdir = cdir_l[l];

      hypre_PFMGSetCIndex(cdir, cindex);
      hypre_PFMGSetStride(cdir, stride);

      /* set up interpolation and restriction operators */
      P_l[l] = hypre_zPFMGCreateInterpOp(A_l[l], cdir, stride);
      RT_l[l] = P_l[l];
#if 0 /* TODO: Allow RT != P */
      if (nonsymmetric_cycle)
      {
         RT_l[l] = hypre_zPFMGCreateRestrictOp(A_l[l], cdir, stride);
      }
#endif
      HYPRE_StructMatrixSetTranspose(RT_l[l], 1);
      hypre_StructMatrixInitialize(P_l[l]);
      hypre_zPFMGSetupInterpOp(P_l[l], A_l[l], cdir);
#if 0 /* TODO: Allow RT != P */
      if (nonsymmetric_cycle)
      {
         hypre_StructMatrixInitialize(RT_l[l]);
         hypre_zPFMGSetupRestrictOp(RT_l[l], A_l[l], cdir);
      }
#endif

      if (rap_type == 0)
      {
         HYPRE_Int           nmatrices   = 3;
         HYPRE_StructMatrix  matrices[3] = {A_l[l], P_l[l], RT_l[l]};
         HYPRE_Int           nterms      = 3;
         HYPRE_Int           terms[3]    = {1,0,1};
         HYPRE_Int           trans[3]    = {1,0,0};

         /* If restriction is not the same as interpolation, point to matrices[2] */
         if (RT_l[l] != P_l[l])
         {
            terms[0] = 2;
         }

         hypre_StructMatmult(nmatrices, matrices, nterms, terms, trans, &A_l[l+1]);
         hypre_StructGridRef(hypre_StructMatrixGrid(A_l[l+1]), &grid_l[l+1]);
      }
      else
      {
         /* RDF: The coarse grid should be computed in CreateRAPOp() */
         hypre_PFMGSetCIndex(cdir, cindex);
         hypre_PFMGSetStride(cdir, stride);
         hypre_StructCoarsen(grid_l[l], cindex, stride, 1, &grid_l[l+1]);

         A_l[l+1] = hypre_PFMGCreateRAPOp(RT_l[l], A_l[l], P_l[l], grid_l[l+1], cdir, rap_type);
         hypre_StructMatrixInitialize(A_l[l+1]);
         hypre_PFMGSetupRAPOp(RT_l[l], A_l[l], P_l[l], cdir, cindex, stride, rap_type, A_l[l+1]);
      }

//      /* RDF AP Debug */
//      hypre_StructAssumedPartitionPrint("zAP", hypre_BoxManAssumedPartition(
//                                           hypre_StructGridBoxMan(grid_l[l+1])));

      b_l[l+1] = hypre_StructVectorCreate(comm, grid_l[l+1]);
      hypre_StructVectorSetNumGhost(b_l[l+1], b_num_ghost);
      hypre_StructVectorInitialize(b_l[l+1]);
      hypre_StructVectorAssemble(b_l[l+1]);

      x_l[l+1] = hypre_StructVectorCreate(comm, grid_l[l+1]);
      hypre_StructVectorSetNumGhost(x_l[l+1], x_num_ghost);
      hypre_StructVectorInitialize(x_l[l+1]);
      hypre_StructVectorAssemble(x_l[l+1]);

      tx_l[l+1] = hypre_StructVectorCreate(comm, grid_l[l+1]);
      hypre_StructVectorSetNumGhost(tx_l[l+1], x_num_ghost);
      hypre_StructVectorInitialize(tx_l[l+1]);
      hypre_StructVectorAssemble(tx_l[l+1]);
   }

   (pfmg_data -> grid_l) = grid_l;
   (pfmg_data -> A_l)    = A_l;
   (pfmg_data -> P_l)    = P_l;
   (pfmg_data -> RT_l)   = RT_l;
   (pfmg_data -> b_l)    = b_l;
   (pfmg_data -> x_l)    = x_l;
   (pfmg_data -> tx_l)   = tx_l;
   (pfmg_data -> r_l)    = r_l;
   (pfmg_data -> e_l)    = e_l;

   /*-----------------------------------------------------
    * Set up multigrid operators and call setup routines
    *-----------------------------------------------------*/

   relax_data_l    = hypre_TAlloc(void *, num_levels);
   matvec_data_l   = hypre_TAlloc(void *, num_levels);
   restrict_data_l = hypre_TAlloc(void *, num_levels);
   interp_data_l   = hypre_TAlloc(void *, num_levels);

   for (l = 0; l < (num_levels - 1); l++)
   {
      cdir = cdir_l[l];

      /* set up the interpolation routine */
      interp_data_l[l] = hypre_StructMatvecCreate();
      hypre_StructMatvecSetup(interp_data_l[l], P_l[l], x_l[l+1]);

      /* set up the restriction routine */
      restrict_data_l[l] = hypre_StructMatvecCreate();
      hypre_StructMatvecSetTranspose(restrict_data_l[l], 1);
      hypre_StructMatvecSetup(restrict_data_l[l], RT_l[l], r_l[l]);

      // Check if P interpolates vector of ones
#if DEBUG
      if (ones != NULL)
      {
         HYPRE_StructVectorDestroy(ones);
      }
      HYPRE_StructVectorCreate(comm, grid_l[l+1], &ones);
      HYPRE_StructVectorInitialize(ones);
      HYPRE_StructVectorSetConstantValues(ones, 1.0);
      HYPRE_StructVectorAssemble(ones);

      hypre_sprintf(filename, "pfmg_ones.%02d", l);
      HYPRE_StructVectorPrint(filename, ones, 0);

      if (Pones != NULL)
      {
         HYPRE_StructVectorDestroy(Pones);
      }
      HYPRE_StructVectorCreate(comm, grid_l[l], &Pones);
      HYPRE_StructVectorInitialize(Pones);
      HYPRE_StructVectorAssemble(Pones);

      /* interpolate error and correct (x = Pe_c) */
      hypre_StructMatvecCompute(interp_data_l[l], 1.0, P_l[l], ones, 0.0, Pones);

      hypre_sprintf(filename, "pfmg_Pones.%02d", l);
      HYPRE_StructVectorPrint(filename, Pones, 0);
#endif

   }

   /*-----------------------------------------------------
    * Check for zero diagonal on coarsest grid, occurs with
    * singular problems like full Neumann or full periodic.
    * Note that a processor with zero diagonal will set
    * active_l =0, other processors will not. This is OK
    * as we only want to avoid the division by zero on the
    * one processor which owns the single coarse grid
    * point.
    *-----------------------------------------------------*/

   if ( hypre_ZeroDiagonal(A_l[l]))
   {
      active_l[l] = 0;
   }

   /* set up fine grid relaxation */
   relax_data_l[0] = hypre_PFMGRelaxCreate(comm);
   hypre_PFMGRelaxSetTol(relax_data_l[0], 0.0);
   if (usr_jacobi_weight)
   {
      hypre_PFMGRelaxSetJacobiWeight(relax_data_l[0], jacobi_weight);
   }
   else
   {
      hypre_PFMGRelaxSetJacobiWeight(relax_data_l[0], relax_weights[0]);
   }
   hypre_PFMGRelaxSetType(relax_data_l[0], relax_type);
   hypre_PFMGRelaxSetTempVec(relax_data_l[0], tx_l[0]);
   hypre_PFMGRelaxSetup(relax_data_l[0], A_l[0], b_l[0], x_l[0]);
   if (num_levels > 1)
   {
      for (l = 1; l < num_levels; l++)
      {
         /* set relaxation parameters */
         if (active_l[l])
         {
            relax_data_l[l] = hypre_PFMGRelaxCreate(comm);
            hypre_PFMGRelaxSetTol(relax_data_l[l], 0.0);
            if (usr_jacobi_weight)
            {
               hypre_PFMGRelaxSetJacobiWeight(relax_data_l[l], jacobi_weight);
            }
            else
            {
               hypre_PFMGRelaxSetJacobiWeight(relax_data_l[l], relax_weights[l]);
            }
            hypre_PFMGRelaxSetType(relax_data_l[l], relax_type);
            hypre_PFMGRelaxSetTempVec(relax_data_l[l], tx_l[l]);
         }
      }

      /* change coarsest grid relaxation parameters */
      l = num_levels - 1;
      if (active_l[l])
      {
         HYPRE_Int maxwork, maxiter;
         hypre_PFMGRelaxSetType(relax_data_l[l], 0);
         /* do no more work on the coarsest grid than the cost of a V-cycle
          * (estimating roughly 4 communications per V-cycle level) */
         maxwork = 4*num_levels;
         /* do sweeps proportional to the coarsest grid size */
         maxiter = hypre_min(maxwork, cmaxsize);
#if 0
         hypre_printf("maxwork = %d, cmaxsize = %d, maxiter = %d\n",
                      maxwork, cmaxsize, maxiter);
#endif
         hypre_PFMGRelaxSetMaxIter(relax_data_l[l], maxiter);
      }

      /* call relax setup */
      for (l = 1; l < num_levels; l++)
      {
         if (active_l[l])
         {
            hypre_PFMGRelaxSetup(relax_data_l[l], A_l[l], b_l[l], x_l[l]);
         }
      }
   }
   hypre_TFree(relax_weights);

   for (l = 0; l < num_levels; l++)
   {
      /* set up the residual routine */
      matvec_data_l[l] = hypre_StructMatvecCreate();
      hypre_StructMatvecSetup(matvec_data_l[l], A_l[l], x_l[l]);
   }

   (pfmg_data -> active_l)        = active_l;
   (pfmg_data -> relax_data_l)    = relax_data_l;
   (pfmg_data -> matvec_data_l)   = matvec_data_l;
   (pfmg_data -> restrict_data_l) = restrict_data_l;
   (pfmg_data -> interp_data_l)   = interp_data_l;

   /*-----------------------------------------------------
    * Allocate space for log info
    *-----------------------------------------------------*/

   if ((pfmg_data -> logging) > 0)
   {
      max_iter = (pfmg_data -> max_iter);
      (pfmg_data -> norms)     = hypre_TAlloc(HYPRE_Real, max_iter);
      (pfmg_data -> rel_norms) = hypre_TAlloc(HYPRE_Real, max_iter);
   }

#if DEBUG
   for (l = 0; l < (num_levels - 1); l++)
   {
      hypre_sprintf(filename, "pfmg_A.%02d", l);
      hypre_StructMatrixPrint(filename, A_l[l], 0);
      hypre_sprintf(filename, "pfmg_P.%02d", l);
      hypre_StructMatrixPrint(filename, P_l[l], 0);
   }
   hypre_sprintf(filename, "pfmg_A.%02d", l);
   hypre_StructMatrixPrint(filename, A_l[l], 0);
#endif

   return hypre_error_flag;
}

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

HYPRE_Int
hypre_PFMGComputeMaxLevels( hypre_StructGrid   *grid,
                            HYPRE_Int          *max_levels )
{
   hypre_Box       *bbox;
   HYPRE_Int        d, ndim;

   ndim = hypre_StructGridNDim(grid);
   bbox = hypre_StructGridBoundingBox(grid);

   *max_levels = 0;
   for (d = 0; d < ndim; d++)
   {
      *max_levels += hypre_Log2(hypre_BoxSizeD(bbox, d)) + 2;
   }

   return hypre_error_flag;
}

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

HYPRE_Int
hypre_PFMGComputeDxyz( hypre_StructMatrix *A,
                       HYPRE_Real         *dxyz,
                       HYPRE_Int          *dxyz_flag)
{
   hypre_BoxArray        *compute_boxes;
   hypre_Box             *compute_box;
   hypre_StructGrid      *grid = hypre_StructMatrixGrid(A);

   hypre_Box             *A_dbox;

   HYPRE_Int              Ai;

   HYPRE_Real            *Ap;
   HYPRE_Real             cxyz[3], sqcxyz[3], tcxyz[3];
   HYPRE_Real             cxyz_max;

   HYPRE_Int              tot_size;

   hypre_StructStencil   *stencil;
   hypre_Index           *stencil_shape;
   HYPRE_Int              stencil_size;

   HYPRE_Int              constant_coefficient;

   HYPRE_Int              Astenc;

   hypre_Index            loop_size;
   hypre_IndexRef         start;
   hypre_Index            stride;

   HYPRE_Int              ndim, i, si, d, sdiag;

   HYPRE_Real             cx, cy, cz, sqcx, sqcy, sqcz, tcx, tcy, tcz, diag;
   HYPRE_Real             mean[HYPRE_MAXDIM], deviation[HYPRE_MAXDIM];

   /*----------------------------------------------------------
    * Exit if user gives dxyz different than zero
    *----------------------------------------------------------*/
   if ((dxyz[0] != 0) && (dxyz[1] != 0) && (dxyz[2] != 0))
   {
      *dxyz_flag = 0;

      return hypre_error_flag;
   }

   /*----------------------------------------------------------
    * Initialize some things
    *----------------------------------------------------------*/

   ndim          = hypre_StructMatrixNDim(A);
   stencil       = hypre_StructMatrixStencil(A);
   stencil_shape = hypre_StructStencilShape(stencil);
   stencil_size  = hypre_StructStencilSize(stencil);
   compute_boxes = hypre_StructGridBoxes(grid);
   tot_size      = hypre_StructGridGlobalSize(grid);
   constant_coefficient = hypre_StructMatrixConstantCoefficient(A);

   hypre_SetIndex(stride, 1);

   /*----------------------------------------------------------
    * Compute cxyz (use arithmetic mean)
    *----------------------------------------------------------*/

   cx = 0.0;
   cy = 0.0;
   cz = 0.0;

   sqcx = 0.0;
   sqcy = 0.0;
   sqcz = 0.0;

   /* find diagonal stencil entry */
   for (si = 0; si < stencil_size; si++)
   {
      if ((hypre_IndexD(stencil_shape[si], 0) == 0) &&
          (hypre_IndexD(stencil_shape[si], 1) == 0) &&
          (hypre_IndexD(stencil_shape[si], 2) == 0))
      {
         sdiag = si;
         break;
      }
   }

   hypre_ForBoxI(i, compute_boxes)
   {
      compute_box = hypre_BoxArrayBox(compute_boxes, i);
      A_dbox = hypre_BoxArrayBox(hypre_StructMatrixDataSpace(A), i);
      start  = hypre_BoxIMin(compute_box);
      hypre_BoxGetStrideSize(compute_box, stride, loop_size);

      /* all coefficients constant or variable diagonal */
      if ( constant_coefficient )
      {
         Ai = hypre_CCBoxIndexRank( A_dbox, start );

         tcx = 0.0;
         tcy = 0.0;
         tcz = 0.0;

         /* get sign of diagonal */
         Ap = hypre_StructMatrixBoxData(A, i, sdiag);
         diag = 1.0;
         if (Ap[Ai] < 0)
         {
            diag = -1.0;
         }

         for (si = 0; si < stencil_size; si++)
         {
            Ap = hypre_StructMatrixBoxData(A, i, si);

            /* x-direction */
            Astenc = hypre_IndexD(stencil_shape[si], 0);
            if (Astenc)
            {
               tcx -= Ap[Ai]*diag;
            }

            /* y-direction */
            Astenc = hypre_IndexD(stencil_shape[si], 1);
            if (Astenc)
            {
               tcy -= Ap[Ai]*diag;
            }

            /* z-direction */
            Astenc = hypre_IndexD(stencil_shape[si], 2);
            if (Astenc)
            {
               tcz -= Ap[Ai]*diag;
            }
         }

         cx += tcx;
         cy += tcy;
         cz += tcz;

         sqcx += (tcx*tcx);
         sqcy += (tcy*tcy);
         sqcz += (tcz*tcz);
      }

      /* constant_coefficient==0, all coefficients vary with space */
      else
      {
         hypre_BoxLoop1Begin(ndim, loop_size,
                             A_dbox, start, stride, Ai);
#ifdef HYPRE_USING_OPENMP
#pragma omp parallel for private(HYPRE_BOX_PRIVATE,Ai,si,Ap,diag,Astenc,tcx,tcy,tcz) reduction(+:cx,cy,cz,sqcx,sqcy,sqcz) HYPRE_SMP_SCHEDULE
#endif
         hypre_BoxLoop1For(Ai)
         {
            tcx = 0.0;
            tcy = 0.0;
            tcz = 0.0;

            /* get sign of diagonal */
            Ap = hypre_StructMatrixBoxData(A, i, sdiag);
            diag = 1.0;
            if (Ap[Ai] < 0)
            {
               diag = -1.0;
            }

            for (si = 0; si < stencil_size; si++)
            {
               Ap = hypre_StructMatrixBoxData(A, i, si);

               /* x-direction */
               Astenc = hypre_IndexD(stencil_shape[si], 0);
               if (Astenc)
               {
                  tcx -= Ap[Ai]*diag;
               }

               /* y-direction */
               Astenc = hypre_IndexD(stencil_shape[si], 1);
               if (Astenc)
               {
                  tcy -= Ap[Ai]*diag;
               }

               /* z-direction */
               Astenc = hypre_IndexD(stencil_shape[si], 2);
               if (Astenc)
               {
                  tcz -= Ap[Ai]*diag;
               }
            }

            cx += tcx;
            cy += tcy;
            cz += tcz;

            sqcx += (tcx*tcx);
            sqcy += (tcy*tcy);
            sqcz += (tcz*tcz);
         }
         hypre_BoxLoop1End(Ai);
      }
   }

   cxyz[0] = cx;
   cxyz[1] = cy;
   cxyz[2] = cz;

   sqcxyz[0] = sqcx;
   sqcxyz[1] = sqcy;
   sqcxyz[2] = sqcz;

   /*----------------------------------------------------------
    * Compute dxyz
    *----------------------------------------------------------*/

   /* all coefficients constant or variable diagonal */
   if ( constant_coefficient )
   {
      for (d = 0; d < ndim; d++)
      {
         mean[d]= cxyz[d];
         deviation[d]= sqcxyz[d];
      }
   }
   /* constant_coefficient==0, all coefficients vary with space */
   else
   {

      tcxyz[0] = cxyz[0];
      tcxyz[1] = cxyz[1];
      tcxyz[2] = cxyz[2];
      hypre_MPI_Allreduce(tcxyz, cxyz, 3, HYPRE_MPI_REAL, hypre_MPI_SUM,
                          hypre_StructMatrixComm(A));

      tcxyz[0] = sqcxyz[0];
      tcxyz[1] = sqcxyz[1];
      tcxyz[2] = sqcxyz[2];
      hypre_MPI_Allreduce(tcxyz, sqcxyz, 3, HYPRE_MPI_REAL, hypre_MPI_SUM,
                          hypre_StructMatrixComm(A));

      for (d = 0; d < ndim; d++)
      {
         mean[d]      = cxyz[d]/tot_size;
         deviation[d] = sqcxyz[d]/tot_size;
      }
   }

   cxyz_max = 0.0;
   for (d = 0; d < ndim; d++)
   {
      cxyz_max = hypre_max(cxyz_max, cxyz[d]);
   }
   if (cxyz_max == 0.0)
   {
      cxyz_max = 1.0;
   }

   for (d = 0; d < ndim; d++)
   {
      if (cxyz[d] > 0)
      {
         cxyz[d] /= cxyz_max;
         dxyz[d] = sqrt(1.0 / cxyz[d]);
      }
      else
      {
         dxyz[d] = 1.0e+123;
      }
   }

   *dxyz_flag = 0;
   for (d = 0; d < ndim; d++)
   {
      /* square of coeff. of variation */
      if (deviation[d]/(mean[d]*mean[d]) > 1.1)
      {
         *dxyz_flag = 1;
      }
   }

   return hypre_error_flag;
}

/*--------------------------------------------------------------------------
 * Returns 1 if there is a diagonal coefficient that is zero,
 * otherwise returns 0.
 *--------------------------------------------------------------------------*/

HYPRE_Int
hypre_ZeroDiagonal( hypre_StructMatrix *A )
{
   hypre_StructStencil   *stencil = hypre_StructMatrixStencil(A);

   hypre_BoxArray        *compute_boxes;
   hypre_Box             *compute_box;

   hypre_Index            loop_size;
   hypre_IndexRef         start;
   hypre_Index            stride;

   HYPRE_Real            *Ap;
   hypre_Box             *A_dbox;
   HYPRE_Int              Ai;

   HYPRE_Int              i, si;

   hypre_Index            diag_offset;
   HYPRE_Real             diag_product = 1.0;
   HYPRE_Int              zero_diag = 0;

   /*----------------------------------------------------------
    * Initialize some things
    *----------------------------------------------------------*/

   hypre_SetIndex3(stride, 1, 1, 1);
   hypre_SetIndex3(diag_offset, 0, 0, 0);

   /* Need to modify here */
   compute_boxes = hypre_StructGridBoxes(hypre_StructMatrixGrid(A));
   hypre_ForBoxI(i, compute_boxes)
   {
      compute_box = hypre_BoxArrayBox(compute_boxes, i);
      start  = hypre_BoxIMin(compute_box);
      A_dbox = hypre_BoxArrayBox(hypre_StructMatrixDataSpace(A), i);
      hypre_BoxGetStrideSize(compute_box, stride, loop_size);

      si = hypre_StructStencilOffsetEntry(stencil, diag_offset);
      Ap = hypre_StructMatrixBoxData(A, i, si);
      if (hypre_StructMatrixConstEntry(A, si))
      {
         diag_product = *Ap;
      }
      else
      {
         hypre_BoxLoop1Begin(hypre_StructMatrixNDim(A), loop_size,
                             A_dbox, start, stride, Ai);
#ifdef HYPRE_USING_OPENMP
#pragma omp parallel for private(HYPRE_BOX_PRIVATE,Ai) reduction(*:diag_product) HYPRE_SMP_SCHEDULE
#endif
         hypre_BoxLoop1For(Ai)
         {
            diag_product *= Ap[Ai];
         }
         hypre_BoxLoop1End(Ai);
      }
   }

   if (diag_product == 0)
   {
      zero_diag = 1;
   }

   return zero_diag;
}

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

HYPRE_Int
hypre_PFMGCoarsen( hypre_Box     *cbox,
                   hypre_Index    periodic,
                   HYPRE_Int      max_levels,
                   HYPRE_Int      dxyz_flag,
                   HYPRE_Real    *dxyz,
                   HYPRE_Int    **cdir_l_ptr,
                   HYPRE_Int    **active_l_ptr,
                   HYPRE_Real   **relax_weights_ptr,
                   HYPRE_Int     *num_levels)
{
   HYPRE_Int     *cdir_l;
   HYPRE_Int     *active_l;
   HYPRE_Real    *relax_weights;

   hypre_Index    coarsen;
   hypre_Index    cindex;
   hypre_Index    stride;

   HYPRE_Real     alpha, beta, min_dxyz;
   HYPRE_Int      ndim, d, l, cdir;

   ndim = hypre_BoxNDim(cbox);

   /* Allocate data */
   cdir_l        = hypre_TAlloc(HYPRE_Int, max_levels);
   active_l      = hypre_TAlloc(HYPRE_Int, max_levels);
   relax_weights = hypre_CTAlloc(HYPRE_Real, max_levels);

   /* Force relaxation on finest grid */
   hypre_SetIndex(coarsen, 1);
   for (l = 0; l < max_levels; l++)
   {
      /* Initialize min_dxyz */
      min_dxyz = 1;
      for (d = 0; d < ndim; d++)
      {
         min_dxyz += dxyz[d];
      }

      /* Determine cdir */
      cdir = -1;
      alpha = 0.0;
      for (d = 0; d < ndim; d++)
      {
         if ((hypre_BoxIMaxD(cbox, d) > hypre_BoxIMinD(cbox, d)) &&
             (dxyz[d] < min_dxyz))
         {
            min_dxyz = dxyz[d];
            cdir = d;
         }
         alpha += 1.0/(dxyz[d]*dxyz[d]);
      }
      relax_weights[l] = 1.0;

      /* If it's possible to coarsen, change relax_weights */
      beta = 0.0;
      if (cdir != -1)
      {
         if (dxyz_flag || (ndim == 1))
         {
            relax_weights[l] = 2.0/3.0;
         }
         else
         {
            for (d = 0; d < ndim; d++)
            {
               if (d != cdir)
               {
                  beta += 1.0/(dxyz[d]*dxyz[d]);
               }
            }

            /* determine level Jacobi weights */
            relax_weights[l] = 2.0/(3.0 - beta/alpha);
         }

         /*    don't coarsen if a periodic direction and not divisible by 2
            or don't coarsen if we've reached max_levels*/
         if (((periodic[cdir]) && (periodic[cdir] % 2)) || l == (max_levels - 1))
         {
            cdir = -1;
         }
      }

      /* stop coarsening */
      if (cdir == -1)
      {
         active_l[l] = 1; /* forces relaxation on coarsest grid */
         break;
      }

      cdir_l[l] = cdir;

      if (hypre_IndexD(coarsen, cdir) != 0)
      {
         /* coarsened previously in this direction, relax level l */
         active_l[l] = 1;
         hypre_SetIndex(coarsen, 0);
      }
      else
      {
         active_l[l] = 0;
      }
      hypre_IndexD(coarsen, cdir) = 1;

      /* set cindex, findex, and stride */
      hypre_PFMGSetCIndex(cdir, cindex);
      hypre_PFMGSetStride(cdir, stride);

      /* update dxyz and coarsen cbox*/
      dxyz[cdir] *= 2;
      hypre_ProjectBox(cbox, cindex, stride);
      hypre_StructMapFineToCoarse(hypre_BoxIMin(cbox), cindex, stride, hypre_BoxIMin(cbox));
      hypre_StructMapFineToCoarse(hypre_BoxIMax(cbox), cindex, stride, hypre_BoxIMax(cbox));

      /* update periodic */
      periodic[cdir] /= 2;

      /* build the coarse grid */
      /* RDF: RAP is already coarsening the grid; we shouldn't be doing this twice! */
      //hypre_StructCoarsen(grid_l[l], cindex, stride, 1, &grid_l[l+1]);
   }
   *num_levels = l + 1;

   *cdir_l_ptr        = cdir_l;
   *active_l_ptr      = active_l;
   *relax_weights_ptr = relax_weights;

   return hypre_error_flag;
}
