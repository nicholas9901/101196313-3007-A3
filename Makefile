EXE = db_app.exe

# PostgreSQL directory:
POSTGRESSQL_DIR = C:/"Program Files"/PostgreSQL/16

POSTGRESSQL_LIB = ${POSTGRESSQL_DIR}/lib
POSTGRESSQL_INCLUDE = ${POSTGRESSQL_DIR}/include

all:
	gcc -g -c -I${POSTGRESSQL_INCLUDE} db_app_main.c db_app_utils.c
	gcc -g -o ${EXE} db_app_main.o db_app_utils.o -L${POSTGRESSQL_LIB} -lpq

clean:
	del ${EXE}
	del db_app_main.o
	del db_app_utils.o