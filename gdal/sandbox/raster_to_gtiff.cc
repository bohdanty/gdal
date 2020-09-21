#include <iostream>
#include <vector>

#include "get_raster_data.h"
#include "gdal_sandbox.h"

int main(int argc, char** argv) {
  const std::string input_data_path = "../dataset.tif";
  const std::string output_data_folder = "/home/asdf/Desktop/gdal/gdal/sandbox";
  std::string out_file_path = "/home/asdf/Desktop/gdal/gdal/sandbox/sand_test_1.tif";

  RasterDataset bands_data = GetRasterBandsFromFile(input_data_path);

  std::cout << "Amount of bands: " << bands_data.bands.size() << std::endl;
  for (int i = 0; i < bands_data.bands.size(); ++i) {
    std::cout << bands_data.bands[i].width << "x" << bands_data.bands[i].height 
        << " " << bands_data.bands[i].data.size() << std::endl;
  }

  GdalSapiSandbox sandbox(output_data_folder);
  sandbox.Init().IgnoreError();
  gdalApi api(&sandbox);
  api.GDALAllRegister().IgnoreError();
  
  std::string driver_name = "GTiff";
  sapi::v::CStr driver_name_ptr(driver_name.data());

  auto driver = api.GDALGetDriverByName(driver_name_ptr.PtrBefore());
  std::cout << "Driver address: " << driver.value() << std::endl;
  sapi::v::RemotePtr driver_ptr(driver.value());
  
  sapi::v::CStr out_file_path_ptr(out_file_path.data());
  sapi::v::NullPtr options;

  auto dataset = api.GDALCreate(&driver_ptr, out_file_path_ptr.PtrBefore(), 
      bands_data.width, bands_data.height, bands_data.bands.size(), GDT_UInt16,
      &options);

  std::cout << "Dataset address: " << dataset.value() << std::endl;
  sapi::v::RemotePtr dataset_ptr(dataset.value());

  for (int i = 0; i < bands_data.bands.size(); ++i) {
    auto current_band = api.GDALGetRasterBand(&dataset_ptr, i + 1);
    std::cout << "Band number " << (i + 1) << " address is: " << current_band.value() << std::endl;
    sapi::v::RemotePtr current_band_ptr(current_band.value());

    sapi::v::Array<int> data_array(bands_data.bands[i].data.data(), 
        bands_data.bands[i].data.size());

    api.GDALRasterIO(&current_band_ptr, GF_Write, 0, 0, 
        bands_data.bands[i].width, bands_data.bands[i].height,
        data_array.PtrBefore(), bands_data.bands[i].width, 
        bands_data.bands[i].height, GDT_Int32, 0, 0).IgnoreError();

    if (bands_data.bands[i].no_data_value.has_value()) {
      api.GDALSetRasterNoDataValue(&current_band_ptr, 
        bands_data.bands[i].no_data_value.value()).IgnoreError();
      std::cout << "No data value set" << std::endl;
    }
  }

  if (bands_data.wkt_projection.length() > 0) {
    sapi::v::CStr wkt_projection_ptr(bands_data.wkt_projection.data());
    api.GDALSetProjection(&dataset_ptr, 
      wkt_projection_ptr.PtrBefore()).IgnoreError();
  }

  if (bands_data.geo_transform.size() > 0) {
    sapi::v::Array<double> geo_transform_ptr(bands_data.geo_transform.data(), 
      bands_data.geo_transform.size());
    api.GDALSetGeoTransform(&dataset_ptr, 
      geo_transform_ptr.PtrBefore()).IgnoreError();
  }

  api.GDALClose(&dataset_ptr).IgnoreError();

  return 0;
}