// This file is part of the GDAL OPeNDAP Adapter

// Copyright (c) 2004 OPeNDAP, Inc.
// Author: Frank Warmerdam <warmerdam@pobox.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <string>

#include <InternalErr.h>

#include "GDALTypes.h"

using namespace libdap ;

/************************************************************************/
/* ==================================================================== */
/*                               GDALByte                               */
/* ==================================================================== */
/************************************************************************/

GDALByte::GDALByte(const string &n) : Byte(n)
{
}

BaseType *
GDALByte::ptr_duplicate()
{
    return new GDALByte(*this);
}

bool
GDALByte::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                               GDALUInt16                             */
/* ==================================================================== */
/************************************************************************/

GDALUInt16::GDALUInt16(const string &n) : UInt16(n)
{
}

BaseType *
GDALUInt16::ptr_duplicate()
{
    return new GDALUInt16(*this);
}

bool
GDALUInt16::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                               GDALInt16                              */
/* ==================================================================== */
/************************************************************************/

GDALInt16::GDALInt16(const string &n) : Int16(n)
{
}

BaseType *
GDALInt16::ptr_duplicate()
{
    return new GDALInt16(*this);
}

bool
GDALInt16::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                               GDALUInt32                             */
/* ==================================================================== */
/************************************************************************/

GDALUInt32::GDALUInt32(const string &n) : UInt32(n)
{
}

BaseType *
GDALUInt32::ptr_duplicate()
{
    return new GDALUInt32(*this);
}

bool
GDALUInt32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                               GDALInt32                               */
/* ==================================================================== */
/************************************************************************/

GDALInt32::GDALInt32(const string &n) : Int32(n)
{
}

BaseType *
GDALInt32::ptr_duplicate()
{
    return new GDALInt32(*this);
}

bool
GDALInt32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                              GDALFloat32                             */
/* ==================================================================== */
/************************************************************************/

GDALFloat32::GDALFloat32(const string &n) : Float32(n)
{
}

BaseType *
GDALFloat32::ptr_duplicate()
{
    return new GDALFloat32(*this);
}

bool
GDALFloat32::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                              GDALFloat64                             */
/* ==================================================================== */
/************************************************************************/

GDALFloat64::GDALFloat64(const string &n) : Float64(n)
{
}

BaseType *
GDALFloat64::ptr_duplicate()
{
    return new GDALFloat64(*this);
}

bool
GDALFloat64::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

/************************************************************************/
/* ==================================================================== */
/*                              GDALArray                               */
/* ==================================================================== */
/************************************************************************/

GDALArray::GDALArray(const string &n, BaseType *v) : Array(n, v)
{
}

BaseType *
GDALArray::ptr_duplicate()
{
    return new GDALArray(*this);
}

bool
GDALArray::read(const string &)
{
    throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}
