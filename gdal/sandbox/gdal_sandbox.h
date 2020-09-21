#include <string>

#include <syscall.h>

#include "gdal_sapi.sapi.h"
#include "sandboxed_api/sandbox2/util/fileops.h"

class GdalSapiSandbox : public gdalSandbox {
 public:
  GdalSapiSandbox(std::string path) 
    : gdalSandbox(),
      path_(std::move(path)) 
    {}

  std::unique_ptr<sandbox2::Policy> ModifyPolicy(
      sandbox2::PolicyBuilder*) override {

      return sandbox2::PolicyBuilder()
        .AllowDynamicStartup()
        .AllowRead()
        .AllowSystemMalloc()
        .AllowWrite()
        .AllowExit()
        .AllowStat()
        .AllowOpen()
        .AllowSyscalls({
            __NR_futex, __NR_close,
            __NR_recvmsg,  // To work with remote fd
            __NR_getdents64,  // DriverRegisterAll()
            __NR_lseek,  // GDALCreate()
            __NR_getpid,  // GDALCreate()
            __NR_sysinfo,
            __NR_prlimit64,
            __NR_ftruncate,  // GTiffDataset::FillEmptyTiles
            __NR_unlink,  // GDALDriver::Delete
        })
        .AddFile("/usr/local/share/proj/proj.db")  // proj.db is required
        .AddDirectory("/usr/local/lib")  // To add libproj.so.19.1.1
        .AddDirectory(path_, false)
        .BuildOrDie();
    }

 private:
  std::string path_;
};
