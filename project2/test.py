import os
import sys

def compile():
    os.system("make splc")

def generate_SPL_file_path(num: int) -> str:
    return "test/test_2_r{:02d}.spl".format(num)
def generate_cmp_file_name(num: int) -> str:
    return "test/test_2_r{:02d}.out".format(num)
def generate_UDF_SPL_file_path(num: int) -> str:
    return "test/test_2_11712121_{:d}.spl".format(num)
def generate_UDF_cmp_file_name(num: int) -> str:
    return "test/test_2_11712121_{:d}.out".format(num)

def run_specific(num: int):
    spl_path = generate_SPL_file_path(num)
    cmp_path = generate_cmp_file_name(num)
    cmd = "./bin/splc {} > out && diff out {}".format(spl_path, cmp_path)
    print("exe: " + cmd)
    os.system(cmd)

def run_UDF_specific(num: int):
    spl_path = generate_UDF_SPL_file_path(num)
    cmp_path = generate_UDF_cmp_file_name(num)
    cmd = "./bin/splc {} > out && diff out {}".format(spl_path, cmp_path)
    print("exe: " + cmd)
    os.system(cmd)

def run_all():
    TEST_CASE_NUM = 15
    for i in range(1, 1+TEST_CASE_NUM, 1):
        run_specific(i)
    TEST_CASE_NUM_UDF = 10
    for i in range(1, 1+TEST_CASE_NUM_UDF, 1):
        run_UDF_specific(i)

if __name__ == "__main__":
    compile()
    if len(sys.argv) == 1:
        run_all()
        os.remove('out')
    elif len(sys.argv) == 2:
        num = eval(sys.argv[1])
        run_specific(num)