













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
void addTransaction(PGconn *conn, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, int credit_card_id, int is_repeated, int account_id);
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
    printf("10. Add Bank Company\n");
    printf("11. Add Account\n");
    printf("12. View Accounts\n");
    printf("13. View Bank Companies\n"); // New option
    printf("14. Exit\n");
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









void addTransaction(PGconn *conn, const char *description, float amount, const char *type, int category_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, int credit_card_id, int is_repeated, int account_id) {
    char query[512];
    if (company_name[0] == '\0') {
        // If company name is empty, insert NULL for company_id
        if (payment_method_id == 2) { // Credit Card
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated, account_id) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, '%s', %d, %s, %d)", 
                     description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated ? "TRUE" : "FALSE", account_id);
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, date_record, credit_card_id, is_repeated, account_id) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, '%s', NULL, %s, %d)", 
                     description, amount, type, category_id, payment_method_id, date_record, is_repeated ? "TRUE" : "FALSE", account_id);
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
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, company_id, date_record, credit_card_id, is_repeated, account_id) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', %d, %s, %d)", 
                     description, amount, type, category_id, payment_method_id, company_name, date_record, credit_card_id, is_repeated ? "TRUE" : "FALSE", account_id);
        } else {
            // For non-credit card payments, set credit_card_id to NULL
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (description, amount, type, category_id, payment_method_id, company_id, date_record, credit_card_id, is_repeated, account_id) "
                     "VALUES ('%s', %.2f, '%s', %d, %d, (SELECT id FROM companies WHERE name = '%s'), '%s', NULL, %s, %d)", 
                     description, amount, type, category_id, payment_method_id, company_name, date_record, is_repeated ? "TRUE" : "FALSE", account_id);
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

// Function to add an account
void addAccount(PGconn *conn, const char *title_account, float balance, int banks_company_id) {
    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO account (title_account, balance, banks_company_id) VALUES ('%s', %.2f, %d)", 
             title_account, balance, banks_company_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Account insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Account added successfully!\n");
}

// Function to view accounts
void viewAccounts(PGconn *conn) {
    const char *query = "SELECT a.id, a.title_account, a.balance, b.name AS bank_name FROM account a LEFT JOIN banks_company b ON a.banks_company_id = b.id ORDER BY a.id";
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





// Function to view bank companies
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
    int account_id; // Add this line to declare account_id

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
            
                // For Pix and other methods that require an account ID, ask for account ID
                if (payment_method_id == 4) { // Pix
                    viewAccounts(conn); // Show available accounts
                    printf("Enter account ID: ");
                    scanf("%d", &account_id); // Get account ID from user
                } else {
                    account_id = 0; // Not applicable for non-Pix payments
                }
            
                addTransaction(conn, description, amount, "expense", category_id, payment_method_id, company_name, company_location, date_record, credit_card_id, is_repeated, account_id);
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
                addAccount(conn, description, amount, category_id);
                break;
            case 12: // View Accounts
                viewAccounts(conn);
                break;
            case 13: // View Bank Companies
                viewBankCompanies(conn);
                break;

            case 14: // Exit
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
            








            -- psql -d database -U user

            -- psql -d shutdown_logs -U postgres
            -- SELECT * FROM shutdown_events;










WINDOWS:

    gcc -o finance_control_New finance_control_New.c -I "C:\Program Files\PostgreSQL\<version>\include" -L "C:\Program Files\PostgreSQL\<version>\lib" -lpq

    gcc -o finance_control_New-prd finance_control_New-prd.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq











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












