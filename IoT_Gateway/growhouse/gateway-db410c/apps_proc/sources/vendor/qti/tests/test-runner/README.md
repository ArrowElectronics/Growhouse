#test-runner-tests documentation:

each directory is a test case to be run in test-runner

copy lib directory files to test-runner's directory- 
[test-runner-cloned directory]/test-definitions/automated/lib/

copy utils directory files to test-runner's directory- 
[test-runner-cloned directory]/test-definitions/automated/utils/

copy test plans files to lt-ci directory- 
[lt-ci-cloned directory]/plans/

how to run the test cases:
	
        1. setup the test runner see: https://github.com/96boards/documentation/blob/master/ConsumerEdition/DragonBoard-410c/Guides/Testing.md

        2. git clone current repo.

        3. copy each directory to  [test-runner-cloned directory]/test-definitions/automated/linux/

        4. run a test: test-runner -g root@[DB410c IP ADDRESS] -d automated/linux/[test case directory]/[test case dicrory].yaml

        5. run a test plan e.g: test-runner -g root@[DB410c IP ADDRESS] -p ../lt-ci/plans/storage/OE-storage-DUT1.yaml

*** list of tests below requires executables that's not included in the open embedded builds
the user must compile them and copy then to the device under test(DUT) <test-case-directory>/util/
when the test is run they are copied by Test-runner to the DUT and are executed on the DUT.


1. lscpu
	1.1 copy lscpu binary file to util directory
		source files: https://github.com/karelzak/util-linux/blob/master/sys-utils/lscpu.c

2. sysbench-open-embedded
	2.1 copy sysbench binary file to util directory 
		source files: https://github.com/akopytov/sysbench
	2.2 add libmysqlclient.so.18 file to util directory

3. gettime
	3.1 copy gettimeofday02 to util directory
		source file: https://github.com/linux-test-project/ltp/blob/master/testcases/kernel/syscalls/gettimeofday/gettimeofday02.c

4. rtc
	4.1 copy rtc01 to util directory
		source file: https://github.com/linux-test-project/ltp/blob/master/testcases/kernel/device-drivers/rtc/rtc01.c

