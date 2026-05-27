from header_generator import *
from datetime import datetime, timezone, timedelta

version = "Alpha 12.418.0"
version_number = "Y26V03"
maintainer = "Sudo-su-Bash"

class VersionGenerator(HeaderGenerator):
    def __init__(self, version, version_number, arch, maintainer):
        HeaderGenerator.__init__(self, "version", "version.h")
        self.version = version
        self.version_number = version_number
        self.arch = arch
        self.maintainer = maintainer

    def _get_time(self):
        tz = timezone(timedelta(hours=0))
        now = datetime.now(tz)
        return now.strftime("%a %b %d, %H:%M:%S UTC %Y")


    def generate(self):
        self._ifd_header()
        now = self._get_time()
        self.write_define("KERNEL_VERSION", f'"{self.version}"')
        self.write_define("KERNEL_COMPILE_TIME", f'"{now}"')
        self.write_define("KERNEL_VERSION_NUMBER", f'"{self.version_number}"')
        self.write_define("KERNEL_PLATFORM", f'"{self.arch}"')
        self.write_define("KERNEL_MAINTAINER", f'"{self.maintainer}"')
        self._ifd_end()

def gen_help():
    print("usage: gen_version.py <arch>")

def main():
    arch = sys_param_check()
    if arch  == None:
        gen_help()
        return 0
    obj = VersionGenerator(version, version_number, arch, maintainer)
    obj.generate()
    
if __name__ == "__main__":
    main()