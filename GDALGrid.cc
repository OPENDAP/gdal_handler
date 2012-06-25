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

//#define DODS_DEBUG 1

#include <string>

#include <cpl_conv.h>

#include <debug.h>

#include "GDALTypes.h"


/************************************************************************/
/* ==================================================================== */
/*                               GDALGrid                               */
/* ==================================================================== */
/************************************************************************/

void
GDALGrid::m_duplicate(const GDALGrid &g)
{
    DBG(cerr << "GDALGrid::m_duplicate: " << g.name() << "source pointer value: " << &g
    		<< ", dest pointer value: " << this << endl);

    hDS = g.hDS;
    hBand = g.hBand;
    eBufType = g.eBufType;
}


// protected

BaseType *
GDALGrid::ptr_duplicate()
{
    return new GDALGrid(*this);
}

// public

GDALGrid::GDALGrid(const string &n, GDALDatasetH hDSIn, 
                   GDALRasterBandH hBandIn, GDALDataType eBufTypeIn ) : Grid(n)
{
    hDS = hDSIn;
    hBand = hBandIn;
    eBufType = eBufTypeIn;
}

GDALGrid::GDALGrid(const GDALGrid &rhs) :Grid(rhs)
{
	m_duplicate(rhs);
}

GDALGrid &GDALGrid::operator=(const GDALGrid &rhs)
{
    if (this == &rhs)
        return *this;

    //static_cast<GDALGrid&>(*this) = rhs;
    m_duplicate(rhs);

    return *this;
}

GDALGrid::~GDALGrid()
{
}

bool
GDALGrid::read(/*const string &*/)
{
    DBG(cerr << "In GDALGrid::read" << endl);

    bool status = false;

    if (read_p()) // nothing to do
        return false;

/* -------------------------------------------------------------------- */
/*      Collect the x and y sampling values from the constraint.        */
/* -------------------------------------------------------------------- */
    GDALArray *array = dynamic_cast<GDALArray*>(array_var());
    Array::Dim_iter p = array->dim_begin();
    int start = array->dimension_start(p,true);
    int stride = array->dimension_stride(p, true);
    int stop = array->dimension_stop(p, true); 

    p++;
    int start_2 = array->dimension_start(p,true);
    int stride_2 = array->dimension_stride(p, true);
    int stop_2 = array->dimension_stop(p, true); 

    if(start+stop+stride == 0){ //default rows
        start = 0;
        stride = 1;
        stop = GDALGetRasterBandYSize( hBand )-1;
    }
    if(start_2+stop_2+stride_2 == 0){ //default columns
        start_2 = 0;
        stride_2 = 1;
        stop_2 = GDALGetRasterBandXSize( hBand )-1;
    }

/* -------------------------------------------------------------------- */
/*      Build a window and buf size from this.                          */
/* -------------------------------------------------------------------- */
    int nWinXOff, nWinYOff, nWinXSize, nWinYSize, nBufXSize, nBufYSize;

    nWinXOff = start_2;
    nWinYOff = start;
    nWinXSize = stop_2+1 - start_2;
    nWinYSize = stop+1 - start;

    nBufXSize = (stop_2 - start_2) / stride_2 + 1;
    nBufYSize = (stop   - start  ) / stride + 1;
    
/* -------------------------------------------------------------------- */
/*      Allocate buffer.                                                */
/* -------------------------------------------------------------------- */
    int nPixelSize = GDALGetDataTypeSize( eBufType ) / 8;
    // TODO use vector<char>
    void *pData = CPLMalloc( nBufXSize * nBufYSize * nPixelSize );

/* -------------------------------------------------------------------- */
/*      Read request into buffer.                                       */
/* -------------------------------------------------------------------- */
    CPLErr eErr;

    eErr = 
        GDALRasterIO( hBand, GF_Read, 
                      nWinXOff, nWinYOff, nWinXSize, nWinYSize, 
                      pData, nBufXSize, nBufYSize, eBufType, 0, 0 );

    
    // TODO should check for errors!

    // TODO use set_value()
    array->val2buf( pData );
    array->set_read_p( true );
    //set_read_p( true ); Set at the end of the method. jhrg 6/24/12


    CPLFree( pData );

/* -------------------------------------------------------------------- */
/*      Read or default the geotransform used to generate the           */
/*      georeferencing maps.                                            */
/* -------------------------------------------------------------------- */
    double adfGeoTransform[6];

    if( GDALGetGeoTransform( hDS, adfGeoTransform ) != CE_None )
    {
        adfGeoTransform[0] = 0.0;
        adfGeoTransform[1] = 1.0;
        adfGeoTransform[2] = 0.0;
        adfGeoTransform[3] = 0.0;
        adfGeoTransform[4] = 0.0;
        adfGeoTransform[5] = 1.0;
    }

/* -------------------------------------------------------------------- */
/*      Set "y" map array.                                              */
/* -------------------------------------------------------------------- */
    double *padfMap;
    int i, iPixel, iLine;

    padfMap = (double *) CPLMalloc(sizeof(double) * nBufYSize);

    for( i = 0, iLine = start; iLine <= stop; iLine += stride )
    {
        padfMap[i++] = adfGeoTransform[3] + adfGeoTransform[5] * iLine;
    }

    Map_iter miter = map_begin(); 
    array = dynamic_cast<GDALArray*>( (*miter) );
    array->val2buf( (void *) padfMap );
    array->set_read_p( true );
    
    CPLFree( padfMap );

/* -------------------------------------------------------------------- */
/*      Set the "x" map.                                                */
/* -------------------------------------------------------------------- */

    padfMap = (double *) CPLMalloc(sizeof(double) * nBufXSize);

    for( i = 0, iPixel = start_2; iPixel <= stop_2; iPixel += stride_2 )
    {
        padfMap[i++] = adfGeoTransform[0] + iPixel * adfGeoTransform[1];
    }

    ++miter;
    array = dynamic_cast<GDALArray*>( *miter );
    array->val2buf( (void *) padfMap );
    array->set_read_p( true );
    
    CPLFree( padfMap );

    // TODO Added this; maybe it's not needed? jhrg 6/21/12
    set_read_p(true);

    return status;
}
