
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
#include <sstream>
#include <string>

#include <gdal.h>

#include <DDS.h>
#include <DAS.h>
#include <ResponseBuilder.h>
#include <debug.h>

#include "GDALTypes.h"

extern void gdal_read_dataset_attributes(DAS &dds, const string &filename);

/************************************************************************/
/*                          read_descriptors()                          */
/************************************************************************/

GDALDatasetH gdal_read_dataset_variables(DDS *dds, const string &filename)
{
    GDALDatasetH hDS;
/* -------------------------------------------------------------------- */
/*      Open the dataset.                                               */
/* -------------------------------------------------------------------- */

    // TODO Since this code no longer closes the dataset, it would be
    // better to move the open operation outside it, to the caller.
    // jhrg 7/26/12
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
#if 0
        //BaseType *bt;
        // TODO ostringstream
        char szName[32];
        // nsprintf if not ostringstream
        sprintf( szName, "band_%d", iBand+1 );
#endif
        ostringstream oss;
        oss << "band_" << iBand+1;

        eBufType = GDALGetRasterDataType( hBand );

        BaseType *bt;
        switch( GDALGetRasterDataType( hBand ) )
        {
          case GDT_Byte:
            bt = new Byte( oss.str() );
            break;

          case GDT_UInt16:
            bt = new UInt16( oss.str() );
            break;

          case GDT_Int16:
            bt = new Int16( oss.str() );
            break;

          case GDT_UInt32:
            bt = new UInt32( oss.str() );
            break;

          case GDT_Int32:
            bt = new Int32( oss.str() );
            break;

          case GDT_Float32:
            bt = new Float32( oss.str() );
            break;

          case GDT_Float64:
            bt = new Float64( oss.str() );
            break;

          case GDT_CFloat32:
          case GDT_CFloat64:
          case GDT_CInt16:
          case GDT_CInt32:
          default:
            // TODO eventually we need to preserve complex info
            bt = new Float64( oss.str() );
            eBufType = GDT_Float64;
            break;
        }

/* -------------------------------------------------------------------- */
/*      Create a grid to hold the raster.                               */
/* -------------------------------------------------------------------- */
        Grid *grid;
        grid = new GDALGrid( filename, hBand, eBufType );
        grid->set_name(oss.str());

/* -------------------------------------------------------------------- */
/*      Make into an Array for the raster data with appropriate         */
/*      dimensions.                                                     */
/* -------------------------------------------------------------------- */
        Array *ar;
        // A 'feature' of Array is that it copies the variable passed to
        // its ctor. To get around that, pass null and use add_var_nocopy().
        ar = new GDALArray( oss.str(), 0 );
        ar->add_var_nocopy( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );

        grid->add_var_nocopy( ar, array );

/* -------------------------------------------------------------------- */
/*      Add the dimension map arrays.                                   */
/* -------------------------------------------------------------------- */
        bt = new GDALFloat64( "northing" );
        ar = new GDALArray( "northing", 0 );
        ar->add_var_nocopy( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );

        grid->add_var_nocopy( ar, maps );

        bt = new GDALFloat64( "easting" );
        ar = new GDALArray( "easting", 0 );
        ar->add_var_nocopy( bt );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );

        grid->add_var_nocopy( ar, maps );

        DBG(cerr << "Type of grid: " << typeid(grid).name() << endl);

        dds->add_var_nocopy( grid );
    }

    return hDS;
}
