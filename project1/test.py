import os
import sys

def compile():
    os.system("make splc")

def generate_SPL_file_path(num: int) -> str:
    return "test/test_1_r{:02d}.spl".format(num)
def generate_cmp_file_name(num: int) -> str:
    return "test/test_1_r{:02d}.out".format(num)

def run_specific(num: int):
    spl_path = generate_SPL_file_path(num)
    cmp_path = generate_cmp_file_name(num)
    cmd = "./bin/splc {} > out && diff out {}".format(spl_path, cmp_path)
    print("exe: " + cmd)
    os.system(cmd)

def run_all():
    for i in range(1, 13, 1):
        run_specific(i)

if __name__ == "__main__":
    compile()
    if len(sys.argv) == 1:
        run_all()
    elif len(sys.argv) == 2:
        num = eval(sys.argv[1])
        run_specific(num)
    
