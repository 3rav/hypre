/*
 * File:          Hypre_StructToIJVector_Skel.c
 * Symbol:        Hypre.StructToIJVector-v0.1.5
 * Symbol Type:   class
 * Babel Version: 0.7.4
 * SIDL Created:  20021217 16:38:33 PST
 * Generated:     20021217 16:38:42 PST
 * Description:   Server-side glue code for Hypre.StructToIJVector
 * 
 * WARNING: Automatically generated; changes will be lost
 * 
 * babel-version = 0.7.4
 * source-line   = 450
 * source-url    = file:/home/painter/linear_solvers/babel/Interfaces.idl
 */

#include "Hypre_StructToIJVector_IOR.h"
#include "Hypre_StructToIJVector.h"
#include <stddef.h>

extern void
impl_Hypre_StructToIJVector__ctor(
  Hypre_StructToIJVector);

extern void
impl_Hypre_StructToIJVector__dtor(
  Hypre_StructToIJVector);

extern int32_t
impl_Hypre_StructToIJVector_SetIJVector(
  Hypre_StructToIJVector,
  Hypre_IJBuildVector);

extern int32_t
impl_Hypre_StructToIJVector_SetCommunicator(
  Hypre_StructToIJVector,
  void*);

extern int32_t
impl_Hypre_StructToIJVector_Initialize(
  Hypre_StructToIJVector);

extern int32_t
impl_Hypre_StructToIJVector_Assemble(
  Hypre_StructToIJVector);

extern int32_t
impl_Hypre_StructToIJVector_GetObject(
  Hypre_StructToIJVector,
  SIDL_BaseInterface*);

extern int32_t
impl_Hypre_StructToIJVector_SetGrid(
  Hypre_StructToIJVector,
  Hypre_StructGrid);

extern int32_t
impl_Hypre_StructToIJVector_SetStencil(
  Hypre_StructToIJVector,
  Hypre_StructStencil);

extern int32_t
impl_Hypre_StructToIJVector_SetValue(
  Hypre_StructToIJVector,
  struct SIDL_int__array*,
  double);

extern int32_t
impl_Hypre_StructToIJVector_SetBoxValues(
  Hypre_StructToIJVector,
  struct SIDL_int__array*,
  struct SIDL_int__array*,
  struct SIDL_double__array*);

void
Hypre_StructToIJVector__set_epv(struct Hypre_StructToIJVector__epv *epv)
{
  epv->f__ctor = impl_Hypre_StructToIJVector__ctor;
  epv->f__dtor = impl_Hypre_StructToIJVector__dtor;
  epv->f_SetIJVector = impl_Hypre_StructToIJVector_SetIJVector;
  epv->f_SetCommunicator = impl_Hypre_StructToIJVector_SetCommunicator;
  epv->f_Initialize = impl_Hypre_StructToIJVector_Initialize;
  epv->f_Assemble = impl_Hypre_StructToIJVector_Assemble;
  epv->f_GetObject = impl_Hypre_StructToIJVector_GetObject;
  epv->f_SetGrid = impl_Hypre_StructToIJVector_SetGrid;
  epv->f_SetStencil = impl_Hypre_StructToIJVector_SetStencil;
  epv->f_SetValue = impl_Hypre_StructToIJVector_SetValue;
  epv->f_SetBoxValues = impl_Hypre_StructToIJVector_SetBoxValues;
}

struct Hypre_StructToIJVector__data*
Hypre_StructToIJVector__get_data(Hypre_StructToIJVector self)
{
  return (struct Hypre_StructToIJVector__data*)(self ? self->d_data : NULL);
}

void Hypre_StructToIJVector__set_data(
  Hypre_StructToIJVector self,
  struct Hypre_StructToIJVector__data* data)
{
  if (self) {
    self->d_data = data;
  }
}
