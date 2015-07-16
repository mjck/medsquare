How to write a test under Medsquare.

STEPS:
1.- Write your test file.
    one simple file of test is:
    #!/usr/bin/python
    import sys

    def main():   
	sys.exit(0);
 
    main()
    
    NOTE: if the returned value is 0 the test was successful.

2.- Edit the file CMakeLists.txt to add your new test:
    -Add the name of the file of test in "SET(TESTSPY", if your file of test
    is yourTest.py:
    	 SET(TESTSPY
		...
		yourTest
	 )
    -


HOW TO RUN TESTS
only run with:
make test
