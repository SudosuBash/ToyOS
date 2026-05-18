import csv
import os
 
from header_generator import *

class SystableGenerator(HeaderGenerator):
    def __init__(self, arch):
        HeaderGenerator.__init__(self, "syscall_id", "syscall_id.inc", arch)
        self.syscalls = 512
        self.systables = [0] * self.syscalls
        rootdir = os.getcwd()
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
        for i in self.systables:
            if(i!=0):
                self.write(f"extern long {i["cfuncname"]}();\n")
        self.write("\nconst syscall_fn syscall_table[{nr}] = {{\n".format(nr=self.syscalls))
        for i in range(self.syscalls):
            if self.systables[i] == 0:
                txt = "    (void*)empty_syscall,\n"
            else:
                txt = f"    (void*){self.systables[i]["cfuncname"]},\n"
            self.write(txt)
        self.write("};")
        self.close()

def gen_help():
    print("usage: gen_systable.py <arch>")

def main():
    arch = sys_param_check()
    if arch  == None:
        gen_help()
        return 0
    obj = SystableGenerator(arch=arch)
    obj.generate()
    
if __name__ == "__main__":
    main()