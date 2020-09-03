#include <iostream>

#include "gdal.h"
#include "cpl_conv.h"

int main() {
    GDALDatasetH hDataset;
    GDALAllRegister();
    const char* pszFilename = "";
    hDataset = GDALOpen(pszFilename, GA_ReadOnly);

    if (hDataset == NULL) {
        std::cout << "File not found" << std::endl;
    }

    return 0;
}