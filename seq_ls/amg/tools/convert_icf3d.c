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




#include <stdio.h>

#include "general.h"
#include "matrix.h"
#include "io.h"


/*--------------------------------------------------------------------------
 * Convert icf3d data to AMG data
 *--------------------------------------------------------------------------*/

int   main(argc, argv)
int   argc;
char *argv[];
{
   char    *run_name;

   char     file_name[255];
   FILE    *fp;

   long int ft;

   double  *S_data;
   int     *S_ia;
   int     *S_ja;
   double  *A_data;
   int     *A_ia;
   int     *A_ja;
   hypre_Matrix  *A;

   double  *x;
   double  *y;
   double  *z;

   int     *iarray;
   
   int      n, m, i, j, ii, jj;



   /*-------------------------------------------------------
    * Check that the number of command args is correct
    *-------------------------------------------------------*/

   if (argc < 2)
   {
      fprintf(stderr, "Usage:  convert_icf3d <run name>\n");
      exit(1);
   }

   run_name = argv[1];

   /*-------------------------------------------------------
    * Read in the icf3d data
    *-------------------------------------------------------*/

   sprintf(file_name, "%s.raw.mat", run_name);

   fp = fopen(file_name, "r");

   fscanf(fp, "%d", &n);

   /* save current file position */
   ft = ftell(fp);

   /* compute symmetric S_ia */
   S_ia = hypre_TAlloc(int, (n+1));
   S_ia[0] = 0;
   m = 1;
   for (i = 0; i < n;)
   {
      fscanf(fp, "%d", &j);
      if (j == i)
      {
	 S_ia[i+1] = m;
	 i++;
      }
      m++;
   }

   /* seek to the previously saved file position */
   fseek(fp, ft, 0);

   /* read in symmetric S_ja (put diagonal at beginning of row) */
   S_ja = hypre_TAlloc(int, S_ia[n]);
   for (i = 0; i < n; i++)
   {
      for (j = (S_ia[i]+1); j < S_ia[i+1]; j++)
	 fscanf(fp, "%d", &S_ja[j]);
      fscanf(fp, "%d", &S_ja[S_ia[i]]);
   }

   /* read in symmetric S_data (put diagonal at beginning of row) */
   S_data = hypre_TAlloc(double, S_ia[n]);
   for (i = 0; i < n; i++)
   {
      for (j = (S_ia[i]+1); j < S_ia[i+1]; j++)
	 fscanf(fp, "%le", &S_data[j]);
      fscanf(fp, "%le", &S_data[S_ia[i]]);
   }

   /* read in x, y, z coordinates */
   x = hypre_TAlloc(double, n);
   y = hypre_TAlloc(double, n);
   z = hypre_TAlloc(double, n);
   for (i = 0; i < n; i++)
      fscanf(fp, "%*d%le%le%le", &x[i], &y[i], &z[i]);

   fclose(fp);

   /*-------------------------------------------------------
    * Write out the x, y, z data
    *-------------------------------------------------------*/

   sprintf(file_name, "%s.in.xyz", run_name);

   fp = fopen(file_name, "w");

   for (i = 0; i < n; i++)
      fprintf(fp, "%le\n", x[i]);
   for (i = 0; i < n; i++)
      fprintf(fp, "%le\n", y[i]);
   for (i = 0; i < n; i++)
      fprintf(fp, "%le\n", z[i]);

   fclose(fp);

   hypre_TFree(x);
   hypre_TFree(y);
   hypre_TFree(z);

   /*-------------------------------------------------------
    * Set up the matrix structure
    *-------------------------------------------------------*/

   /* compute number of additional row elements in A */
   iarray = hypre_TAlloc(int, n);
   for (i = 0; i < n; i++)
      iarray[i] = 0;
   for (i = 0; i < n; i++)
   {
      for (j = (S_ia[i]+1); j < S_ia[i+1]; j++)
	 iarray[S_ja[j]]++;
   }

   /* compute A_ia */
   A_ia = hypre_TAlloc(int, (n+1));
   m = 0;
   for (i = 0; i < n; i++)
   {
      A_ia[i] = S_ia[i] + m;
      m += iarray[i];
   }
   A_ia[n] = S_ia[n] + m;

   /* copy S_ja and S_data into A_ja and A_data */
   A_ja = hypre_TAlloc(int, A_ia[n]);
   A_data = hypre_TAlloc(double, A_ia[n]);
   for (i = 0; i < n; i++)
   {
      m = S_ia[i+1] - S_ia[i];
      for (j = 0; j < m; j++)
      {
	 ii = S_ia[i] + j;
	 jj = A_ia[i] + j;
	 A_ja[jj] = S_ja[ii];
	 A_data[jj] = S_data[ii];
      }
   }

   /* fill in the upper triangle */
   for (i = 0; i < n; i++)
   {
      for (j = (S_ia[i]+1); j < S_ia[i+1]; j++)
      {
	 ii = S_ja[j];
	 jj = A_ia[ii+1] - iarray[ii];
	 A_ja[jj] = i;
	 A_data[jj] = S_data[j];

	 iarray[ii]--;
      }
   }

   /* convert to fortran indexing */
   for (j = 0; j < A_ia[n]; j++)
      A_ja[j]++;
   for (i = 0; i <= n; i++)
      A_ia[i]++;

   /* create the matrix */
   A = hypre_NewMatrix(A_data, A_ia, A_ja, n);

   /*-------------------------------------------------------
    * Write out the matrix data
    *-------------------------------------------------------*/

   sprintf(file_name, "%s.in.ysmp", run_name);
   hypre_WriteYSMP(file_name, A);

   return 0;
}

