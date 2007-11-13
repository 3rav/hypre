/*BHEADER**********************************************************************
 * Copyright (c) 2007, Lawrence Livermore National Security, LLC.
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





/*****************************************************************************
 * stores element matrices in format:
 *  i_element_chord,
 *  j_element_chord,
 *  a_element_chord; 
 *
 * here chord = (i_dof, j_dof) directed pair of indices
 *  for which A(i_dof, j_dof) \ne 0; A is the global assembled matrix
 * 
 * also returns i_chord_dof, j_chord_dof, num_chords;
 ****************************************************************************/


#include "headers.h" 

int hypre_AMGeElementMatrixDof(int *i_element_dof, int *j_element_dof,


			       double *element_data,

			       int **i_element_chord_pointer,
			       int **j_element_chord_pointer,
			       double **a_element_chord_pointer,

			       int **i_chord_dof_pointer, 
			       int **j_chord_dof_pointer,

			       int *num_chords_pointer,

			       int num_elements, int num_dofs)

{
  int ierr = 0;

  int i,j,k,l;
  int k_dof, l_dof;


  int *i_dof_element, *j_dof_element;

  int *i_dof_chord, *j_dof_chord;

  int *i_element_chord, *j_element_chord;
  double *a_element_chord;

  int *i_chord_dof, *j_chord_dof;

  int *i_dof_dof, *j_dof_dof;

  int chord_counter, chord_dof_counter,
    element_chord_counter;

  int end_chord, chord;


  printf("num_elements: %d, num_dofs: %d\n", num_elements, num_dofs);
  i_element_chord = hypre_CTAlloc(int, num_elements+1);

  ierr = transpose_matrix_create(&i_dof_element, &j_dof_element,
				 i_element_dof, j_element_dof,
				 num_elements, num_dofs);


  ierr = matrix_matrix_product(&i_dof_dof, &j_dof_dof,

			       i_dof_element, j_dof_element,
			       i_element_dof, j_element_dof,
	
			       num_dofs, num_elements, num_dofs);
  hypre_TFree(i_dof_element);
  hypre_TFree(j_dof_element);

  i_chord_dof = hypre_CTAlloc(int, i_dof_dof[num_dofs]+1);
  j_chord_dof = hypre_CTAlloc(int, 2*i_dof_dof[num_dofs]);

  chord_counter = 0;
  chord_dof_counter= 0;
  for (i=0; i < num_dofs; i++)
    for (j=i_dof_dof[i]; j < i_dof_dof[i+1]; j++)
      {
	i_chord_dof[chord_counter] = chord_dof_counter;
	chord_counter++;
	j_chord_dof[chord_dof_counter] = i;
	chord_dof_counter++;
	j_chord_dof[chord_dof_counter] = j_dof_dof[j];
	chord_dof_counter++;
      }

  hypre_TFree(i_dof_dof);
  hypre_TFree(j_dof_dof);

  i_chord_dof[chord_counter] = chord_dof_counter;

  *num_chords_pointer = chord_counter;

  i_element_chord[0] = 0;
  element_chord_counter = 0;
  for (i=0; i < num_elements; i++)
    {
      element_chord_counter+= (i_element_dof[i+1]-i_element_dof[i])
	* (i_element_dof[i+1]-i_element_dof[i]);

      i_element_chord[i+1] = element_chord_counter;
    }


  *i_element_chord_pointer = i_element_chord;

  j_element_chord = hypre_CTAlloc(int, element_chord_counter); 

  ierr = transpose_matrix_create(&i_dof_chord, &j_dof_chord,
				 i_chord_dof, j_chord_dof,
				 chord_counter, num_dofs);



  element_chord_counter = 0;
  for (i=0; i < num_elements; i++)
    {
      for (k=i_element_dof[i];  k < i_element_dof[i+1]; k++)
	{
	  k_dof = j_element_dof[k];

	  for (l=i_element_dof[i];  l < i_element_dof[i+1]; l++)
	    {
	      l_dof = j_element_dof[l];
	      end_chord = -1;
	      for (j=i_dof_chord[k_dof]; j < i_dof_chord[k_dof+1]; j++)
		if (l_dof == j_chord_dof[i_chord_dof[j_dof_chord[j]]+1] &&
		    k_dof == j_chord_dof[i_chord_dof[j_dof_chord[j]]])
		  {
		    end_chord = l_dof;
		    chord = j_dof_chord[j];
		    break;
		  }

	      if (end_chord == -1)
		{
		  printf("chord with no end: ************************\n");
		  return -1;
		}
	      
	      j_element_chord[element_chord_counter] = chord;

	      element_chord_counter++;
	    }
	}
    }

  /*
  hypre_TFree(i_element_dof);
  hypre_TFree(j_element_dof);
  */

  hypre_TFree(i_dof_chord);
  hypre_TFree(j_dof_chord);



  *i_chord_dof_pointer = i_chord_dof;
  *j_chord_dof_pointer = j_chord_dof;

  *j_element_chord_pointer = j_element_chord;
  *a_element_chord_pointer = element_data;

  /* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
  printf("element matrices in element_chord format: \n");
  for (i=0; i < num_elements; i++)
    {
      printf("element %d: num_entries^2: %d \n", i,
	     i_element_chord[i+1] - i_element_chord[i]);

      for (l=i_element_chord[i]; l < i_element_chord[i+1]; l++)
	{
	  k = j_element_chord[l];

	    if (j_chord_dof[i_chord_dof[k]] == j_chord_dof[i_chord_dof[k]+1])
	      printf("(%d,%d): %e\n", j_chord_dof[i_chord_dof[k]],
		   j_chord_dof[i_chord_dof[k]+1], element_data[l]);
	}
      printf("==================================================\n\n");
    }

    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */

    return ierr;

}

