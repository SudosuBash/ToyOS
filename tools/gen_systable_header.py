import csv
import sys
import os
 
from header_generator import HeaderGenerator

syscalls = 512
rootdir = os.getcwd()

class SystableHeaderGenerator(HeaderGenerator):
    def __init__(self, arch):
        HeaderGenerator.__init__(self, "syscall_id", "syscall_id.h", arch)
        self.syscalls = 512
        self.systables = [0] * syscalls
        table_path = "{rootdir}/arch/{arch}/systable.csv".format(rootdir=rootdir, arch = arch)

        with open(table_path, 'r' ,encoding = "utf-8") as f:
            r = csv.reader(f)
            next(r)
            for row in r:   
                cid = int(row[0])
                cname = str(row[1])
                cfuncname = str(row[2])
                self.systables[cid] = {
                    "cname": cname,
                    "cfuncname": "syscall_"+cfuncname.strip(" ")
                }

    def generate(self):
        self._ifd_header()
        for i in range(len(self.systables)):
            if self.systables[i] != 0:
                self.write_define(f"_sys_{i}",self.systables[i]["cname"])
        self._ifd_end()

def gen_help():
    print("usage: gen_systable_header.py <arch>")
    
def sys_param_check():
    args = sys.argv
    if len(args) != 2:
        help()
        return None
    arch = args[1]
    return arch

def main():
    arch = sys_param_check()
    if arch  == None:
        gen_help()
        return 0
    obj = SystableHeaderGenerator(arch=arch)
    obj.generate()
    
if __name__ == "__main__":
    main()