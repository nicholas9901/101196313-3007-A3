#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <libpq-fe.h>

// Safely exits the conneciton
void exit_connection(PGconn* connection);

// PQconnectdb() with additional error checking
void connect(PGconn** connection, const char* credentials);

// Helper function for gathering user input
void prompt_credentials(    
    const char* prefix,
    char* credential,
    char* buffer,
    char* credentials
    );

// Prints tuples returned from PGresult*
// From https://www.postgresql.org/docs/current/libpq-example.html
void printTuples(PGconn** connection, PGresult** result);

// Prints all the entries in the students table
void getAllStudents(PGconn** connection, PGresult** result);


// Adds student according to parameters
void addStudent(
    PGconn** connection, 
    PGresult** result,
    const char* first_name,
    const char* last_name,
    const char* email,
    const char* enrollment_date
    );

// Updates the email address for a student with the specified student_id
void updateStudentEmail(
    PGconn** connection, 
    PGresult** result,
    const char* student_id,
    const char* new_email
    );

// Deletes the record of the student with the specified student_id
void deleteStudent(
    PGconn** connection, 
    PGresult** result,
    const char* student_id
    );