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

class GDALArray: public Array {
public:
    GDALArray(const string &n = "", BaseType *v = 0);
    virtual ~GDALArray() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

/************************************************************************/
/*                               GDALGrid                               */
/************************************************************************/

class GDALGrid: public Grid {

    GDALRasterBandH hBand;
    GDALDataType    eBufType;
    string filename;   
    
    void m_duplicate(const GDALGrid &g);

public:
    GDALGrid(const GDALGrid &rhs);
    GDALGrid(const string &filenameIn, GDALRasterBandH, GDALDataType);   
    virtual ~GDALGrid();

    GDALGrid &operator=(const GDALGrid &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool read(/*const string &dataset*/);
};

// $Log: GDALTypes.h,v $
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//

#endif // ndef _GDALTypes_h


