#include "defs.h"

void exit_connection(PGconn* connection)
{
    PQfinish(connection);
    exit(1);
}

void connect(PGconn** connection, const char* credentials)
{
    *connection = PQconnectdb(credentials);

    // Error checker: Valid connection?
    // Exit the program otherwise
    if (PQstatus(*connection) != CONNECTION_OK) {
        fprintf(stderr, "%s", PQerrorMessage(*connection));
        printf("Error: Could not connect. Exiting...");
        exit_connection(*connection);
    }
    else printf("Connection successful.\n");    
}

void prompt_credentials(
    const char* prefix,
    char* credential,
    char* buffer,
    char* credentials
    ) 
{
    // If the user inputs the newline, use the default parameter (credential)
    fgets(buffer, BUFSIZ, stdin);
    if (buffer[0] == '\n') strcat(strcat(credentials, prefix), credential);

    // Otherwise, use the prefix parameter
    else {
        buffer[strcspn(buffer, "\n")] = 0;
        strcat(strcat(credentials, prefix), buffer);
        strcpy(credential, buffer);
    }
}

void printTuples(PGconn** connection, PGresult** result) 
{
    printf("\n");

    // Loop through and print the field names
    for (int i = 0; i < PQnfields(*result); i++) {
        printf("%-25s", PQfname(*result, i));
    }
    printf("\n\n");

    // Loop through and print the tuples
    for (int i = 0; i < PQntuples(*result); i++) {
        for (int j = 0; j < PQnfields(*result); j++) {
            printf("%-25s", PQgetvalue(*result, i, j));
        }
        printf("\n\n");
    }
}

void getAllStudents(PGconn** connection, PGresult** result) 
{
    // Query execute
    *result = PQexec(*connection, /* Query */ "TABLE students"); 

    // Error checker: Were the tuples returned proeprly?
    if (PQresultStatus(*result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(*connection));
        PQclear(*result);
        return;
    }

    // Call printTuples() with the result 
    printTuples(connection, result);
    PQclear(*result); // Free *result
}

void addStudent(
    PGconn** connection, 
    PGresult** result,
    const char* first_name,
    const char* last_name,
    const char* email,
    const char* enrollment_date
    )
{
    char sql[BUFSIZ] = "";

    // Format the parameters into the sql statement accordingly
    snprintf(
        sql, 
        sizeof sql,
        // Insert the new student according to the parameters
        "INSERT INTO students (first_name, last_name, email, enrollment_date)"
        "VALUES ('%s', '%s', '%s', '%s')",
        first_name,
        last_name,
        email,
        enrollment_date
        );

    *result = PQexec(*connection, sql); // Query execute

    // Error checker: Command executed properly?
    if (PQresultStatus(*result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(*connection));
        PQclear(*result);
        return;
    }

    PQclear(*result); // Free *result
    printf("\n");

    *result = PQexec(*connection, // Query execute
        // Show that the student was added successfuly using lastval()
        "SELECT * FROM students WHERE student_id=lastval()"
        );

    // Print the query
    printTuples(connection, result);
    PQclear(*result); // Free *result
}

void updateStudentEmail(
    PGconn** connection, 
    PGresult** result,
    const char* student_id,
    const char* new_email
    )
{
    char sql[BUFSIZ] = "";

    // Format the parameters into the sql statement accordingly
    snprintf(
        sql, 
        sizeof sql, 
        // Update the student with the correct ID with the new email 
        "UPDATE students SET email ='%s' WHERE student_id=%s",
        new_email,
        student_id
        );

    *result = PQexec(*connection, sql); // Query execute

    // Error checker: Command executed properly?
    if (PQresultStatus(*result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(*connection));
        PQclear(*result); // Free *result
        return;
    }

    PQclear(*result); // Free *result
    snprintf(
        sql,
        sizeof sql,
        // Show that the student has been updated accordingly
        "SELECT * FROM students WHERE student_id=%s",
        student_id
    );

    *result = PQexec(*connection, sql); // Query execute

    // Print the query
    printTuples(connection, result);
    PQclear(*result); // Free *result
}

void deleteStudent(
    PGconn** connection, 
    PGresult** result,
    const char* student_id
    )
{
    char sql[BUFSIZ] = "";

    // Format the parameters into the sql statement accordingly
    snprintf(
        sql, 
        sizeof sql,
        // Delete the student with the correct ID
        "DELETE FROM students WHERE student_id=%s",
        student_id
        );
    
    *result = PQexec(*connection, sql); // Query execute

    // Error checker: Command executed properly?
    if (PQresultStatus(*result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s\n", PQerrorMessage(*connection));
        PQclear(*result); // Free *result
        return;
    }

    PQclear(*result); // Free *result

    // Show that the deleted value is no longer present
    getAllStudents(connection, result); 
}
