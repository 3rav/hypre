/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision$
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 * HYPRE_StructMatrix interface
 *
 *****************************************************************************/

#include "headers.h"

/*--------------------------------------------------------------------------
 * HYPRE_NewStructInterfaceVector
 *--------------------------------------------------------------------------*/

HYPRE_StructInterfaceVector
HYPRE_NewStructInterfaceVector( MPI_Comm     context,
		      HYPRE_StructGrid     grid,
		      HYPRE_StructStencil  stencil )
{
   return ( (HYPRE_StructInterfaceVector)
	    hypre_NewStructInterfaceVector( context,
				  (hypre_StructGrid *) grid,
				  (hypre_StructStencil *) stencil ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_FreeStructInterfaceVector
 *--------------------------------------------------------------------------*/

int 
HYPRE_FreeStructInterfaceVector( HYPRE_StructInterfaceVector struct_vector )
{
   return( hypre_FreeStructInterfaceVector( (hypre_StructInterfaceVector *) struct_vector ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_SetStructInterfaceVectorCoeffs
 *--------------------------------------------------------------------------*/

int 
HYPRE_SetStructInterfaceVectorCoeffs( HYPRE_StructInterfaceVector  vector,
			    int               *grid_index,
			    double            *coeffs      )
{
   hypre_StructInterfaceVector *new_vector;
   hypre_Index         *new_grid_index;

   int                d;
   int                ierr;

   new_vector = (hypre_StructInterfaceVector *) vector;
   new_grid_index = hypre_NewIndex();
   for (d = 0;
	d < hypre_StructGridDim(hypre_StructInterfaceVectorStructGrid(new_vector));
	d++)
   {
      hypre_IndexD(new_grid_index, d) = grid_index[d];
   }

   ierr = hypre_SetStructInterfaceVectorCoeffs( new_vector, new_grid_index, coeffs );

   hypre_FreeIndex(new_grid_index);

   return (ierr);
}

/*--------------------------------------------------------------------------
 * HYPRE_SetStructInterfaceVector
 *--------------------------------------------------------------------------*/

int 
HYPRE_SetStructInterfaceVector( HYPRE_StructInterfaceVector  vector,
			    double      *val      )
{
   hypre_StructInterfaceVector *new_vector;

   int                ierr;

   new_vector = (hypre_StructInterfaceVector *) vector;

   ierr = hypre_SetStructInterfaceVector( new_vector, val );

   return (ierr);
}

/*--------------------------------------------------------------------------
 * HYPRE_AssembleStructInterfaceVector
 *--------------------------------------------------------------------------*/

int 
HYPRE_AssembleStructInterfaceVector( HYPRE_StructInterfaceVector vector )
{
   return( hypre_AssembleStructInterfaceVector( (hypre_StructInterfaceVector *) vector ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_SetStructInterfaceVectorStorageType
 *--------------------------------------------------------------------------*/

int
HYPRE_SetStructInterfaceVectorStorageType( HYPRE_StructInterfaceVector  struct_vector,
				 int                type           )
{
   return( hypre_SetStructInterfaceVectorStorageType(
      (hypre_StructInterfaceVector *) struct_vector, type ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_StructInterfaceVectorGetData
 *--------------------------------------------------------------------------*/

void *
HYPRE_StructInterfaceVectorGetData( HYPRE_StructInterfaceVector vector )
{
   return( hypre_StructInterfaceVectorData( (hypre_StructInterfaceVector *) vector ) );
}

/* OUTPUT */
/*--------------------------------------------------------------------------
 * HYPRE_PrintStructInterfaceVector
 *--------------------------------------------------------------------------*/

int 
HYPRE_PrintStructInterfaceVector( HYPRE_StructInterfaceVector vector )
{
   return( hypre_PrintStructInterfaceVector( (hypre_StructInterfaceVector *) vector ) );
}
