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
#include <debug.h>

#include "GDALTypes.h"

using namespace libdap ;

/************************************************************************/
/* ==================================================================== */
/*                              GDALArray                               */
/* ==================================================================== */
/************************************************************************/
#if 0
Array *
NewArray(const string &n, BaseType *v)
{
    cerr << "Making a new GDALArray" << endl;

    return new GDALArray(n, v);
}
#endif

BaseType *
GDALArray::ptr_duplicate()
{
    return new GDALArray(*this);
}

GDALArray::GDALArray(const string &n, BaseType *v) : Array(n, v)
{
}

GDALArray::~GDALArray()
{
}

bool
GDALArray::read(const string &)
{
    DBG(cerr << "In GDALArray::read" << endl);

    if (read_p())
        return false;
    throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

// $Log: GDALArray.cc,v $
// Revision 1.1  2004/10/19 20:38:28  warmerda
// New
//
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//
