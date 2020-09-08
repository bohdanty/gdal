#include <iostream>

#include "gdal_sapi.sapi.h"

#include "sandboxed_api/sandbox2/util/fileops.h"

class GdalSapiSandbox : public gdalSandbox {
  public:
  std::unique_ptr<sandbox2::Policy> ModifyPolicy(
      sandbox2::PolicyBuilder*) override {

      return sandbox2::PolicyBuilder()
        .DangerDefaultAllowAll()
        .DisableNamespaces()
        .BuildOrDie();
    }
};

int main() {
    GdalSapiSandbox sandbox;
    sandbox.Init().IgnoreError();
    gdalApi api(&sandbox);
    api.GDALAllRegister().IgnoreError();
    std::string filename = "/home/asdf/Desktop/CANYrelief1-geo.tif";
    sapi::v::CStr s(filename.data());

    auto res = api.GDALOpen(s.PtrBefore(), GDALAccess::GA_ReadOnly);
    sapi::v::RemotePtr hDataset(res.value());
    std::cout << res.value() << std::endl;
    res = api.GDALGetDatasetDriver(&hDataset);
    sapi::v::RemotePtr hDriver(res.value());
    std::cout << res.value() << std::endl;

    auto s_name_address = api.GDALGetDriverShortName(&hDriver);
    auto l_name_address = api.GDALGetDriverLongName(&hDriver);

    sapi::v::RemotePtr s_name_ptr(s_name_address.value());
    sapi::v::RemotePtr l_name_ptr(l_name_address.value());

    std::string driver_short_name = sandbox.GetCString(s_name_ptr).value();
    std::string driver_long_name = sandbox.GetCString(l_name_ptr).value();

    printf( "Driver: %s/%s\n",
            driver_short_name.c_str(),
            driver_long_name.c_str());

    sapi::v::RemotePtr null(nullptr);
    auto gdal_info_ptr = api.GDALInfo(&hDataset, &null);

    sapi::v::RemotePtr info_str_ptr(gdal_info_ptr.value());

    std::string gdal_info = sandbox.GetCString(info_str_ptr).value();
    std::cout << gdal_info << std::endl;

    return 0;
}