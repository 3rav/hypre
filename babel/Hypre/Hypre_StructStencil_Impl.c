/*
 * File:          Hypre_StructStencil_Impl.c
 * Symbol:        Hypre.StructStencil-v0.1.6
 * Symbol Type:   class
 * Babel Version: 0.8.0
 * SIDL Created:  20030210 16:05:28 PST
 * Generated:     20030210 16:05:38 PST
 * Description:   Server-side implementation for Hypre.StructStencil
 * 
 * WARNING: Automatically generated; only changes within splicers preserved
 * 
 * babel-version = 0.8.0
 * source-line   = 398
 * source-url    = file:/home/painter/linear_solvers/babel/Interfaces.idl
 */

/*
 * DEVELOPERS ARE EXPECTED TO PROVIDE IMPLEMENTATIONS
 * FOR THE FOLLOWING METHODS BETWEEN SPLICER PAIRS.
 */

/*
 * Symbol "Hypre.StructStencil" (version 0.1.6)
 * 
 * Define a structured stencil for a structured problem description.
 * More than one implementation is not envisioned, thus the decision has
 * been made to make this a class rather than an interface.
 */

#include "Hypre_StructStencil_Impl.h"

/* DO-NOT-DELETE splicer.begin(Hypre.StructStencil._includes) */
/* Put additional includes or other arbitrary code here... */
/* DO-NOT-DELETE splicer.end(Hypre.StructStencil._includes) */

/*
 * Class constructor called when the class is created.
 */

#undef __FUNC__
#define __FUNC__ "impl_Hypre_StructStencil__ctor"

void
impl_Hypre_StructStencil__ctor(
  Hypre_StructStencil self)
{
  /* DO-NOT-DELETE splicer.begin(Hypre.StructStencil._ctor) */
  /* Insert the implementation of the constructor method here... */
  /* DO-NOT-DELETE splicer.end(Hypre.StructStencil._ctor) */
}

/*
 * Class destructor called when the class is deleted.
 */

#undef __FUNC__
#define __FUNC__ "impl_Hypre_StructStencil__dtor"

void
impl_Hypre_StructStencil__dtor(
  Hypre_StructStencil self)
{
  /* DO-NOT-DELETE splicer.begin(Hypre.StructStencil._dtor) */
  /* Insert the implementation of the destructor method here... */
  /* DO-NOT-DELETE splicer.end(Hypre.StructStencil._dtor) */
}

/*
 * Method:  SetDimension[]
 */

#undef __FUNC__
#define __FUNC__ "impl_Hypre_StructStencil_SetDimension"

int32_t
impl_Hypre_StructStencil_SetDimension(
  Hypre_StructStencil self, int32_t dim)
{
  /* DO-NOT-DELETE splicer.begin(Hypre.StructStencil.SetDimension) */
  /* Insert the implementation of the SetDimension method here... */
   return 1;
  /* DO-NOT-DELETE splicer.end(Hypre.StructStencil.SetDimension) */
}

/*
 * Method:  SetSize[]
 */

#undef __FUNC__
#define __FUNC__ "impl_Hypre_StructStencil_SetSize"

int32_t
impl_Hypre_StructStencil_SetSize(
  Hypre_StructStencil self, int32_t size)
{
  /* DO-NOT-DELETE splicer.begin(Hypre.StructStencil.SetSize) */
  /* Insert the implementation of the SetSize method here... */
   return 1;
  /* DO-NOT-DELETE splicer.end(Hypre.StructStencil.SetSize) */
}

/*
 * Method:  SetElement[]
 */

#undef __FUNC__
#define __FUNC__ "impl_Hypre_StructStencil_SetElement"

int32_t
impl_Hypre_StructStencil_SetElement(
  Hypre_StructStencil self, int32_t index, struct SIDL_int__array* offset)
{
  /* DO-NOT-DELETE splicer.begin(Hypre.StructStencil.SetElement) */
  /* Insert the implementation of the SetElement method here... */
   return 1;
  /* DO-NOT-DELETE splicer.end(Hypre.StructStencil.SetElement) */
}
