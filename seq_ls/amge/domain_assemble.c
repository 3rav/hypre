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






#include "headers.h" 

int hypre_AMGeDomainElementSparseAssemble(int *i_domain_element,
					  int *j_domain_element,
					  int num_domains,

					  int *i_element_chord,
					  int *j_element_chord,
					  double *a_element_chord,

					  int *i_chord_dof, int *j_chord_dof,

					  int **i_domain_chord_pointer, 
					  int **j_domain_chord_pointer,
					  double **a_domain_chord_pointer,

					  int num_elements, int num_chords,
					  int num_dofs)

{
  int ierr = 0;
  int i,j,k,l;
  int *i_domain_chord, *j_domain_chord;
  double *a_domain_chord;

  int *i_chord_domain_index;

  i_chord_domain_index =  hypre_CTAlloc(int, num_chords);
  
  ierr = matrix_matrix_product(&i_domain_chord,
			       &j_domain_chord,

			       i_domain_element, j_domain_element,
			       i_element_chord, j_element_chord,
			       num_domains, num_elements, num_chords);

  /* numeric multiplication: --------------------------------------------*/

  a_domain_chord = hypre_CTAlloc(double, i_domain_chord[num_domains]);

  for (i=0; i < num_chords; i++)
   i_chord_domain_index[i] = -1; 

  for (i=0; i < i_domain_chord[num_domains]; i++)
    a_domain_chord[i] = 0.e0;

  for (i=0; i < num_domains; i++)
    {
      for (j=i_domain_chord[i]; j < i_domain_chord[i+1]; j++)
	i_chord_domain_index[j_domain_chord[j]] = j;

    for (j=i_domain_element[i]; j < i_domain_element[i+1]; j++)
      for (k=i_element_chord[j_domain_element[j]];
	   k<i_element_chord[j_domain_element[j]+1]; k++)
	{
	  a_domain_chord[i_chord_domain_index[j_element_chord[k]]]
	    += a_element_chord[k];

	  /*
	  for (l=i_domain_chord[i]; l < i_domain_chord[i+1]; l++)
	    if (j_domain_chord[l] == j_element_chord[k])
	      {
		a_domain_chord[l] += a_element_chord[k];
		break;
	      }
	      */
	}
      for (j=i_domain_chord[i]; j < i_domain_chord[i+1]; j++)
	i_chord_domain_index[j_domain_chord[j]] = -1;
    }
  
  hypre_TFree(i_chord_domain_index);

  *i_domain_chord_pointer = i_domain_chord;
  *j_domain_chord_pointer = j_domain_chord;
  *a_domain_chord_pointer = a_domain_chord;

  /*

  printf("assembled domain sparse matrices: \n");
  for (i=0; i < num_domains; i++)
    {
      if (i_domain_chord[i+1] > i_domain_chord[i])
	{

	  printf("domain %d: num_nonzero_entries: %d \n", i,
		 i_domain_chord[i+1] - i_domain_chord[i]);

	  for (l=i_domain_chord[i]; l < i_domain_chord[i+1]; l++)
	    {
	      k = j_domain_chord[l];
	      if (j_chord_dof[i_chord_dof[k]] == j_chord_dof[i_chord_dof[k]+1])
		printf("(%d,%d): %e\n", j_chord_dof[i_chord_dof[k]],
		       j_chord_dof[i_chord_dof[k]+1], a_domain_chord[l]);
	    }
	  printf("==================================================\n\n");
	}
    }
  

    */


  return ierr;
}
 
