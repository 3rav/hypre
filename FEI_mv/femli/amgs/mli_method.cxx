/*BHEADER**********************************************************************
 * (c) 2001   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 *********************************************************************EHEADER*/

/******************************************************************************
 *
 * functions for creating MLI_Method 
 *
 *****************************************************************************/

/*--------------------------------------------------------------------------
 * include files 
 *--------------------------------------------------------------------------*/

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include <string.h>
#include "amgs/mli_method.h"
#include "amgs/mli_method_amgsa.h"
#include "amgs/mli_method_amgrs.h"

/*****************************************************************************
 * create a method from method name
 *--------------------------------------------------------------------------*/

MLI_Method *MLI_Method_CreateFromName( char *str, MPI_Comm mpi_comm )
{
   MLI_Method *method_ptr;
   char       paramString[80];

   if ( !strcasecmp(str, "AMGSA" ) )
   {
      method_ptr  = new MLI_Method_AMGSA(mpi_comm);
   }
   else if ( !strcasecmp(str, "AMGSAe" ) )
   {
      method_ptr  = new MLI_Method_AMGSA(mpi_comm);
      strcpy( paramString, "useSAMGe" );
      method_ptr->setParams( paramString, 0, NULL );
   }
   else if ( !strcasecmp(str, "AMGSADD" ) )
   {
      method_ptr  = new MLI_Method_AMGSA(mpi_comm);
      strcpy( paramString, "useSAMGDD" );
      method_ptr->setParams( paramString, 0, NULL );
      strcpy( paramString, "setNumLevels 2" );
      method_ptr->setParams( paramString, 0, NULL );
   }
   else if ( !strcasecmp(str, "AMGSADDe" ) )
   {
      method_ptr  = new MLI_Method_AMGSA(mpi_comm);
      strcpy( paramString, "useSAMGe" );
      method_ptr->setParams( paramString, 0, NULL );
      strcpy( paramString, "useSAMGDD" );
      method_ptr->setParams( paramString, 0, NULL );
      strcpy( paramString, "setNumLevels 2" );
      method_ptr->setParams( paramString, 0, NULL );
   }
   else if ( !strcasecmp(str, "AMGRS" ) )
   {
      method_ptr  = new MLI_Method_AMGRS(mpi_comm);
   }
   else
   {
      printf("MLI_Method_Create ERROR : method %s not defined.\n", str);
      printf("    valid ones are : \n\n");
      printf("    (1) AMGSA (%d)\n", MLI_METHOD_AMGSA_ID); 
      printf("    (2) AMGSAe (%d)\n", MLI_METHOD_AMGSAE_ID); 
      printf("    (3) AMGSADD (%d)\n", MLI_METHOD_AMGSADD_ID); 
      printf("    (4) AMGSADDe (%d)\n", MLI_METHOD_AMGSADDE_ID); 
      printf("    (5) AMGRS (%d)\n", MLI_METHOD_AMGRS_ID); 
      exit(1);
   }
   return method_ptr;
}

/*****************************************************************************
 * create a method from method ID
 *--------------------------------------------------------------------------*/

MLI_Method *MLI_Method_CreateFromID( int method_id, MPI_Comm mpi_comm )
{
   MLI_Method *method_ptr;
   char       paramString[80];

   switch ( method_id )
   {
      case MLI_METHOD_AMGSA_ID :
           method_ptr  = new MLI_Method_AMGSA(mpi_comm);
           break;
      case MLI_METHOD_AMGSAE_ID :
           method_ptr  = new MLI_Method_AMGSA(mpi_comm);
           strcpy( paramString, "useSAMGe" );
           method_ptr->setParams(paramString, 0, NULL);
           break;
      case MLI_METHOD_AMGSADD_ID :
           method_ptr  = new MLI_Method_AMGSA(mpi_comm);
           strcpy( paramString, "useSAMGDD" );
           method_ptr->setParams(paramString, 0, NULL);
           strcpy( paramString, "setNumLevels 2" );
           method_ptr->setParams(paramString, 0, NULL);
           break;
      case MLI_METHOD_AMGSADDE_ID :
           method_ptr  = new MLI_Method_AMGSA(mpi_comm);
           strcpy( paramString, "useSAMGe" );
           method_ptr->setParams(paramString, 0, NULL);
           strcpy( paramString, "useSAMGDD" );
           method_ptr->setParams(paramString, 0, NULL);
           strcpy( paramString, "setNumLevels 2" );
           method_ptr->setParams(paramString, 0, NULL);
           break;
      case MLI_METHOD_AMGRS_ID :
           method_ptr  = new MLI_Method_AMGRS(mpi_comm);
           break;
      default :
           printf("MLI_Method_Create ERROR : method %d not defined\n",
                  method_id);
           printf("    valid ones are : \n\n");
           printf("    (1) AMGSA (%d)\n", MLI_METHOD_AMGSA_ID); 
           printf("    (2) AMGSAe (%d)\n", MLI_METHOD_AMGSAE_ID); 
           printf("    (3) AMGSADD (%d)\n", MLI_METHOD_AMGSADD_ID); 
           printf("    (4) AMGSADDe (%d)\n", MLI_METHOD_AMGSADDE_ID); 
           printf("    (5) AMGRS (%d)\n", MLI_METHOD_AMGRS_ID); 
           exit(1);
   }
   return method_ptr;
}

