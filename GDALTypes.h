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

#ifndef _GDALTypes_h
#define _GDALTypes_h 1

#include <gdal.h>

#include <Byte.h>
#include <UInt16.h>
#include <Int16.h>
#include <UInt32.h>
#include <Int32.h>
#include <Float32.h>
#include <Float64.h>
#include <Array.h>
#include <Grid.h>

using namespace libdap ;

/************************************************************************/
/*                               GDALByte                               */
/************************************************************************/

extern Byte * NewByte(const string &n = "");

class GDALByte: public Byte {
public:
    GDALByte(const string &n = "");
    virtual ~GDALByte() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                              GDALUInt16                              */
/************************************************************************/

extern UInt16 * NewUInt16(const string &n = "");

class GDALUInt16: public UInt16 {
public:
    GDALUInt16(const string &n = "");
    virtual ~GDALUInt16() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                              GDALInt16                               */
/************************************************************************/

extern Int16 * NewInt16(const string &n = "");

class GDALInt16: public Int16 {
public:
    GDALInt16(const string &n = "");
    virtual ~GDALInt16() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                              GDALUInt32                               */
/************************************************************************/

extern UInt32 * NewUInt32(const string &n = "");

class GDALUInt32: public UInt32 {
public:
    GDALUInt32(const string &n = "");
    virtual ~GDALUInt32() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                              GDALInt32                               */
/************************************************************************/

extern Int32 * NewInt32(const string &n = "");

class GDALInt32: public Int32 {
public:
    GDALInt32(const string &n = "");
    virtual ~GDALInt32() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                             GDALFloat32                              */
/************************************************************************/

extern Float32 * NewFloat32(const string &n = "");

class GDALFloat32: public Float32 {
public:
    GDALFloat32(const string &n = "");
    virtual ~GDALFloat32() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                             GDALFloat64                              */
/************************************************************************/

extern Float64 * NewFloat64(const string &n = "");

class GDALFloat64: public Float64 {
public:
    GDALFloat64(const string &n = "");
    virtual ~GDALFloat64() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                              GDALArray                               */
/************************************************************************/

extern Array * NewArray(const string &n , BaseType *v);

class GDALArray: public Array {
public:
    GDALArray(const string &n = "", BaseType *v = 0);
    virtual ~GDALArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                               GDALGrid                               */
/************************************************************************/

extern Grid * NewGrid(const string &n, GDALDatasetH, GDALRasterBandH, 
                      GDALDataType );

class GDALGrid: public Grid {

    GDALDatasetH    hDS;
    GDALRasterBandH hBand;
    GDALDataType    eBufType;
    
public:
    GDALGrid(const string &n, GDALDatasetH, GDALRasterBandH, GDALDataType );
    virtual ~GDALGrid();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

// $Log: GDALTypes.h,v $
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//

#endif // ndef _GDALTypes_h


