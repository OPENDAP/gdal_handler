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

#include <string>
#include "GDALTypes.h"
#include "cpl_conv.h"

/************************************************************************/
/* ==================================================================== */
/*                               GDALGrid                               */
/* ==================================================================== */
/************************************************************************/

Grid *
NewGrid(const string &n, GDALDatasetH hDS, GDALRasterBandH hBand,
        GDALDataType eBufType )
{
    return new GDALGrid(n, hDS, hBand, eBufType);
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

GDALGrid::~GDALGrid()
{
}

bool
GDALGrid::read(const string &dataset)
{
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
    void *pData = CPLMalloc( nBufXSize * nBufYSize * nPixelSize );

/* -------------------------------------------------------------------- */
/*      Read request into buffer.                                       */
/* -------------------------------------------------------------------- */
    CPLErr eErr;

    eErr = 
        GDALRasterIO( hBand, GF_Read, 
                      nWinXOff, nWinYOff, nWinXSize, nWinYSize, 
                      pData, nBufXSize, nBufYSize, eBufType, 0, 0 );

    
    // should check for errors!

    array->set_read_p( true );
    set_read_p( true );

    array->val2buf( pData );

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

    for( i = 0, iPixel = start; iPixel <= stop; iPixel += stride )
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

    for( i = 0, iLine = start_2; iLine <= stop_2; iLine += stride_2 )
    {
        padfMap[i++] = adfGeoTransform[0] + iPixel * adfGeoTransform[1];
    }

    ++miter;
    array = dynamic_cast<GDALArray*>( *miter );
    array->val2buf( (void *) padfMap );
    array->set_read_p( true );
    
    CPLFree( padfMap );

    return status;
}
