









#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <libpq-fe.h>

// Function declarations
void clearScreen();
void printMenu();
void getCurrentDate(char *date);
void detect_os();
void addTransaction(PGconn *conn, int user_id, const char *title, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, const char *purchase_date, int credit_card_id, int is_repeated, int account_id);
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record);
void viewTransactions(PGconn *conn, int user_id);
void viewIncome(PGconn *conn);
void viewCategories(PGconn *conn);
void viewIncomeCategories(PGconn *conn);
void viewPaymentMethods(PGconn *conn);
void viewCreditCards(PGconn *conn);
void addCreditCard(PGconn *conn);
void displayIntroduction();
void registerUser(PGconn *conn);
int  loginUser(PGconn *conn, int *user_id);
void addBankCompany(PGconn *conn, const char *name, const char *location); // Add this
void viewBankCompanies(PGconn *conn); // Add this
void addAccount(PGconn *conn, int user_id, const char *title_account, float balance, int banks_company_id); // Update this
void viewAccounts(PGconn *conn, int user_id); // Update this
void recordLoginActivity(PGconn *conn, int user_id);
void recordLogoutActivity(PGconn *conn, int user_id);
void addCategory(PGconn *conn, int is_income_category);





// Function to clear the screen
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}





// Function to display the introduction screen
void displayIntroduction() {
    clearScreen();
    printf("\n\n");
    printf("   __   _                                  _           _                            _                    _ \n");
    printf("  / _| (_)  _ __     __ _   _ __     ___  (_)   __ _  | |     ___    ___    _ __   | |_   _ __    ___   | |\n");
    printf(" | |_  | | | '_ \\   / _` | | '_ \\   / __| | |  / _` | | |    / __|  / _ \\  | '_ \\  | __| | '__|  / _ \\  | |\n");
    printf(" |  _| | | | | | | | (_| | | | | | | (__  | | | (_| | | |   | (__  | (_) | | | | | | |_  | |    | (_) | | |\n");
    printf(" |_|   |_| |_| |_|  \\__,_| |_| |_|  \\___| |_|  \\__,_| |_|    \\___|  \\___/  |_| |_|  \\__| |_|     \\___/  |_|\n");
    printf("\n\n");
    printf("Financial Control\n");
    printf("Created by Pedro Akira, with C and PostgreSQL\n");
    printf("\n\n");
    printf("Press Enter to continue...");
    getchar(); // Wait for Enter key
}




void printMenu() {
    printf("1. Add Income (e.g., Salary)\n");
    printf("2. Add Expense\n");
    printf("3. View Transactions\n");
    printf("4. View Income\n");
    printf("5. View Transaction Categories\n");
    printf("6. View Income Categories\n");
    printf("7. View Payment Methods\n");
    printf("8. View Credit Cards\n");
    printf("9. Add Credit Card\n");
    printf("10. Add Bank Company\n");
    printf("11. Add Account\n");
    printf("12. View Accounts\n");
    printf("13. View Bank Companies\n");
    printf("14. View User Activity\n");
    printf("15. Add Category\n");  // New unified option
    printf("16. Exit\n");
}








// Function to add a new category (transaction or income)
void addCategory(PGconn *conn, int is_income_category) {
    char category_name[100];
    char table_name[20];
    
    strcpy(table_name, is_income_category ? "categories_income" : "categories");
    
    printf("Enter new %s category name: ", is_income_category ? "income" : "transaction");
    scanf(" %[^\n]", category_name);
    
    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO %s (name) VALUES ('%s')", 
             table_name, category_name);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to add category: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Category added successfully!\n");
}




// Function to get the current date in YYYY-MM-DD format
void getCurrentDate(char *date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}


// Function to detect the operating system
void detect_os() {
    #ifdef _WIN32
        printf("Operating System: Windows\n");
    #else
        printf("Operating System: Linux\n");
    #endif
}


void addTransaction(PGconn *conn, int user_id, const char *title, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, const char *purchase_date, int credit_card_id, int is_repeated, int account_id) {
    char query[512];
    char account_id_str[20]; // Buffer to hold the account_id as a string or "NULL"

    // Convert account_id to a string or "NULL"
    if (account_id == -1) {
        strcpy(account_id_str, "NULL");
    } else {
        snprintf(account_id_str, sizeof(account_id_str), "%d", account_id);
    }

    if (company_name[0] == '\0') {
        // If company name is empty, insert NULL for company_id
        if (payment_method_id == 2) { // Credit Card
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (title, description, amount, type, category_id, payment_method_id, date_record, purchase_date, credit_card_id, is_repeated, account_id, user_id) "
                     "VALUES ('%s', '%s', %.2f, '%s', %d, %d, '%s', '%s', %d, %s, %s, %d)", 
                     title, description, amount, type, category_id, payment_method_id, date_record, purchase_date, credit_card_id, is_repeated ? "TRUE" : "FALSE", account_id_str, user_id);
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (title, description, amount, type, category_id, payment_method_id, date_record, purchase_date, credit_card_id, is_repeated, account_id, user_id) "
                     "VALUES ('%s', '%s', %.2f, '%s', %d, %d, '%s', '%s', NULL, %s, %s, %d)", 
                     title, description, amount, type, category_id, payment_method_id, date_record, purchase_date, is_repeated ? "TRUE" : "FALSE", account_id_str, user_id);
        }
    } else {
        // Insert company if it doesn't exist
        char company_query[256];
        snprintf(company_query, sizeof(company_query), 
                 "INSERT INTO companies (name, location) VALUES ('%s', '%s') ON CONFLICT (name) DO NOTHING", 
                 company_name, company_location);
        PGresult *company_res = PQexec(conn, company_query);
        if (PQresultStatus(company_res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Company insertion failed: %s", PQerrorMessage(conn));
            PQclear(company_res);
            return;
        }
        PQclear(company_res);

        // Insert transaction with company_id
        if (payment_method_id == 2) { // Credit Card
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (title, description, amount, type, category_id, payment_method_id, company_id, date_record, purchase_date, credit_card_id, is_repeated, account_id, user_id) "
                     "VALUES ('%s', '%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', '%s', %d, %s, %s, %d)", 
                     title, description, amount, type, category_id, payment_method_id, company_name, date_record, purchase_date, credit_card_id, is_repeated ? "TRUE" : "FALSE", account_id_str, user_id);
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (title, description, amount, type, category_id, payment_method_id, company_id, date_record, purchase_date, credit_card_id, is_repeated, account_id, user_id) "
                     "VALUES ('%s', '%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', '%s', NULL, %s, %s, %d)", 
                     title, description, amount, type, category_id, payment_method_id, company_name, date_record, purchase_date, is_repeated ? "TRUE" : "FALSE", account_id_str, user_id);
        }
    }

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Transaction failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Transaction added successfully!\n");
}




// Function to add a bank company
void addAccount(PGconn *conn, int user_id, const char *title_account, float balance, int banks_company_id) {
    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO account (title_account, balance, banks_company_id, user_id) "
             "VALUES ('%s', %.2f, %d, %d)", 
             title_account, balance, banks_company_id, user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Account insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Account added successfully!\n");
}









// Function to view bank companies
void viewAccounts(PGconn *conn, int user_id) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.title_account, a.balance, b.name AS bank_name "
             "FROM account a "
             "LEFT JOIN banks_company b ON a.banks_company_id = b.id "
             "WHERE a.user_id = %d "
             "ORDER BY a.id", user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Account Title     | Balance  | Bank Name\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-17s | %-8s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1),
               PQgetvalue(res, i, 2),
               PQgetvalue(res, i, 3));
    }
    PQclear(res);
}








// Function to add income
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record) {
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO income (description, amount, category_income_id, payment_method_id, date_record) "
             "VALUES ('%s', %.2f, %d, %d, '%s')", 
             description, amount, category_income_id, payment_method_id, date_record);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Income insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Income added successfully!\n");
}



void viewTransactions(PGconn *conn, int user_id) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, cc.card_name, t.is_repeated "
             "FROM transactions t "
             "LEFT JOIN categories c ON t.category_id = c.id "
             "LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id "
             "LEFT JOIN companies co ON t.company_id = co.id "
             "LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id "
             "WHERE t.user_id = %d "
             "ORDER BY t.date DESC", user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No transactions found.\n");
    } else {
        printf("ID | Title            | Description       | Amount  | Type    | Category      | Payment Method | Company        | Date Record | Purchase Date | Date       | Credit Card | Repeated\n");
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-15s | %-17s | %-7s | %-7s | %-13s | %-14s | %-14s | %-11s | %-12s | %s | %-11s | %s\n",
                   PQgetvalue(res, i, 0), // ID
                   PQgetvalue(res, i, 1), // Title
                   PQgetvalue(res, i, 2), // Description
                   PQgetvalue(res, i, 3), // Amount
                   PQgetvalue(res, i, 4), // Type
                   PQgetvalue(res, i, 5), // Category
                   PQgetvalue(res, i, 6), // Payment Method
                   PQgetvalue(res, i, 7), // Company
                   PQgetvalue(res, i, 8), // Date Record
                   PQgetvalue(res, i, 9), // Purchase Date
                   PQgetvalue(res, i, 10), // Date
                   PQgetvalue(res, i, 11), // Credit Card
                   PQgetvalue(res, i, 12)); // Repeated
        }
    }
    PQclear(res);
}



// Function to view income
void viewIncome(PGconn *conn) {
    const char *query = "SELECT i.id, i.description, i.amount, ci.name AS category, pm.method AS payment_method, i.date_record, i.date "
                        "FROM income i "
                        "LEFT JOIN categories_income ci ON i.category_income_id = ci.id "
                        "LEFT JOIN payment_methods pm ON i.payment_method_id = pm.id "
                        "ORDER BY i.date DESC";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No income records found.\n");
    } else {
        printf("ID | Description       | Amount  | Category      | Payment Method | Date Record | Date\n");
        printf("------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-7s | %-13s | %-14s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),
                   PQgetvalue(res, i, 1),
                   PQgetvalue(res, i, 2),
                   PQgetvalue(res, i, 3),
                   PQgetvalue(res, i, 4),
                   PQgetvalue(res, i, 5),
                   PQgetvalue(res, i, 6));
        }
    }
    PQclear(res);
}

// Function to view transaction categories
void viewCategories(PGconn *conn) {
    const char *query = "SELECT * FROM categories ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Category Name\n");
    printf("-----------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view income categories
void viewIncomeCategories(PGconn *conn) {
    const char *query = "SELECT * FROM categories_income ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Income Category\n");
    printf("-------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view payment methods
void viewPaymentMethods(PGconn *conn) {
    const char *query = "SELECT * FROM payment_methods ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Payment Method\n");
    printf("------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view credit cards
void viewCreditCards(PGconn *conn) {
    const char *query = "SELECT * FROM credit_cards ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Card Name       | Credit Limit | Closes On | Due Day\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-15s | %-12s | %-9s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1),
               PQgetvalue(res, i, 2),
               PQgetvalue(res, i, 3),
               PQgetvalue(res, i, 4));
    }
    PQclear(res);
}

// Function to add a credit card
void addCreditCard(PGconn *conn) {
    char card_name[100];
    float credit_limit;
    int closes_on_day, due_day;

    printf("Enter card name: ");
    scanf(" %[^\n]", card_name);
    printf("Enter credit limit: ");
    scanf("%f", &credit_limit);
    printf("Enter closes on day (1-31): ");
    scanf("%d", &closes_on_day);
    printf("Enter due day (1-31): ");
    scanf("%d", &due_day);

    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO credit_cards (card_name, credit_limit, closes_on_day, due_day) "
             "VALUES ('%s', %.2f, %d, %d)", 
             card_name, credit_limit, closes_on_day, due_day);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Credit card insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Credit card added successfully!\n");
}






void addBankCompany(PGconn *conn, const char *name, const char *location) {
    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO banks_company (name, location) VALUES ('%s', '%s')", 
             name, location);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Bank company insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Bank company added successfully!\n");
}

void viewBankCompanies(PGconn *conn) {
    const char *query = "SELECT id, name, location FROM banks_company ORDER BY id";
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No bank companies found.\n");
    } else {
        printf("ID | Bank Name         | Location\n");
        printf("---------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %s\n",
                   PQgetvalue(res, i, 0), // ID
                   PQgetvalue(res, i, 1), // Name
                   PQgetvalue(res, i, 2)); // Location
        }
    }
    PQclear(res);
}





// Function to register a new user
void registerUser(PGconn *conn) {
    char name[100], password[100], email[100], profession[100];
    int age;

    printf("Enter your name: ");
    scanf(" %[^\n]", name);
    printf("Enter your password: ");
    scanf(" %[^\n]", password);
    printf("Enter your age: ");
    scanf("%d", &age);
    printf("Enter your profession: ");
    scanf(" %[^\n]", profession);
    printf("Enter your email: ");
    scanf(" %[^\n]", email);

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO users (name, password, age, profession, email) "
             "VALUES ('%s', '%s', %d, '%s', '%s')",
             name, password, age, profession, email);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "User registration failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("User registered successfully!\n");
}

// Function to log in a user
int loginUser(PGconn *conn, int *user_id) {
    char email[100], password[100];

    printf("Enter your email: ");
    scanf(" %[^\n]", email);
    printf("Enter your password: ");
    scanf(" %[^\n]", password);

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT id FROM users WHERE email = '%s' AND password = '%s'",
             email, password);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        fprintf(stderr, "Login failed: Invalid email or password\n");
        PQclear(res);
        return 0; // Login failed
    }

    *user_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    printf("Login successful! User ID: %d\n", *user_id);
    return 1; // Login successful
}





// Function to record login activity
void recordLoginActivity(PGconn *conn, int user_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_activity (activity_type, user_id) VALUES ('login', %d)",
             user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Login activity recording failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Login activity recorded successfully!\n");
}





// Function to record logout activity
void recordLogoutActivity(PGconn *conn, int user_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO user_activity (activity_type, user_id) VALUES ('logout', %d)",
             user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Logout activity recording failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Logout activity recorded successfully!\n");
}









void viewUserActivity(PGconn *conn, int user_id) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, activity_type, activity_time "
             "FROM user_activity "
             "WHERE user_id = %d "
             "ORDER BY activity_time DESC",
             user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Activity Type | Activity Time\n");
    printf("----------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-13s | %s\n",
               PQgetvalue(res, i, 0), // ID
               PQgetvalue(res, i, 1), // Activity Type
               PQgetvalue(res, i, 2)); // Activity Time
    }
    PQclear(res);
}





int main() {
    const char *conninfo = "dbname=finances_Homolog user=postgres password=p0w2i8 hostaddr=127.0.0.1 port=5432";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }


    // Display the introduction screen
    displayIntroduction();    



    // Login or register
    int user_id = -1; // Declare user_id
    while (user_id == -1) {
        printf("1. Register\n");
        printf("2. Login\n");
        printf("Enter your choice: ");
        int auth_choice;
        scanf("%d", &auth_choice);
    
        if (auth_choice == 1) {
            registerUser(conn);
        } else if (auth_choice == 2) {
            if (loginUser(conn, &user_id)) {
                // Record login activity after successful login
                recordLoginActivity(conn, user_id);
                break; // Exit loop on successful login
            }
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }


   


    int choice;
    char description[100];
    float amount;
    int category_id, category_income_id, payment_method_id, credit_card_id, is_repeated;
    char company_name[100], company_location[100];
    char date_record[11]; // YYYY-MM-DD format
    int account_id; // Add this line to declare account_id

    while (1) {
        clearScreen();
        // Detect and display the operating system
        detect_os();         
        printMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Add Income
                printf("Enter description: ");
                scanf(" %[^\n]", description);
                printf("Enter amount: ");
                scanf("%f", &amount);
                viewIncomeCategories(conn);
                printf("Enter income category ID: ");
                scanf("%d", &category_income_id);
                viewPaymentMethods(conn);
                printf("Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card): ");
                scanf("%d", &payment_method_id);
                printf("Enter date record (YYYY-MM-DD): ");
                scanf(" %[^\n]", date_record);
                addIncome(conn, description, amount, category_income_id, payment_method_id, date_record);
                break;
                case 2: // Add Expense
                char title[100];
                char purchase_date[11]; // YYYY-MM-DD format
                printf("Enter title: ");
                scanf(" %[^\n]", title);
                printf("Enter description: ");
                scanf(" %[^\n]", description);
                printf("Enter amount: ");
                scanf("%f", &amount);
                viewCategories(conn);
                printf("Enter category ID: ");
                scanf("%d", &category_id);
                viewPaymentMethods(conn);
                printf("Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card, 4 for Pix): ");
                scanf("%d", &payment_method_id);
                
                if (payment_method_id == 2) { // Credit Card
                    viewCreditCards(conn);
                    printf("Enter credit card ID: ");
                    scanf("%d", &credit_card_id);
                } else {
                    credit_card_id = 0; // Not applicable for non-credit card payments
                }
                
                printf("Is this transaction repeated? (1 for Yes, 0 for No): ");
                scanf("%d", &is_repeated);
                
                // Show existing companies
                const char *company_query = "SELECT id, name, location FROM companies ORDER BY id";
                PGresult *company_res = PQexec(conn, company_query);
                if (PQresultStatus(company_res) != PGRES_TUPLES_OK) {
                    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
                    PQclear(company_res);
                    break;
                }
                
                int company_rows = PQntuples(company_res);
                if (company_rows > 0) {
                    printf("Existing Companies:\n");
                    printf("ID | Company Name         | Location\n");
                    printf("------------------------------------\n");
                    for (int i = 0; i < company_rows; i++) {
                        printf("%s | %-20s | %s\n",
                               PQgetvalue(company_res, i, 0), // ID
                               PQgetvalue(company_res, i, 1), // Name
                               PQgetvalue(company_res, i, 2)); // Location
                    }
                } else {
                    printf("No companies found.\n");
                }
                
                // Ask if the user wants to add a new company
                int add_new_company;
                printf("Do you want to add a new company? (1 for Yes, 0 for No): ");
                scanf("%d", &add_new_company);
                
                if (add_new_company == 1) {
                    // Add a new company
                    printf("Enter company name: ");
                    scanf(" %[^\n]", company_name);
                    printf("Enter company location: ");
                    scanf(" %[^\n]", company_location);
                
                    char new_company_query[256];
                    snprintf(new_company_query, sizeof(new_company_query), 
                             "INSERT INTO companies (name, location) VALUES ('%s', '%s') ON CONFLICT (name) DO NOTHING", 
                             company_name, company_location);
                    PGresult *new_company_res = PQexec(conn, new_company_query);
                    if (PQresultStatus(new_company_res) != PGRES_COMMAND_OK) {
                        fprintf(stderr, "Company insertion failed: %s", PQerrorMessage(conn));
                        PQclear(new_company_res);
                        PQclear(company_res);
                        break;
                    }
                    PQclear(new_company_res);
                    printf("New company added successfully!\n");
                } else {
                    // Select an existing company
                    int company_id;
                    printf("Enter the ID of the company: ");
                    scanf("%d", &company_id);
                
                    // Fetch the selected company's name and location
                    char selected_company_query[256];
                    snprintf(selected_company_query, sizeof(selected_company_query), 
                             "SELECT name, location FROM companies WHERE id = %d", 
                             company_id);
                    PGresult *selected_company_res = PQexec(conn, selected_company_query);
                    if (PQresultStatus(selected_company_res) != PGRES_TUPLES_OK || PQntuples(selected_company_res) == 0) {
                        fprintf(stderr, "Invalid company ID: %s", PQerrorMessage(conn));
                        PQclear(selected_company_res);
                        PQclear(company_res);
                        break;
                    }
                
                    // Copy the company name and location
                    strcpy(company_name, PQgetvalue(selected_company_res, 0, 0));
                    strcpy(company_location, PQgetvalue(selected_company_res, 0, 1));
                    PQclear(selected_company_res);
                }
                
                PQclear(company_res);
                
                printf("Enter date record (YYYY-MM-DD): ***(If Card: Expiry date): ");
                scanf(" %[^\n]", date_record);
                
                printf("Enter purchase date (YYYY-MM-DD): ");
                scanf(" %[^\n]", purchase_date);
                
                // Ask for account ID for all payment methods
                viewAccounts(conn, user_id); // Pass user_id
                printf("Enter account ID (or -1 if not applicable): ");
                scanf("%d", &account_id); // Get account ID from user
                
                addTransaction(conn, user_id, title, description, amount, "expense", category_id, payment_method_id, company_name, company_location, date_record, purchase_date, credit_card_id, is_repeated, account_id);
                break;
            case 3: // View Transactions
                viewTransactions(conn, user_id); // Pass user_id
                break;
                break;
            case 4: // View Income
                viewIncome(conn);
                break;
            case 5: // View Transaction Categories
                viewCategories(conn);
                break;
            case 6: // View Income Categories
                viewIncomeCategories(conn);
                break;
            case 7: // View Payment Methods
                viewPaymentMethods(conn);
                break;
            case 8: // View Credit Cards
                viewCreditCards(conn);
                break;
            case 9: // Add Credit Card
                addCreditCard(conn);
                break;
            case 10: // Add Bank Company
                printf("Enter bank name: ");
                scanf(" %[^\n]", company_name);
                printf("Enter bank location: ");
                scanf(" %[^\n]", company_location);
                addBankCompany(conn, company_name, company_location);
                break;            
            case 11: // Add Account
                printf("Enter account title: ");
                scanf(" %[^\n]", description);
                printf("Enter initial balance: ");
                scanf("%f", &amount);
                viewBankCompanies(conn);
                printf("Enter bank company ID: ");
                scanf("%d", &category_id);
                addAccount(conn, user_id, description, amount, category_id);
                break;
            
            case 12: // View Accounts
                viewAccounts(conn, user_id); // Pass user_id
                break;            
            case 13: // View Bank Companies
                viewBankCompanies(conn);
                break;
            case 14: // View User Activity
                viewUserActivity(conn, user_id);
                break;

            case 15: { // Add Category
                    int category_type;
                    printf("Select category type:\n");
                    printf("1. Transaction Category\n");
                    printf("2. Income Category\n");
                    printf("Enter your choice: ");
                    scanf("%d", &category_type);
                    
                    if (category_type == 1 || category_type == 2) {
                        addCategory(conn, category_type == 2);
                    } else {
                        printf("Invalid category type selection.\n");
                    }
                    break;
            }

            case 16: // Exit
                // Record logout activity before exiting
                recordLogoutActivity(conn, user_id);            
                PQfinish(conn);
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("Press Enter to continue...");
        getchar(); getchar(); // Wait for Enter key
    }

    PQfinish(conn);
    return 0;
}











/*

POSTGRESQL:



            --DB: finances

            -- Categories for transactions
            CREATE TABLE categories (
                id SERIAL PRIMARY KEY,
                name VARCHAR(50) NOT NULL UNIQUE
            );

            -- Categories for income
            CREATE TABLE categories_income (
                id SERIAL PRIMARY KEY,
                name VARCHAR(50) NOT NULL UNIQUE
            );

            -- Payment methods
            CREATE TABLE payment_methods (
                id SERIAL PRIMARY KEY,
                method VARCHAR(50) NOT NULL UNIQUE
            );

            -- Companies
            CREATE TABLE companies (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL UNIQUE,
                location VARCHAR(100)
            );

            -- Transactions table (for expenses)
            CREATE TABLE transactions (
                id SERIAL PRIMARY KEY,
                title VARCHAR(100), -- Run the following SQL command to add the title column to the transactions table:
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                type VARCHAR(10) CHECK (type IN ('income', 'expense')) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_id INT REFERENCES categories(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL,
                company_id INT REFERENCES companies(id) ON DELETE SET NULL
            );

            -- Income table
            CREATE TABLE income (
                id SERIAL PRIMARY KEY,
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_income_id INT REFERENCES categories_income(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL
            );

            -- Insert initial data for transaction categories
            INSERT INTO categories (name) VALUES
            ('Food'),
            ('Bills'),
            ('Car'),
            ('Clothing'),
            ('Family'),
            ('Leisure'),
            ('Payments'),
            ('School'),
            ('Services'),
            ('Health'),
            ('Home'),
            ('Transport'),
            ('Entertainment'),
            ('Others');

            -- Insert initial data for transaction categories
            INSERT INTO categories (name) VALUES            
            ('Education and Development'),  -- New category
            ('Emergencies'),                -- New category
            ('Hobbies and Leisure Activities'),  -- New category
            ('Streaming');                  -- New category;





            -- Insert initial data for income categories
            INSERT INTO categories_income (name) VALUES
            ('Payments'),
            ('Commission'),
            ('Recurring'),
            ('Sales'),
            ('Savings');

            -- Insert initial data for payment methods
            INSERT INTO payment_methods (method) VALUES
            ('Cash'),   
            ('Credit Card'),
            ('Debit Card'),
            ('Pix');  

      

            -- Credit Cards table
            CREATE TABLE credit_cards (
                id SERIAL PRIMARY KEY,
                card_name VARCHAR(100) NOT NULL UNIQUE,
                credit_limit DECIMAL(10, 2) NOT NULL,
                closes_on_day INT CHECK (closes_on_day BETWEEN 1 AND 31) NOT NULL,
                due_day INT CHECK (due_day BETWEEN 1 AND 31) NOT NULL
            );

            -- Modify Transactions table
            ALTER TABLE transactions
            ADD COLUMN credit_card_id INT REFERENCES credit_cards(id) ON DELETE SET NULL,
            ADD COLUMN is_repeated BOOLEAN DEFAULT FALSE;




            -- add new table

            CREATE TABLE banks_company (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL UNIQUE,
                location VARCHAR(100)
            );


            CREATE TABLE account (
                id SERIAL PRIMARY KEY,
                title_account VARCHAR(100) NOT NULL,
                balance DECIMAL(10, 2) NOT NULL,
                banks_company_id INT REFERENCES banks_company(id) ON DELETE SET NULL
            );


            ALTER TABLE transactions
            ADD COLUMN account_id INT REFERENCES account(id) ON DELETE SET NULL;



            INSERT INTO banks_company (name, location) VALUES
            ('Wallet', 'Home'),                 
            ('Banco do Brasil', 'Brazil'),
            ('Bradesco', 'Brazil'),
            ('Itau Unibanco', 'Brazil'),
            ('Caixa Economica Federal', 'Brazil'),
            ('Santander Brasil', 'Brazil'),
            ('Banco da Amazonia', 'Brazil'),
            ('Banrisul', 'Brazil'),
            ('Banco do Nordeste', 'Brazil'),
            ('BTG Pactual', 'Brazil'),
            ('Banco Safra', 'Brazil'),
            ('Banco Votorantim', 'Brazil'),
            ('Banco Pan', 'Brazil'),
            ('Banco Original', 'Brazil'),
            ('Banco Inter', 'Brazil'),
            ('Banco Modal', 'Brazil'),
            ('Mercado Pago', 'Brazil'),
            ('Nubank', 'Brazil'),
            ('XP Investimentos', 'Brazil'),
            ('PicPay', 'Brazil'),
            ('PagSeguro', 'Brazil'),
            ('Stone Pagamentos', 'Brazil'),
            ('Neon', 'Brazil'),
            ('Banco BMG', 'Brazil'),
            ('C6 Bank', 'Brazil'),
            ('Sicredi', 'Brazil');
            
            -- Run the following SQL command to add the title column to the transactions table:
            --ALTER TABLE transactions             ADD COLUMN title VARCHAR(100);




            -- OPTION TO PUT 
            INSERT INTO credit_cards (card_name, credit_limit, closes_on_day, due_day) VALUES  ('Inter Card', 4540.00, 22, 28), ('C6 Card', 4940.11, 29, 5), ('Mercado Pago Card', 4100.00, 29, 4);
            -- INSERT INTO account (title_account, balance, banks_company_id) VALUES ('Mercado Pago Account', 0.00, 17), ('Itau Account', 0.00, 4), ('C6 Bank Account', 0.00, 25), ('Inter', 0.00, 15), ('My Wallet', 0.00, 1);





            -- Users table
            CREATE TABLE users (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                password VARCHAR(100) NOT NULL,
                age INT,
                profession VARCHAR(100),
                email VARCHAR(100) UNIQUE NOT NULL
            );

            -- Add user_id to transactions table
            ALTER TABLE transactions ADD COLUMN user_id INT REFERENCES users(id) ON DELETE CASCADE;

            -- Add user_id to account table
            ALTER TABLE account ADD COLUMN user_id INT REFERENCES users(id) ON DELETE CASCADE;


            CREATE TABLE user_activity (
                id SERIAL PRIMARY KEY,
                activity_type VARCHAR(50) NOT NULL, -- 'login' or 'logout'
                activity_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP 
            );


            ALTER TABLE transactions
            ADD COLUMN purchase_date DATE;





            -- Add user_id column to user_activity
            ALTER TABLE user_activity ADD COLUMN user_id INT;

            -- Add foreign key constraint
            ALTER TABLE user_activity 
            ADD CONSTRAINT fk_user_activity_user 
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE;





            -- psql -d database -U user

            -- psql -d shutdown_logs -U postgres
            -- SELECT * FROM shutdown_events;



            How to access PgAdmin4 in web browser?
            Open your desktop pgAdmin4, then do the following steps:

            Open File > Runtime > View Log
            Scroll untill the bottom then you will find the following Application Server URL: http://127.0.0.1:{PORT_NUMBER}/?key={YOUR_KEY}
            Copy this and open in your browser: https://stackoverflow.com/questions/73339903/how-to-access-pgadmin4-in-web-browser

            http://127.0.0.1:5050/browser/
            127.0.0.1:5050/?key=d0ffdd35-5d4e-4fb9-a1c2-15f44fa6092e







WINDOWS:

    gcc -o finance_control_New finance_control_New.c -I "C:\Program Files\PostgreSQL\<version>\include" -L "C:\Program Files\PostgreSQL\<version>\lib" -lpq

    gcc -o finance_control_New-homo finance_control_New-homo.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq











LINUX:
        gcc -o shutdown_program_Time_POSTGRESQLRemote_moreInfos shutdown_program_Time_POSTGRESQLRemote_moreInfos.c -lpq ./shutdown_program_Time_POSTGRESQLRemote_moreInfos        








LINUX preparation: 

        ### login server:
        ### On Ubuntu ###
        sudo su -
        sudo apt update  --yes
        sudo apt-get update && sudo apt-get upgrade -y
        apt list --upgradable
        sudo apt-get install -y gcc
        sudo apt-get install build-essential
        ## Installing Clang on Linux
        sudo apt install -y clang


        sudo apt install postgresql postgresql-contrib
        sudo service postgresql start
        sudo -u postgres psql -c "SELECT version();"

        sudo apt-get install libpq-dev



*/
































// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <libpq-fe.h>

// Function declarations
void clearScreen();
void printMenu();
void getCurrentDate(char *date);
void detectOS();
void addTransaction(PGconn *conn, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, int credit_card_id, int is_repeated);
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record);
void viewTransactions(PGconn *conn);
void viewIncome(PGconn *conn);
void viewCategories(PGconn *conn);
void viewIncomeCategories(PGconn *conn);
void viewPaymentMethods(PGconn *conn);
void viewCreditCards(PGconn *conn);
void addCreditCard(PGconn *conn);

// Function to clear the screen
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// Function to print the menu
void printMenu() {
    printf("1. Add Income (e.g., Salary)\n");
    printf("2. Add Expense\n");
    printf("3. View Transactions\n");
    printf("4. View Income\n");
    printf("5. View Transaction Categories\n");
    printf("6. View Income Categories\n");
    printf("7. View Payment Methods\n");
    printf("8. View Credit Cards\n");
    printf("9. Add Credit Card\n");
    printf("10. Exit\n");
}

// Function to get the current date in YYYY-MM-DD format
void getCurrentDate(char *date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Function to detect the operating system
void detectOS() {
    #ifdef _WIN32
    printf("Operating System: Windows\n");
    #else
    printf("Operating System: Linux\n");
    #endif
}



void addTransaction(PGconn *conn, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, int credit_card_id, int is_repeated) {
    char query[512];
    if (company_name[0] == '\0') {
        // If company name is empty, insert NULL for company_id
        if (payment_method_id == 2) { // Credit Card
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, '%s', %d, %s)", 
                     description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated ? "TRUE" : "FALSE");
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, '%s', NULL, %s)", 
                     description, amount, type, category_id, payment_method_id, date_record, is_repeated ? "TRUE" : "FALSE");
        }
    } else {
        // Insert company if it doesn't exist
        char company_query[256];
        snprintf(company_query, sizeof(company_query), 
                 "INSERT INTO companies (name, location) VALUES ('%s', '%s') ON CONFLICT (name) DO NOTHING", 
                 company_name, company_location);
        PGresult *company_res = PQexec(conn, company_query);
        if (PQresultStatus(company_res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Company insertion failed: %s", PQerrorMessage(conn));
            PQclear(company_res);
            return;
        }
        PQclear(company_res);

        // Insert transaction with company_id
        if (payment_method_id == 2) { // Credit Card
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, company_id, date_record, credit_card_id, is_repeated) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', %d, %s)", 
                     description, amount, type, category_id, payment_method_id, company_name, date_record, credit_card_id, is_repeated ? "TRUE" : "FALSE");
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, company_id, date_record, credit_card_id, is_repeated) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', NULL, %s)", 
                     description, amount, type, category_id, payment_method_id, company_name, date_record, is_repeated ? "TRUE" : "FALSE");
        }
    }

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Transaction failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Transaction added successfully!\n");
}


// Function to add income
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record) {
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO income (description, amount, category_income_id, payment_method_id, date_record) "
             "VALUES ('%s', %.2f, %d, %d, '%s')", 
             description, amount, category_income_id, payment_method_id, date_record);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Income insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Income added successfully!\n");
}

// Function to view transactions
void viewTransactions(PGconn *conn) {
    const char *query = "SELECT t.id, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, t.date_record, t.date, cc.card_name, t.is_repeated "
                        "FROM transactions t "
                        "LEFT JOIN categories c ON t.category_id = c.id "
                        "LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id "
                        "LEFT JOIN companies co ON t.company_id = co.id "
                        "LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id "
                        "ORDER BY t.date DESC";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No transactions found.\n");
    } else {
        printf("ID | Description       | Amount  | Type    | Category      | Payment Method | Company        | Date Record | Date       | Credit Card | Repeated\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-7s | %-7s | %-13s | %-14s | %-14s | %-11s | %s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),
                   PQgetvalue(res, i, 1),
                   PQgetvalue(res, i, 2),
                   PQgetvalue(res, i, 3),
                   PQgetvalue(res, i, 4),
                   PQgetvalue(res, i, 5),
                   PQgetvalue(res, i, 6),
                   PQgetvalue(res, i, 7),
                   PQgetvalue(res, i, 8),
                   PQgetvalue(res, i, 9),
                   PQgetvalue(res, i, 10));
        }
    }
    PQclear(res);
}

// Function to view income
void viewIncome(PGconn *conn) {
    const char *query = "SELECT i.id, i.description, i.amount, ci.name AS category, pm.method AS payment_method, i.date_record, i.date "
                        "FROM income i "
                        "LEFT JOIN categories_income ci ON i.category_income_id = ci.id "
                        "LEFT JOIN payment_methods pm ON i.payment_method_id = pm.id "
                        "ORDER BY i.date DESC";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No income records found.\n");
    } else {
        printf("ID | Description       | Amount  | Category      | Payment Method | Date Record | Date\n");
        printf("------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-7s | %-13s | %-14s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),
                   PQgetvalue(res, i, 1),
                   PQgetvalue(res, i, 2),
                   PQgetvalue(res, i, 3),
                   PQgetvalue(res, i, 4),
                   PQgetvalue(res, i, 5),
                   PQgetvalue(res, i, 6));
        }
    }
    PQclear(res);
}

// Function to view transaction categories
void viewCategories(PGconn *conn) {
    const char *query = "SELECT * FROM categories ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Category Name\n");
    printf("-----------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view income categories
void viewIncomeCategories(PGconn *conn) {
    const char *query = "SELECT * FROM categories_income ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Income Category\n");
    printf("-------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view payment methods
void viewPaymentMethods(PGconn *conn) {
    const char *query = "SELECT * FROM payment_methods ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Payment Method\n");
    printf("------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}

// Function to view credit cards
void viewCreditCards(PGconn *conn) {
    const char *query = "SELECT * FROM credit_cards ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Card Name       | Credit Limit | Closes On | Due Day\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-15s | %-12s | %-9s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1),
               PQgetvalue(res, i, 2),
               PQgetvalue(res, i, 3),
               PQgetvalue(res, i, 4));
    }
    PQclear(res);
}

// Function to add a credit card
void addCreditCard(PGconn *conn) {
    char card_name[100];
    float credit_limit;
    int closes_on_day, due_day;

    printf("Enter card name: ");
    scanf(" %[^\n]", card_name);
    printf("Enter credit limit: ");
    scanf("%f", &credit_limit);
    printf("Enter closes on day (1-31): ");
    scanf("%d", &closes_on_day);
    printf("Enter due day (1-31): ");
    scanf("%d", &due_day);

    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO credit_cards (card_name, credit_limit, closes_on_day, due_day) "
             "VALUES ('%s', %.2f, %d, %d)", 
             card_name, credit_limit, closes_on_day, due_day);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Credit card insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Credit card added successfully!\n");
}

int main() {
    const char *conninfo = "dbname=finances user=postgres password=p0w2i8 hostaddr=127.0.0.1 port=5432";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Detect and display the operating system
    detectOS();

    int choice;
    char description[100];
    float amount;
    int category_id, category_income_id, payment_method_id, credit_card_id, is_repeated;
    char company_name[100], company_location[100];
    char date_record[11]; // YYYY-MM-DD format

    while (1) {
        clearScreen();
        printMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Add Income
                printf("Enter description: ");
                scanf(" %[^\n]", description);
                printf("Enter amount: ");
                scanf("%f", &amount);
                viewIncomeCategories(conn);
                printf("Enter income category ID: ");
                scanf("%d", &category_income_id);
                viewPaymentMethods(conn);
                printf("Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card): ");
                scanf("%d", &payment_method_id);
                printf("Enter date record (YYYY-MM-DD): ");
                scanf(" %[^\n]", date_record);
                addIncome(conn, description, amount, category_income_id, payment_method_id, date_record);
                break;
            case 2: // Add Expense
                printf("Enter description: ");
                scanf(" %[^\n]", description);
                printf("Enter amount: ");
                scanf("%f", &amount);
                viewCategories(conn);
                printf("Enter category ID: ");
                scanf("%d", &category_id);
                viewPaymentMethods(conn);
                printf("Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card): ");
                scanf("%d", &payment_method_id);
                if (payment_method_id == 2) { // Credit Card
                    viewCreditCards(conn);
                    printf("Enter credit card ID: ");
                    scanf("%d", &credit_card_id);
                } else {
                    credit_card_id = 0; // Not applicable for non-credit card payments
                }
                printf("Is this transaction repeated? (1 for Yes, 0 for No): ");
                scanf("%d", &is_repeated);
                printf("Enter company name (or leave blank): ");
                scanf(" %[^\n]", company_name);
                if (company_name[0] != '\0') {
                    printf("Enter company location (or leave blank): ");
                    scanf(" %[^\n]", company_location);
                } else {
                    company_location[0] = '\0';
                }
                printf("Enter date record (YYYY-MM-DD): ");
                scanf(" %[^\n]", date_record);
                addTransaction(conn, description, amount, "expense", category_id, payment_method_id, company_name, company_location, date_record, credit_card_id, is_repeated);
                break;
            case 3: // View Transactions
                viewTransactions(conn);
                break;
            case 4: // View Income
                viewIncome(conn);
                break;
            case 5: // View Transaction Categories
                viewCategories(conn);
                break;
            case 6: // View Income Categories
                viewIncomeCategories(conn);
                break;
            case 7: // View Payment Methods
                viewPaymentMethods(conn);
                break;
            case 8: // View Credit Cards
                viewCreditCards(conn);
                break;
            case 9: // Add Credit Card
                addCreditCard(conn);
                break;
            case 10: // Exit
                PQfinish(conn);
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("Press Enter to continue...");
        getchar(); getchar(); // Wait for Enter key
    }

    PQfinish(conn);
    return 0;
}

*/









/*

POSTGRESQL:



            --DB: finances

            -- Categories for transactions
            CREATE TABLE categories (
                id SERIAL PRIMARY KEY,
                name VARCHAR(50) NOT NULL UNIQUE
            );

            -- Categories for income
            CREATE TABLE categories_income (
                id SERIAL PRIMARY KEY,
                name VARCHAR(50) NOT NULL UNIQUE
            );

            -- Payment methods
            CREATE TABLE payment_methods (
                id SERIAL PRIMARY KEY,
                method VARCHAR(50) NOT NULL UNIQUE
            );

            -- Companies
            CREATE TABLE companies (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL UNIQUE,
                location VARCHAR(100)
            );

            -- Transactions table (for expenses)
            CREATE TABLE transactions (
                id SERIAL PRIMARY KEY,
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                type VARCHAR(10) CHECK (type IN ('income', 'expense')) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_id INT REFERENCES categories(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL,
                company_id INT REFERENCES companies(id) ON DELETE SET NULL
            );

            -- Income table
            CREATE TABLE income (
                id SERIAL PRIMARY KEY,
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_income_id INT REFERENCES categories_income(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL
            );

            -- Insert initial data for transaction categories
            INSERT INTO categories (name) VALUES
            ('Food'),
            ('Bills'),
            ('Car'),
            ('Clothing'),
            ('Family'),
            ('Leisure'),
            ('Payments'),
            ('School'),
            ('Services'),
            ('Health'),
            ('Home'),
            ('Transport'),
            ('Entertainment'),
            ('Others');

            -- Insert initial data for income categories
            INSERT INTO categories_income (name) VALUES
            ('Payments'),
            ('Commission'),
            ('Recurring'),
            ('Sales'),
            ('Savings');

            -- Insert initial data for payment methods
            INSERT INTO payment_methods (method) VALUES
            ('Cash'),
            ('Pix')            
            ('Credit Card'),
            ('Debit Card');



            -- Credit Cards table
            CREATE TABLE credit_cards (
                id SERIAL PRIMARY KEY,
                card_name VARCHAR(100) NOT NULL UNIQUE,
                credit_limit DECIMAL(10, 2) NOT NULL,
                closes_on_day INT CHECK (closes_on_day BETWEEN 1 AND 31) NOT NULL,
                due_day INT CHECK (due_day BETWEEN 1 AND 31) NOT NULL
            );

            -- Modify Transactions table
            ALTER TABLE transactions
            ADD COLUMN credit_card_id INT REFERENCES credit_cards(id) ON DELETE SET NULL,
            ADD COLUMN is_repeated BOOLEAN DEFAULT FALSE;


            -- Run the following SQL command to add the title column to the transactions table:
            ALTER TABLE transactions
            ADD COLUMN title VARCHAR(100);


            -- Users table
            CREATE TABLE users (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                password VARCHAR(100) NOT NULL,
                age INT,
                profession VARCHAR(100),
                email VARCHAR(100) UNIQUE NOT NULL
            );

            -- Add user_id to transactions table
            ALTER TABLE transactions ADD COLUMN user_id INT REFERENCES users(id) ON DELETE CASCADE;

            -- Add user_id to account table
            ALTER TABLE account ADD COLUMN user_id INT REFERENCES users(id) ON DELETE CASCADE;            


            CREATE TABLE user_activity (
                id SERIAL PRIMARY KEY,
                activity_type VARCHAR(50) NOT NULL, -- 'login' or 'logout'
                activity_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP -- Time when the activity occurred
            );



            -- psql -d database -U user

            -- psql -d shutdown_logs -U postgres
            -- SELECT * FROM shutdown_events;










WINDOWS:

    gcc -o finance_control_New finance_control_New.c -I "C:\Program Files\PostgreSQL\<version>\include" -L "C:\Program Files\PostgreSQL\<version>\lib" -lpq

    gcc -o finance_control_New finance_control_New.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq











LINUX:
        gcc -o shutdown_program_Time_POSTGRESQLRemote_moreInfos shutdown_program_Time_POSTGRESQLRemote_moreInfos.c -lpq ./shutdown_program_Time_POSTGRESQLRemote_moreInfos        








LINUX preparation: 

        ### login server:
        ### On Ubuntu ###
        sudo su -
        sudo apt update  --yes
        sudo apt-get update && sudo apt-get upgrade -y
        apt list --upgradable
        sudo apt-get install -y gcc
        sudo apt-get install build-essential
        ## Installing Clang on Linux
        sudo apt install -y clang


        sudo apt install postgresql postgresql-contrib
        sudo service postgresql start
        sudo -u postgres psql -c "SELECT version();"

        sudo apt-get install libpq-dev



*/











