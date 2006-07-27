/*BHEADER**********************************************************************
 * Copyright (c) 2006   The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by the HYPRE team <hypre-users@llnl.gov>, UCRL-CODE-222953.
 * All rights reserved.
 *
 * This file is part of HYPRE (see http://www.llnl.gov/CASC/hypre/).
 * Please see the COPYRIGHT_and_LICENSE file for the copyright notice, 
 * disclaimer and the GNU Lesser General Public License.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (as published by the Free
 * Software Foundation) version 2.1 dated February 1999.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Revision$
 ***********************************************************************EHEADER*/



#include "../amge/headers.h"  

/*****************************************************************************
 *
 * builds smoother: Schwarz solver based on AE as subdomains;
 *
 ****************************************************************************/


int hypre_AMGeAGSchwarzSmootherSetup(int ***i_domain_dof_pointer,
				     int ***j_domain_dof_pointer,
				     double ***domain_matrixinverse_pointer,
				   
				     hypre_AMGeMatrixTopology **A,

				     hypre_CSRMatrix **Matrix,

				     int *level_pointer,


				     int *Num_elements, 

				     int *Num_dofs)

{
  int ierr = 0;

  int i,j,l;
  int level = level_pointer[0];
  int **i_domain_dof, **j_domain_dof;
  double **domain_matrixinverse;

  int *i_AE_node, *j_AE_node;

  int *i_AE_element, *j_AE_element;
  int *i_element_node, *j_element_node;
			     

  i_domain_dof = hypre_CTAlloc(int*, level);
  j_domain_dof = hypre_CTAlloc(int*, level);
  domain_matrixinverse = hypre_CTAlloc(double*, level);


  l=0;
factorization_step:
  printf("\n\nC O M P U T I N G  level[%d] SCHWARZ  S M O O T H E R\n",l);

  i_element_node = hypre_AMGeMatrixTopologyIElementNode(A[l]);
  j_element_node = hypre_AMGeMatrixTopologyJElementNode(A[l]);



  /*
  i_AE_element = hypre_AMGeMatrixTopologyIAEElement(A[l+1]);
  j_AE_element = hypre_AMGeMatrixTopologyJAEElement(A[l+1]);



  ierr = matrix_matrix_product(&i_AE_node, &j_AE_node,

			       i_AE_element, j_AE_element,
			       i_element_node, j_element_node,

			       Num_elements[l+1], 
			       Num_elements[l],
			       Num_dofs[l]);


  hypre_TFree(i_AE_element);
  hypre_TFree(j_AE_element);

  hypre_AMGeMatrixTopologyIAEElement(A[l+1]) = NULL;
  hypre_AMGeMatrixTopologyJAEElement(A[l+1]) = NULL;



  i_domain_dof[l] = i_AE_node;
  j_domain_dof[l] = j_AE_node;

  */


  i_domain_dof[l] = i_element_node;
  j_domain_dof[l] = j_element_node;
 

  /*
  ierr = hypre_ComputeSchwarzSmoother(i_domain_dof[l],
				      j_domain_dof[l],
				      Num_elements[l+1],

				      Matrix[l],

				      &domain_matrixinverse[l]);
				      */

  ierr = hypre_ComputeSchwarzSmoother(i_domain_dof[l],
				      j_domain_dof[l],
				      Num_elements[l],

				      Matrix[l],

				      &domain_matrixinverse[l]);
  l++;
  
  if (l < level && Num_dofs[l] > 0) goto factorization_step;
 
  level = l;


  *i_domain_dof_pointer = i_domain_dof;
  *j_domain_dof_pointer = j_domain_dof;
  *domain_matrixinverse_pointer = domain_matrixinverse;

  *level_pointer = level;

  return ierr;

}


 
