
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

#include <iostream>
#include <string>

#include <gdal.h>

#include <DDS.h>
#include <DAS.h>
#include <ResponseBuilder.h>
#include <debug.h>

#include "GDALTypes.h"

void gdal_read_dataset_variables(DDS &dds, const string &filename);
extern void gdal_read_dataset_attributes(DAS &dds, const string &filename);

/************************************************************************/
/*                          read_descriptors()                          */
/************************************************************************/

void gdal_read_dataset_variables(DDS &dds, const string &filename)
{
    GDALDatasetH hDS;
/* -------------------------------------------------------------------- */
/*      Open the dataset.                                               */
/* -------------------------------------------------------------------- */
    GDALAllRegister();
    hDS = GDALOpen(filename.c_str(), GA_ReadOnly);

    if (hDS == NULL)
        throw Error(string(CPLGetLastErrorMsg()));


/* -------------------------------------------------------------------- */
/*      Create the basic matrix for each band.                          */
/* -------------------------------------------------------------------- */
    GDALDataType eBufType; 

    for( int iBand = 0; iBand < GDALGetRasterCount( hDS ); iBand++ )
    {
        DBG(cerr << "In dgal_dds.cc  iBand" << endl);

        GDALRasterBandH hBand = GDALGetRasterBand( hDS, iBand+1 );
        //BaseType *bt;
        // TODO ostringstream
        char szName[32];
        // nsprintf if not ostringstream
        sprintf( szName, "band_%d", iBand+1 );

        eBufType = GDALGetRasterDataType( hBand );

        BaseType *bt;
        switch( GDALGetRasterDataType( hBand ) )
        {
          case GDT_Byte:
            bt = new Byte( string(szName) );
            break;

          case GDT_UInt16:
            bt = new UInt16( string(szName) );
            break;

          case GDT_Int16:
            bt = new Int16( string(szName) );
            break;

          case GDT_UInt32:
            bt = new UInt32( string(szName) );
            break;

          case GDT_Int32:
            bt = new Int32( string(szName) );
            break;

          case GDT_Float32:
            bt = new Float32( string(szName) );
            break;

          case GDT_Float64:
            bt = new Float64( string(szName) );
            break;

          case GDT_CFloat32:
          case GDT_CFloat64:
          case GDT_CInt16:
          case GDT_CInt32:
          default:
            // TODO eventually we need to preserve complex info
            bt = new Float64( string(szName) );
            eBufType = GDT_Float64;
            break;
        }

/* -------------------------------------------------------------------- */
/*      Create a grid to hold the raster.                               */
/* -------------------------------------------------------------------- */
        Grid *grid;
        grid = new GDALGrid( filename, hBand, eBufType );

/* -------------------------------------------------------------------- */
/*      Make into an Array for the raster data with appropriate         */
/*      dimensions.                                                     */
/* -------------------------------------------------------------------- */
        Array *ar;
        ar = new GDALArray( string(szName), bt );
        
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );

        grid->add_var( ar, array );

/* -------------------------------------------------------------------- */
/*      Add the dimension map arrays.                                   */
/* -------------------------------------------------------------------- */
        bt = new GDALFloat64( string("northing") );
        ar = new GDALArray( "northing", bt );
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );
        grid->add_var( ar, maps );

        bt = new GDALFloat64( string("easting") );
        ar = new GDALArray( "easting", bt );
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );
        grid->add_var( ar, maps );

        DBG(cerr << "Type of grid: " << typeid(grid).name() << endl);

        dds.add_var( grid );
    }

/* -------------------------------------------------------------------- */
/*      Close the dataset.                                               */
/* -------------------------------------------------------------------- */
    GDALClose(hDS);
}

// $Log: gdal_handler.cc,v $
// Revision 1.1  2004/10/19 20:38:28  warmerda
// New
//
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//
