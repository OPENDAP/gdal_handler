#include <iostream>
#include <string>

#include "DDS.h"
#include "cgi_util.h"
#include "DODSFilter.h"
#include "gdal.h"

#include "GDALTypes.h"

static void read_descriptors(DDS &dds, const string &filename)
throw  (Error);
extern void read_variables(DAS &dds, const string &filename)
throw  (Error);

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

/************************************************************************/
/*                          read_descriptors()                          */
/************************************************************************/

static void read_descriptors(DDS &dds, const string &filename) throw  (Error)

{
    GDALDatasetH hDS;

    dds.set_dataset_name( name_path( filename ) );

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
