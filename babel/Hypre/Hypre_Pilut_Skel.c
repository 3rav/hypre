/*
 * File:          Hypre_Pilut_Skel.c
 * Symbol:        Hypre.Pilut-v0.1.6
 * Symbol Type:   class
 * Babel Version: 0.8.0
 * SIDL Created:  20030121 14:39:01 PST
 * Generated:     20030121 14:39:09 PST
 * Description:   Server-side glue code for Hypre.Pilut
 * 
 * WARNING: Automatically generated; changes will be lost
 * 
 * babel-version = 0.8.0
 * source-line   = 460
 * source-url    = file:/home/painter/linear_solvers/babel/Interfaces.idl
 */

#include "Hypre_Pilut_IOR.h"
#include "Hypre_Pilut.h"
#include <stddef.h>

extern void
impl_Hypre_Pilut__ctor(
  Hypre_Pilut);

extern void
impl_Hypre_Pilut__dtor(
  Hypre_Pilut);

extern int32_t
impl_Hypre_Pilut_SetCommunicator(
  Hypre_Pilut,
  void*);

extern int32_t
impl_Hypre_Pilut_GetDoubleValue(
  Hypre_Pilut,
  const char*,
  double*);

extern int32_t
impl_Hypre_Pilut_GetIntValue(
  Hypre_Pilut,
  const char*,
  int32_t*);

extern int32_t
impl_Hypre_Pilut_SetDoubleParameter(
  Hypre_Pilut,
  const char*,
  double);

extern int32_t
impl_Hypre_Pilut_SetIntParameter(
  Hypre_Pilut,
  const char*,
  int32_t);

extern int32_t
impl_Hypre_Pilut_SetStringParameter(
  Hypre_Pilut,
  const char*,
  const char*);

extern int32_t
impl_Hypre_Pilut_SetIntArrayParameter(
  Hypre_Pilut,
  const char*,
  struct SIDL_int__array*);

extern int32_t
impl_Hypre_Pilut_SetDoubleArrayParameter(
  Hypre_Pilut,
  const char*,
  struct SIDL_double__array*);

extern int32_t
impl_Hypre_Pilut_Setup(
  Hypre_Pilut,
  Hypre_Vector,
  Hypre_Vector);

extern int32_t
impl_Hypre_Pilut_Apply(
  Hypre_Pilut,
  Hypre_Vector,
  Hypre_Vector*);

extern int32_t
impl_Hypre_Pilut_SetOperator(
  Hypre_Pilut,
  Hypre_Operator);

extern int32_t
impl_Hypre_Pilut_GetResidual(
  Hypre_Pilut,
  Hypre_Vector*);

extern int32_t
impl_Hypre_Pilut_SetLogging(
  Hypre_Pilut,
  int32_t);

extern int32_t
impl_Hypre_Pilut_SetPrintLevel(
  Hypre_Pilut,
  int32_t);

void
Hypre_Pilut__set_epv(struct Hypre_Pilut__epv *epv)
{
  epv->f__ctor = impl_Hypre_Pilut__ctor;
  epv->f__dtor = impl_Hypre_Pilut__dtor;
  epv->f_SetCommunicator = impl_Hypre_Pilut_SetCommunicator;
  epv->f_GetDoubleValue = impl_Hypre_Pilut_GetDoubleValue;
  epv->f_GetIntValue = impl_Hypre_Pilut_GetIntValue;
  epv->f_SetDoubleParameter = impl_Hypre_Pilut_SetDoubleParameter;
  epv->f_SetIntParameter = impl_Hypre_Pilut_SetIntParameter;
  epv->f_SetStringParameter = impl_Hypre_Pilut_SetStringParameter;
  epv->f_SetIntArrayParameter = impl_Hypre_Pilut_SetIntArrayParameter;
  epv->f_SetDoubleArrayParameter = impl_Hypre_Pilut_SetDoubleArrayParameter;
  epv->f_Setup = impl_Hypre_Pilut_Setup;
  epv->f_Apply = impl_Hypre_Pilut_Apply;
  epv->f_SetOperator = impl_Hypre_Pilut_SetOperator;
  epv->f_GetResidual = impl_Hypre_Pilut_GetResidual;
  epv->f_SetLogging = impl_Hypre_Pilut_SetLogging;
  epv->f_SetPrintLevel = impl_Hypre_Pilut_SetPrintLevel;
}

struct Hypre_Pilut__data*
Hypre_Pilut__get_data(Hypre_Pilut self)
{
  return (struct Hypre_Pilut__data*)(self ? self->d_data : NULL);
}

void Hypre_Pilut__set_data(
  Hypre_Pilut self,
  struct Hypre_Pilut__data* data)
{
  if (self) {
    self->d_data = data;
  }
}
