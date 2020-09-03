#include <iostream>

#include "gdal_sapi.sapi.h"

int main() {
    gdalSandbox sandbox;
    sandbox.Init().IgnoreError();
    gdalApi api(&sandbox);
    std::string filename = "test";
    sapi::v::CStr s(filename.data());
    auto res = api.GDALOpen(s.PtrBefore(), GDALAccess::GA_ReadOnly);

    return 0;
}