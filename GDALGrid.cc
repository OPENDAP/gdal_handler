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
#include "BESDebug.h"

//#define DODS_DEBUG 1

#include <string>

#include <gdal.h>

#include <cpl_conv.h>

#include <debug.h>

#include "GDALTypes.h"

/************************************************************************/
/* ==================================================================== */
/*                               GDALGrid                               */
/* ==================================================================== */
/************************************************************************/

void GDALGrid::m_duplicate(const GDALGrid &g)
{
	DBG(cerr << "GDALGrid::m_duplicate: " << g.name() << "source pointer value: " << &g
			<< ", dest pointer value: " << this << endl);

	filename = g.filename;
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

GDALGrid::GDALGrid(const string &filenameIn, const string &name, GDALRasterBandH hBandIn, GDALDataType eBufTypeIn) :
		Grid(name)
{
	filename = filenameIn;	// This is used to open the file in the read() method for GDALGrid
	hBand = hBandIn;
	eBufType = eBufTypeIn;
}

GDALGrid::GDALGrid(const GDALGrid &rhs) :
		Grid(rhs)
{
	m_duplicate(rhs);
}

GDALGrid &GDALGrid::operator=(const GDALGrid &rhs)
{
	if (this == &rhs) return *this;

	m_duplicate(rhs);

	return *this;
}

GDALGrid::~GDALGrid()
{
}

bool GDALGrid::read()
{
	BESDEBUG("gdal", "Entering GDALGrid::read()" << endl);

	if (read_p()) // nothing to do
		return false;

	/* -------------------------------------------------------------------- */
	/*      Collect the x and y sampling values from the constraint.        */
	/* -------------------------------------------------------------------- */
	GDALArray *array = static_cast<GDALArray*>(array_var());
	Array::Dim_iter p = array->dim_begin();
	int start = array->dimension_start(p, true);
	int stride = array->dimension_stride(p, true);
	int stop = array->dimension_stop(p, true);

	p++;
	int start_2 = array->dimension_start(p, true);
	int stride_2 = array->dimension_stride(p, true);
	int stop_2 = array->dimension_stop(p, true);

	if (start + stop + stride == 0) { //default rows
		start = 0;
		stride = 1;
		stop = GDALGetRasterBandYSize(hBand) - 1;
	}
	if (start_2 + stop_2 + stride_2 == 0) { //default columns
		start_2 = 0;
		stride_2 = 1;
		stop_2 = GDALGetRasterBandXSize(hBand) - 1;
	}

	/* -------------------------------------------------------------------- */
	/*      Build a window and buf size from this.                          */
	/* -------------------------------------------------------------------- */
	int nWinXOff, nWinYOff, nWinXSize, nWinYSize, nBufXSize, nBufYSize;

	nWinXOff = start_2;
	nWinYOff = start;
	nWinXSize = stop_2 + 1 - start_2;
	nWinYSize = stop + 1 - start;

	nBufXSize = (stop_2 - start_2) / stride_2 + 1;
	nBufYSize = (stop - start) / stride + 1;

	/* -------------------------------------------------------------------- */
	/*      Allocate buffer.                                                */
	/* -------------------------------------------------------------------- */
	int nPixelSize = GDALGetDataTypeSize(eBufType) / 8;
	vector<char> pData(nBufXSize * nBufYSize * nPixelSize);

	/* -------------------------------------------------------------------- */
	/*      Read request into buffer.                                       */
	/* -------------------------------------------------------------------- */
	CPLErr eErr = GDALRasterIO(hBand, GF_Read, nWinXOff, nWinYOff, nWinXSize, nWinYSize,
			&pData[0], nBufXSize, nBufYSize, eBufType, 0, 0);
	if (eErr != CE_None)
		throw Error("Error reading: " + name());

	array->val2buf(&pData[0]);
	array->set_read_p(true);

	/* -------------------------------------------------------------------- */
	/*      Read or default the geotransform used to generate the           */
	/*      georeferencing maps.                                            */
	/* -------------------------------------------------------------------- */

	// Move this into the gdal_dds.cc code so that it store this in the
	// Grid or maybe in the GDALDDS instance? Then we can avoid a second
	// open/read operation on the file. jhrg
	GDALDatasetH hDS;
	GDALAllRegister(); // even though the calling function called this.

	hDS = GDALOpen(filename.c_str(), GA_ReadOnly);

	if (hDS == NULL) throw Error(string(CPLGetLastErrorMsg()));

	double adfGeoTransform[6];

	if (GDALGetGeoTransform(hDS, adfGeoTransform) != CE_None) {
		adfGeoTransform[0] = 0.0;
		adfGeoTransform[1] = 1.0;
		adfGeoTransform[2] = 0.0;
		adfGeoTransform[3] = 0.0;
		adfGeoTransform[4] = 0.0;
		adfGeoTransform[5] = 1.0;
	}

	GDALClose(hDS);

	/* -------------------------------------------------------------------- */
	/*      Set "y" map array.                                              */
	/* -------------------------------------------------------------------- */
	vector<double> padfMap(nBufYSize);

	for (int i = 0, iLine = start; iLine <= stop; iLine += stride) {
		padfMap[i++] = adfGeoTransform[3] + adfGeoTransform[5] * iLine;
	}

	Map_iter miter = map_begin();
	array = static_cast<GDALArray*>((*miter));
	array->val2buf((void *) &padfMap[0]);
	array->set_read_p(true);

	/* -------------------------------------------------------------------- */
	/*      Set the "x" map.                                                */
	/* -------------------------------------------------------------------- */
	padfMap.reserve(nBufXSize);

	for (int i = 0, iPixel = start_2; iPixel <= stop_2; iPixel += stride_2) {
		padfMap[i++] = adfGeoTransform[0] + iPixel * adfGeoTransform[1];
	}

	++miter;
	array = static_cast<GDALArray*>(*miter);
	array->val2buf((void *) &padfMap[0]);
	array->set_read_p(true);

	set_read_p(true);

	return false;
}
