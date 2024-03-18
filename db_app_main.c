#include "defs.h"

int main() 
{
    char response;

    PGconn* connection; // For establishing the connection to the database
    PGresult* result;   // Query results are stored here

    // Default credentials
    char dbname[BUFSIZ]    = "postgres";
    char username[BUFSIZ]  = "postgres";
    char password[BUFSIZ]  = "postgres";

    // Additional strings to be stored later
    char buffer[BUFSIZ] = "";
    char credentials[BUFSIZ] = "";
    char login[BUFSIZ] = "";

    while (1) {
        // Prompt the user's credentials to connect to the database dbname
        printf("Enter the name of the database (default: %s): ", dbname);
        prompt_credentials("dbname=", dbname, buffer, credentials);

        printf("Enter the username (default: %s): ", username);
        prompt_credentials(" user=", username, buffer, credentials);

        printf("Enter the password (default: %s): ", password);
        prompt_credentials(" password=", password, buffer, credentials);

        printf("Connecting to database with info: \"%s\"\n", credentials);

        // Establish a connection to the database according to the previous
        // input
        connect(&connection, credentials);

        // Store the login details
        snprintf(
            login, 
            sizeof login, 
            " user=%s password=%s", 
            username, 
            password
            );

        // Error checker: Is the connection valid?
        if (PQstatus(connection) != CONNECTION_OK) { 
            if (strstr(PQerrorMessage(connection), "does not exist")) {
                printf("Database does not exist yet. Retrying...\n");
            }
            else exit_connection(connection);
        }
        else break;
    }

    while (response != 'y' && response != 'n') {
        // Prompt the user if they want to continue in the current database
        printf("Make a new database? If not, populate this one [y/n]: ");
        response = fgetc(stdin);
        fflush(stdin);
    }

    if (response == 'y') {
        int result_completed = 0;
        char sql[BUFSIZ] = "";
        char dbname_current[BUFSIZ] = "";
        
        strcpy(dbname_current, dbname);

        while (!result_completed) {
            // Create a new database according to the inputted name
            memset(credentials, 0, sizeof credentials);
            strcpy(sql, "CREATE DATABASE ");
            printf(
                "Enter the name of the database (current: %s): ", 
                dbname_current
                );
            prompt_credentials("dbname=", dbname, buffer, credentials);
            strcat(sql, buffer);

            result = PQexec(connection, sql); // Query execute

            // Error checker: Command executed successfully?
            if (PQresultStatus(result) != PGRES_COMMAND_OK) {
                fprintf(stderr, "%s", PQerrorMessage(connection));
            }
            else {
                result_completed = 1;
                strcat(credentials, login);
                PQclear(result); // Free result pointer
            }
        }

        // Safely exit the current connection
        PQfinish(connection);

        printf("Database creation successful.\n");
        printf("Connecting to database with info: \"%s\"\n", credentials);

        // Connect to the new database with the previous credentials
        connect(&connection, credentials);
    }

    printf("Populating database...\n");

    FILE *db_init;
    int file_size;
    char* sql;

    // Read from init.sql to initialize the data
    db_init = fopen("init.sql", "rb");
    if (db_init == NULL) {
        printf("Error: Failed to open init.sql\n");
        exit_connection(connection);
    }

    // Get the amount of bytes needed to store the file contents
    fseek(db_init, 0, SEEK_END);
    file_size = ftell(db_init);
    sql = malloc(file_size + 1);

    // Reset the pointer, read from the file and store the contents in
    // the buffer
    rewind(db_init);
    fread(sql, 1, file_size, db_init);
    fclose(db_init);
    sql[file_size] = 0;

    result = PQexec(connection, sql); // Query execute

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "%s", PQerrorMessage(connection));

        // If the relation was already initialized, skip this step
        if (strstr(PQerrorMessage(connection), "already exists")) {
            printf("Database already initialized. Continuing...\n");
        }

        // If some other error occured, exit the program
        else {
            printf("Unexpected error occured. Exiting...\n");
            exit_connection(connection);
        }
    }

    PQclear(result); // Free result pointer

    char choice = '1';

    // Parameterized strings for the application functions
    char student_id[BUFSIZ]      = "\n";
    char first_name[BUFSIZ]      = "\n";
    char last_name[BUFSIZ]       = "\n";     
    char email[BUFSIZ]           = "\n";
    char enrollment_date[BUFSIZ] = "\n";

    // Main control loop
    while (choice != '0') {
        printf(
            "\nEnter one of the following options:\n\n"
            "0 - Exit program\n"
            "1 - Get All Students\n"
            "2 - Add Student\n"
            "3 - Update Student\n"
            "4 - Delete Student\n"
            );
            choice = fgetc(stdin);
            fflush(stdin);
            switch (choice) {
                case '1': 
                    getAllStudents(&connection, &result);
                    break;
                case '2': 
                    // Prompt the user for first_name
                    while (first_name[0] == '\n'){
                        printf("Enter the student's first name: ");
                        fgets(first_name, BUFSIZ, stdin);
                    }
                    first_name[strcspn(first_name, "\n")] = 0;

                    // Prompt the user for last_name
                    while (last_name[0] == '\n'){
                        printf("Enter the student's last name: ");
                        fgets(last_name, BUFSIZ, stdin);
                    }
                    last_name[strcspn(last_name, "\n")] = 0;

                    // Prompt the user for email
                    while (email[0] == '\n'){
                        printf("Enter the student's email: ");
                        fgets(email, BUFSIZ, stdin);
                    }
                    email[strcspn(email, "\n")] = 0;

                    // Prompt the user for enrollment_date
                    while (enrollment_date[0] == '\n'){
                        printf("Enter the student's enrollment date: ");
                        fgets(enrollment_date, BUFSIZ, stdin);
                    }
                    enrollment_date[strcspn(enrollment_date, "\n")] = 0;

                    addStudent(
                        &connection, 
                        &result,
                        first_name,
                        last_name,
                        email,
                        enrollment_date
                        );

                    // Reset the first character so the prompt loop still
                    // functions accordingly
                    first_name[0]      = '\n';
                    last_name[0]       = '\n';
                    email[0]           = '\n';
                    enrollment_date[0] = '\n';
                    break;
                case '3': 
                    // Prompt the user for student_id
                    while (student_id[0] == '\n'){
                        printf("Enter the student's ID: ");
                        fgets(student_id, BUFSIZ, stdin);
                    }
                    student_id[strcspn(student_id, "\n")] = 0;
                
                    // Prompt the user for email
                    while (email[0] == '\n'){
                        printf("Enter the student's new email: ");
                        fgets(email, BUFSIZ, stdin);
                    }
                    email[strcspn(email, "\n")] = 0;

                    updateStudentEmail(
                        &connection,
                        &result,
                        student_id,
                        email
                    );

                    // Reset the first character so the prompt loop still
                    // functions accordingly
                    student_id[0] = '\n';
                    email[0]      = '\n';
                    break;
                case '4':
                    // Prompt the user for student_id
                    while (student_id[0] == '\n'){
                        printf("Enter the student's ID: ");
                        fgets(student_id, BUFSIZ, stdin);
                    }
                    student_id[strcspn(student_id, "\n")] = 0;

                    deleteStudent(
                        &connection,
                        &result, 
                        student_id
                    );

                    // Reset the first character so the prompt loop still
                    // functions accordingly
                    student_id[0] = '\n';
                    break;
                default: continue; 
            }
    }

    // End of the program: free memory accordingly
    PQfinish(connection);
    free(sql);
    return 1;
}
