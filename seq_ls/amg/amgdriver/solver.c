/*BHEADER**********************************************************************
 * Copyright (c) 2006   The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by the HYPRE team. UCRL-CODE-222953.
 * All rights reserved.
 *
 * This file is part of HYPRE (see http://www.llnl.gov/CASC/hypre/).
 * Please see the COPYRIGHT_and_LICENSE file for the copyright notice, 
 * disclaimer, contact information and the GNU Lesser General Public License.
 *
 * HYPRE is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License (as published by the Free Software
 * Foundation) version 2.1 dated February 1999.
 *
 * HYPRE is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the terms and conditions of the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Revision$
 ***********************************************************************EHEADER*/




/******************************************************************************
 *
 * Constructors and destructors for solver structure.
 *
 *****************************************************************************/

#include "headers.h"


/*--------------------------------------------------------------------------
 * NewSolver
 *--------------------------------------------------------------------------*/

Solver  *NewSolver(file_name)
char     *file_name;
{
   Solver  *solver;

   /* pcg params */
   int      pcg_max_iter;
   int      pcg_two_norm;

   /* gmres params */
   int      gmres_max_krylov;
   int      gmres_max_restarts;

   /* wjacobi params */
   double   wjacobi_weight;
   int      wjacobi_max_iter;

   /* amg setup params */
   int      amg_levmax;
   int      amg_ncg;
   double   amg_ecg;
   int      amg_nwt;
   double   amg_ewt;
   int      amg_nstr;

   /* amg solve params */
   int      amg_ncyc;
   int     *amg_mu;
   int     *amg_ntrlx;
   int     *amg_iprlx;


   /* amg output params */
   int      amg_ioutdat;

   FILE    *fp;
   int      i;


   /*----------------------------------------------------------
    * Allocate the solver structure
    *----------------------------------------------------------*/

   solver = hypre_CTAlloc(Solver, 1);

   /*----------------------------------------------------------
    * Open the solver file
    *----------------------------------------------------------*/

   fp = fopen(file_name, "r");

   /*----------------------------------------------------------
    * Generic solver input
    *----------------------------------------------------------*/

   fscanf(fp, "%d", &SolverType(solver));
   fscanf(fp, "%le", &SolverStopTolerance(solver));

   /*----------------------------------------------------------
    * PCG input
    *----------------------------------------------------------*/

   fscanf(fp, "%d", &pcg_max_iter);
   fscanf(fp, "%d", &pcg_two_norm);

   SolverPCGMaxIter(solver) = pcg_max_iter;
   SolverPCGTwoNorm(solver) = pcg_two_norm;

   /*----------------------------------------------------------
    * GMRES input
    *----------------------------------------------------------*/

   fscanf(fp, "%d", &gmres_max_krylov);
   fscanf(fp, "%d", &gmres_max_restarts);

   SolverGMRESMaxKrylov(solver)   = gmres_max_krylov;
   SolverGMRESMaxRestarts(solver) = gmres_max_restarts;

   /*----------------------------------------------------------
    * Weighted Jacobi input
    *----------------------------------------------------------*/

   fscanf(fp, "%le", &wjacobi_weight);
   fscanf(fp, "%d",  &wjacobi_max_iter);

   SolverWJacobiWeight(solver)  = wjacobi_weight;
   SolverWJacobiMaxIter(solver) = wjacobi_max_iter;

   /*----------------------------------------------------------
    * AMG input
    *----------------------------------------------------------*/

   fscanf(fp, "%d",  &amg_levmax);
   fscanf(fp, "%d",  &amg_ncg);
   fscanf(fp, "%le", &amg_ecg);
   fscanf(fp, "%d",  &amg_nwt);
   fscanf(fp, "%le", &amg_ewt);
   fscanf(fp, "%d",  &amg_nstr);
   
   fscanf(fp, "%d", &amg_ncyc);
   amg_mu = hypre_CTAlloc(int, amg_levmax);
   for (i = 0; i < amg_levmax; i++)
      fscanf(fp, "%d", &amg_mu[i]);
   amg_ntrlx = hypre_CTAlloc(int, 4);
   for (i = 0; i < 4; i++)
      fscanf(fp, "%d", &amg_ntrlx[i]);
   amg_iprlx = hypre_CTAlloc(int, 4);
   for (i = 0; i < 4; i++)
      fscanf(fp, "%d", &amg_iprlx[i]);
   fscanf(fp, "%d", &amg_ioutdat);

   /*----------------------------------------------------------
    * Set the solver structure
    *----------------------------------------------------------*/

   SolverAMGLevMax(solver)  = amg_levmax;
   SolverAMGNCG(solver)     = amg_ncg;
   SolverAMGECG(solver)     = amg_ecg;
   SolverAMGNWT(solver)     = amg_nwt;
   SolverAMGEWT(solver)     = amg_ewt;
   SolverAMGNSTR(solver)    = amg_nstr;

   SolverAMGNCyc(solver)    = amg_ncyc;
   SolverAMGMU(solver)      = amg_mu;
   SolverAMGNTRLX(solver)   = amg_ntrlx;
   SolverAMGIPRLX(solver)   = amg_iprlx;

   SolverAMGIOutDat(solver) = amg_ioutdat;

   /*----------------------------------------------------------
    * Close the solver file and return
    *----------------------------------------------------------*/

   fclose(fp);

   return solver;
}

/*--------------------------------------------------------------------------
 * FreeSolver
 *--------------------------------------------------------------------------*/

void     FreeSolver(solver)
Solver  *solver;
{
   if (solver)
   {
      hypre_TFree(SolverAMGMU(solver));
      hypre_TFree(SolverAMGNTRLX(solver));
      hypre_TFree(SolverAMGIPRLX(solver));

      hypre_TFree(solver);
   }
}

/*--------------------------------------------------------------------------
 * WriteSolver
 *--------------------------------------------------------------------------*/

void     WriteSolver(file_name, solver)
char    *file_name;
Solver  *solver;

{
   FILE    *fp;

   int      type;

   double   stop_tolerance;

   /* pcg params */
   int      pcg_max_iter;
   int      pcg_two_norm;

   /* gmres params */
   int      gmres_max_krylov;
   int      gmres_max_restarts;

   /* wjacobi params */
   double   wjacobi_weight;
   int      wjacobi_max_iter;

   /* amg setup params */
   int      amg_levmax;
   int      amg_ncg;
   double   amg_ecg;
   int      amg_nwt;
   double   amg_ewt;
   int      amg_nstr;

   /* amg solve params */
   int      amg_ncyc;
   int     *amg_mu;
   int     *amg_ntrlx;
   int     *amg_iprlx;

   /* amg output params */
   int      amg_ioutdat;

   int      j;


   /*----------------------------------------------------------
    * Get the solver data
    *----------------------------------------------------------*/

   type = SolverType(solver);
   stop_tolerance = SolverStopTolerance(solver);

   pcg_max_iter = SolverPCGMaxIter(solver);
   pcg_two_norm = SolverPCGTwoNorm(solver);

   gmres_max_krylov   = SolverGMRESMaxKrylov(solver);
   gmres_max_restarts = SolverGMRESMaxRestarts(solver);

   wjacobi_weight   = SolverWJacobiWeight(solver);
   wjacobi_max_iter = SolverWJacobiMaxIter(solver);

   amg_levmax  = SolverAMGLevMax(solver);
   amg_ncg     = SolverAMGNCG(solver);
   amg_ecg     = SolverAMGECG(solver);
   amg_nwt     = SolverAMGNWT(solver);
   amg_ewt     = SolverAMGEWT(solver);
   amg_nstr    = SolverAMGNSTR(solver);

   amg_ncyc    = SolverAMGNCyc(solver);
   amg_mu      = SolverAMGMU(solver);
   amg_ntrlx   = SolverAMGNTRLX(solver);
   amg_iprlx   = SolverAMGIPRLX(solver);

   amg_ioutdat = SolverAMGIOutDat(solver);

   /*----------------------------------------------------------
    * Open the output file
    *----------------------------------------------------------*/

   fp = fopen(file_name, "a");

   /*----------------------------------------------------------
    * Solver type
    *----------------------------------------------------------*/

   fprintf(fp,"\nSOLVER PARAMETERS:\n\n");
   fprintf(fp, "  Solver Type:  %d - ", type);

   if (type == SOLVER_AMG)
   {
      fprintf(fp, "AMG \n\n");
   }
   else if (type == SOLVER_Jacobi)
   {
      fprintf(fp, "Jacobi \n\n");
   }
   else if (type == SOLVER_AMG_PCG)
   {
      fprintf(fp, "AMG PCG \n\n");
   }
   else if (type == SOLVER_Jacobi_PCG)
   {
      fprintf(fp, "Jacobi PCG \n\n");
   }
   else if (type == SOLVER_AMG_GMRES)
   {
      fprintf(fp, "AMG GMRES \n\n");
   }
   else if (type == SOLVER_Jacobi_GMRES)
   {
      fprintf(fp, "Jacobi GMRES \n\n");
   }

   /*----------------------------------------------------------
    * PCG info
    *----------------------------------------------------------*/

   if (type == SOLVER_AMG_PCG || type == SOLVER_Jacobi_PCG)
   {
       fprintf(fp, "  Preconditioned Conjugate Gradient Parameters:\n");
       fprintf(fp, "    Solver Stop Tolerance:  %e \n", stop_tolerance);
       fprintf(fp, "    Maximum Iterations: %d \n", pcg_max_iter);
       fprintf(fp, "    Two Norm Flag: %d \n\n", pcg_two_norm);
   }

   /*----------------------------------------------------------
    * GMRES info
    *----------------------------------------------------------*/

   if (type == SOLVER_AMG_GMRES || type == SOLVER_Jacobi_GMRES)
   {
       fprintf(fp, "  Generalized Minimum Residual Parameters:\n");
       fprintf(fp, "    Solver Stop Tolerance:  %e \n", stop_tolerance);
       fprintf(fp, "    Maximum Krylov Dimension: %d \n", gmres_max_krylov);
       fprintf(fp, "    Max Number of Restarts: %d \n\n", gmres_max_restarts);
   }

   /*----------------------------------------------------------
    * Jacobi info
    *----------------------------------------------------------*/

   if (type == SOLVER_Jacobi ||
       type == SOLVER_Jacobi_PCG ||
       type == SOLVER_Jacobi_GMRES)
   {
      fprintf(fp, "  Jacobi Parameters:\n");
      fprintf(fp, "    Weight for Relaxation: %f \n", wjacobi_weight);
      fprintf(fp, "    Maximum Iterations: %d \n\n", wjacobi_max_iter);
   }

   /*----------------------------------------------------------
    * AMG info
    *----------------------------------------------------------*/

   if (type == SOLVER_AMG ||
       type == SOLVER_AMG_PCG ||
       type == SOLVER_AMG_GMRES)
   {
      fprintf(fp, "  AMG Parameters:\n");
      fprintf(fp, "    Maximum number of levels:            %d \n",
	      amg_levmax);
      fprintf(fp, "    Coarsening controls (ncg, ecg):      %d   %f \n",
	      amg_ncg, amg_ecg);
      fprintf(fp, "    Interpolation controls (nwt, ewt):   %d   %f \n",
	      amg_nwt, amg_ewt);
      fprintf(fp, "    Strong connection definition (nstr): %d \n", amg_nstr);
      fprintf(fp, "    Number and type of cycles (ncyc):    %d \n", amg_ncyc);
      fprintf(fp, "    Stopping Tolerance                   %e \n",
                   stop_tolerance); 
      fprintf(fp, "    W-cycling parameter (mu): ");
      for (j = 0; j < amg_levmax; j++)
	 fprintf(fp, "%d ", amg_mu[j]);
      fprintf(fp, "\n");
      fprintf(fp, "    Relaxation Parameters:\n");
      fprintf(fp, "       ntr(f,d,u,c): %d  %d  %d  %d \n",
	      amg_ntrlx[0], amg_ntrlx[1], amg_ntrlx[2], amg_ntrlx[3]);
      fprintf(fp, "       ipr(f,d,u,c): %d  %d  %d  %d \n",
	      amg_iprlx[0], amg_iprlx[1], amg_iprlx[2], amg_iprlx[3]);
 
      fprintf(fp, "    Output flag (ioutdat): %d \n", amg_ioutdat);

   }

   /*----------------------------------------------------------
    * Close the output file
    *----------------------------------------------------------*/

   fclose(fp);

   return;
}

