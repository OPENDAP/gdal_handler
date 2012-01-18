
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

#include <iostream>
#include <string>

#include <gdal.h>

#include <DDS.h>
#include <DAS.h>
#include <REsponseBuilder.h>

#include "GDALTypes.h"

void gdal_read_dataset_variables(DDS &dds, const string &filename);
extern void gdal_read_dataset_attributes(DAS &dds, const string &filename);

#if 0

// Noise from the old days... jhrg 1/12/12

const string cgi_version = "gdal-dods/3.4.8"; /* ?? */

/************************************************************************/
/*                                main()                                */
/************************************************************************/
int 
main(int argc, char *argv[])
{
    int i;

    fprintf( stderr, "----------------\n" );
    for( i = 0; i < argc; i++ )
        fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\n" );
    fprintf( stderr, "----------------\n" );

    try { 
	DODSFilter df(argc, argv);
	if (df.get_cgi_version() == "")
	    df.set_cgi_version(cgi_version);

/* -------------------------------------------------------------------- */
/*      Switch based on the request made.                               */
/* -------------------------------------------------------------------- */

	switch (df.get_response()) {

	  case DODSFilter::DAS_Response: {
	    DAS das;

	    read_variables(das, df.get_dataset_name());
	    df.read_ancillary_das(das);
	    df.send_das(das);
	    break;
	  }

	  case DODSFilter::DDS_Response: {
	    DDS dds;

	    read_descriptors(dds, df.get_dataset_name());
	    df.read_ancillary_dds(dds);
	    df.send_dds(dds, true);
	    break;
	  }

	  case DODSFilter::DataDDS_Response: {
	    DDS dds;

	    dds.filename(df.get_dataset_name());
	    read_descriptors(dds, df.get_dataset_name()); 
	    df.read_ancillary_dds(dds);
	    df.send_data(dds, stdout);
	    break;
	  }

#ifdef notdef
	  case DODSFilter::DDX_Response: {
	    DDS dds;
	    DAS das;

	    dds.filename(df.get_dataset_name());

	    read_descriptors(dds, df.get_dataset_name()); 
	    df.read_ancillary_dds(dds);

	    read_variables(das, df.get_dataset_name());
	    df.read_ancillary_das(das);

	    dds.transfer_attributes(&das);

	    df.send_ddx(dds, stdout);
	    break;
	  }
#endif

	  case DODSFilter::Version_Response: {
	    df.send_version_info();

	    break;
	  }

	  default:
	    df.print_usage();	// Throws Error
	}
    }
    catch (Error &e) {
	set_mime_text(cout, dods_error, cgi_version);
	e.print(cout);
	return 1;
    }

    return 0;
}
#endif

/************************************************************************/
/*                          read_descriptors()                          */
/************************************************************************/

void gdal_read_dataset_variables(DDS &dds, const string &filename)
{
    GDALDatasetH hDS;
#if 0
    // Old? From teh Server3 days? jhrg 1/12/12
    dds.set_dataset_name( name_path( filename ) );
#endif
/* -------------------------------------------------------------------- */
/*      Open the dataset.                                               */
/* -------------------------------------------------------------------- */
        GDALAllRegister();
        hDS = GDALOpen( filename.c_str(), GA_ReadOnly );
        
        if( hDS == NULL )
            throw Error(string(CPLGetLastErrorMsg()));


/* -------------------------------------------------------------------- */
/*      Create the basic matrix for each band.                          */
/* -------------------------------------------------------------------- */
    GDALDataType eBufType; 

    for( int iBand = 0; iBand < GDALGetRasterCount( hDS ); iBand++ )
    {
        GDALRasterBandH hBand = GDALGetRasterBand( hDS, iBand+1 );
        BaseType *bt;
        char szName[32];

        sprintf( szName, "band_%d", iBand+1 );

        eBufType = GDALGetRasterDataType( hBand );

        switch( GDALGetRasterDataType( hBand ) )
        {
          case GDT_Byte:
            bt = NewByte( string(szName) );
            break;

          case GDT_UInt16:
            bt = NewUInt16( string(szName) );
            break;
            
          case GDT_Int16:
            bt = NewInt16( string(szName) );
            break;
            
          case GDT_UInt32:
            bt = NewUInt32( string(szName) );
            break;
            
          case GDT_Int32:
            bt = NewInt32( string(szName) );
            break;
            
          case GDT_Float32:
            bt = NewFloat32( string(szName) );
            break;
            
          case GDT_Float64:
            bt = NewFloat64( string(szName) );
            break;
            
          case GDT_CFloat32:
          case GDT_CFloat64:
          case GDT_CInt16:
          case GDT_CInt32:
          default:
            // eventually we need to preserve complex info 
            bt = NewFloat64( string(szName) ); 
            eBufType = GDT_Float64;
            break;
        }

/* -------------------------------------------------------------------- */
/*      Create a grid to hold the raster.                               */
/* -------------------------------------------------------------------- */
        Grid *grid;

        grid = NewGrid( string(szName), hDS, hBand, eBufType );

/* -------------------------------------------------------------------- */
/*      Make into an Array for the raster data with appropriate         */
/*      dimensions.                                                     */
/* -------------------------------------------------------------------- */
        Array *ar;
        ar = NewArray( string(szName), bt );
        
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );

        grid->add_var( ar, array );

/* -------------------------------------------------------------------- */
/*      Add the dimension map arrays.                                   */
/* -------------------------------------------------------------------- */
        bt = NewFloat64( string("northing") );
        ar = NewArray( "northing", bt );
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterYSize( hDS ), "northing" );
        grid->add_var( ar, maps );

        bt = NewFloat64( string("easting") );
        ar = NewArray( "easting", bt );
        ar->add_var( bt );
        ar->append_dim( GDALGetRasterXSize( hDS ), "easting" );
        grid->add_var( ar, maps );

        dds.add_var( grid );
    }
        
}

// $Log: gdal_handler.cc,v $
// Revision 1.1  2004/10/19 20:38:28  warmerda
// New
//
// Revision 1.1  2004/10/04 14:29:29  warmerda
// New
//
