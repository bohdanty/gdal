#ifndef GET_RASTER_DATA_H_
#define GET_RASTER_DATA_H_

#include <vector>
#include <string>
#include <optional>

struct RasterBandData {
  int width;
  int height;
  std::vector<int> data;
  std::optional<double> no_data_value;
};

struct RasterDataset {
  int width;
  int height;
  std::vector<RasterBandData> bands;
  std::string wkt_projection;  // OpenGIS WKT format
  std::vector<double> geo_transform;
};

RasterDataset GetRasterBandsFromFile(const std::string& filename);

#endif
