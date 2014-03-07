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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"
//#define DODS_DEBUG 1

#include <string>

#include <BESDebug.h>

#include <InternalErr.h>
#include <debug.h>

#include "GDALTypes.h"

using namespace libdap ;

/************************************************************************/
/* ==================================================================== */
/*                              GDALArray                               */
/* ==================================================================== */
/************************************************************************/

BaseType *
GDALArray::ptr_duplicate()
{
    return new GDALArray(*this);
}

GDALArray::GDALArray(const string &n, BaseType *v) : Array(n, v)
{
    BESDEBUG("gdal", " Called GDALArray::GDALARRAY()" << endl);
}

GDALArray::~GDALArray()
{
}

bool
GDALArray::read()
{
    DBG(cerr << "In GDALArray::read" << endl);

    if (read_p())
        return true;

    return true;
}

// $Log: GDALArray.cc,v $
// Revision 1.1  2004/10/19 20:38:28  warmerda
// New
//
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//
