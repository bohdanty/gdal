#include "get_raster_data.h"

#include <memory>

#include "gdal.h"

namespace {

void PrintInformationAboutDataset(GDALDatasetH dataset, GDALDriverH driver) {
  double adfGeoTransform[6];
  printf( "Driver: %s/%s\n",
      GDALGetDriverShortName( driver ),
      GDALGetDriverLongName( driver ) );
  printf( "Size is %dx%dx%d\n",
        GDALGetRasterXSize( dataset ),
        GDALGetRasterYSize( dataset ),
        GDALGetRasterCount( dataset ) );
  if( GDALGetProjectionRef( dataset ) != NULL )
      printf( "Projection is `%s'\n", GDALGetProjectionRef( dataset ) );
  if( GDALGetGeoTransform( dataset, adfGeoTransform ) == CE_None )
  {
      printf( "Origin = (%.6f,%.6f)\n",
            adfGeoTransform[0], adfGeoTransform[3] );
      printf( "Pixel Size = (%.6f,%.6f)\n",
            adfGeoTransform[1], adfGeoTransform[5] );
  }
}

}  // namespace

RasterDataset GetRasterBandsFromFile(const std::string& filename) {
  GDALAllRegister();
  GDALDatasetH dataset = GDALOpen(filename.data(), GA_ReadOnly);
  GDALDriverH driver = GDALGetDatasetDriver(dataset);

  PrintInformationAboutDataset(dataset, driver);

  RasterDataset result_dataset = {
    GDALGetRasterXSize(dataset),
    GDALGetRasterYSize(dataset)
  };

  if (GDALGetProjectionRef(dataset) != nullptr) {
    result_dataset.wkt_projection = std::string(GDALGetProjectionRef(dataset));
  }

  std::vector<double> geo_transform(6, 0.0);

  if (GDALGetGeoTransform(dataset, geo_transform.data()) == CE_None) {
    result_dataset.geo_transform = std::move(geo_transform);
  }

  int bands_count = GDALGetRasterCount(dataset);

  std::vector<RasterBandData> bands_data;
  bands_data.reserve(bands_count);

  for (int i = 1; i <= bands_count; ++i) {
    GDALRasterBandH band = GDALGetRasterBand(dataset, i);
    int width = GDALGetRasterBandXSize(band);
    int height = GDALGetRasterBandYSize(band);

    std::unique_ptr<int> no_data_result = nullptr;
    double no_data_value = GDALGetRasterNoDataValue(band, no_data_result.get());
    std::optional<double> no_data_value_holder = 
        no_data_result.get() == nullptr ? std::nullopt 
          : std::make_optional<double>(no_data_value);

    printf("no data val %f\n", no_data_value);

    std::vector<int> band_raster_data;

    band_raster_data.resize(width * height);
    // Use std::variant for the runtime template deduction or void* and reinterpet casts
    GDALRasterIO(band, GF_Read, 0, 0, width, height, band_raster_data.data(),
        width, height, GDT_Int32, 0, 0);

    bands_data.push_back(
      {
        width,
        height,
        std::move(band_raster_data),
        std::move(no_data_value_holder)
      }
    );
  }

  // GDALSetProjection(dataset, result_dataset.wkt_projection.c_str());

  result_dataset.bands = std::move(bands_data);

  return result_dataset;
}