Nicholas Pneumaticos 101196313
COMP 3005 Assignment 3 Question 1

An application written in C which interfaces with a PostgreSQL database.
This application works on Windows only.

For this to run and compile, you will need MinGW and GNU Make for Windows:

    MinGW:
        https://www.mingw-w64.org/

    Make can be installed manually or via Chocolatey:
        https://gnuwin32.sourceforge.net/packages/make.htm
        https://community.chocolatey.org/packages/make#install

Also, make sure the path of PostgreSQL is correct in the Makefile. 

By default, this is:
    C:/"Program Files"/PostgreSQL/16

If this is not the correct path, make sure to change the POSTGRESSQL_DIR
variable in the Makefile.