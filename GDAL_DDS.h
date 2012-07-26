/*
 * GDALDDS.h
 *
 *  Created on: Jul 25, 2012
 *      Author: jimg
 */

#ifndef GDAL_DDS_H_
#define GDAL_DDS_H_

#include "config.h"

#include <gdal.h>

#include <DataDDS.h>
#include <InternalErr.h>

using namespace libdap;

/**
 * This specialization of DDS is used to manage the 'resource' of the open
 * GDAL dataset handle so that the BES will close that handle once the
 * framework is done working with the file. This provides a way for the
 * code in gdal_dds.cc to read binary objects from the file using the gdal
 * library and embed those in instances of Grid. Those Grid variables are
 * used later on (but during the same service request, so the binary data
 * are still valid). When the DDS is deleted by the BES, the GDALDDS()
 * destructor closes the file.
 *
 * @note This was a great idea, but it's not possible without modifying the
 * BES so that it can be given a new DDS object to use.
 *
 * @todo Change DataDDS to DDS if we can...
 */
class GDALDDS : public DataDDS {
private:
    GDALDatasetH d_hDS;

    void m_duplicate(const GDALDDS &src) { d_hDS = src.d_hDS; }

public:
    GDALDDS(DataDDS *ddsIn) : DataDDS(*ddsIn), d_hDS(0) {}
    GDALDDS(BaseTypeFactory *factory, const string &name) : DataDDS(factory, name), d_hDS(0) {}

    GDALDDS(const GDALDDS &rhs) : DataDDS(rhs) {
        m_duplicate(rhs);
    }

    GDALDDS & operator= (const GDALDDS &rhs) {
        if (this == &rhs)
            return *this;

        m_duplicate(rhs);

        return *this;
    }

    ~GDALDDS() {
        if (d_hDS)
            GDALClose(d_hDS);
    }

    void setGDALDataset(const GDALDatasetH &hDSIn) { d_hDS = hDSIn; }
    GDALDatasetH &GDALDataset() { return d_hDS; }
};


#endif /* GDAL_DDS_H_ */
