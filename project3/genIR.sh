#!/bin/bash
make splc

./bin/splc sample/test01.spl > sample/test01.out
./bin/splc sample/test02.spl > sample/test02.out
./bin/splc sample/test03.spl > sample/test03.out
./bin/splc sample/test04.spl > sample/test04.out

./bin/splc test/test_3_r01.spl > test/test_3_r01.out
./bin/splc test/test_3_r02.spl > test/test_3_r02.out
./bin/splc test/test_3_r03.spl > test/test_3_r03.out
./bin/splc test/test_3_r04.spl > test/test_3_r04.out
./bin/splc test/test_3_r05.spl > test/test_3_r05.out
./bin/splc test/test_3_r06.spl > test/test_3_r06.out
./bin/splc test/test_3_r07.spl > test/test_3_r07.out
./bin/splc test/test_3_r08.spl > test/test_3_r08.out
./bin/splc test/test_3_r09.spl > test/test_3_r09.out
./bin/splc test/test_3_r10.spl > test/test_3_r10.out