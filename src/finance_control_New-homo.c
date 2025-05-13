
















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
#define _XOPEN_SOURCE 700
#include <unistd.h>





// Function declarations
void clearScreen();
void printMenu();
void getCurrentDate(char *date);
void detect_os();
void addTransaction(PGconn *conn, int user_id, const char *title, const char *description, float amount, const char *type, int category_id, int subcategory_id, int payment_method_id, const char *company_name, const char *company_location, const char *date_record, const char *purchase_date, int credit_card_id, int is_repeated, int account_id, int is_installment, int total_installments, float installment_value);
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record, int account_id);
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
void addAccount(PGconn *conn, int user_id, const char *title_account, float balance, int banks_company_id, const char *institution_name, const char *agency_number, const char *account_number,  int account_type_id);
void viewAccounts(PGconn *conn, int user_id); // Update this
void recordLoginActivity(PGconn *conn, int user_id);
void recordLogoutActivity(PGconn *conn, int user_id);
void addCategory(PGconn *conn, int is_income_category);
void viewSubcategories(PGconn *conn, int category_id);
void addSubcategory(PGconn *conn);
void viewAccountTypes(PGconn *conn);
// Add these declarations with the other function declarations
void addDebitCard(PGconn *conn, int user_id);
void viewDebitCards(PGconn *conn, int user_id);
void updateDebitCard(PGconn *conn, int user_id);





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
    printf("15. Add Category\n");
    printf("16. Add Subcategory\n");
    printf("17. Transfer Between Accounts\n");
    printf("18. View Transfer History\n");
    printf("19. Add Spending Limit\n");  // New option
    printf("20. View Spending Limits\n"); // New option
    printf("21. Generate Invoices\n");
    printf("22. View Invoices\n");
    printf("23. Pay Invoice\n");
    printf("24. View Invoice Details\n");    
    printf("25. Add Debit Card\n");  // New option
    printf("26. View Debit Cards\n"); // New option    
    printf("27. Update Debit Card\n");  // New option  
    printf("28. Exit\n");
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



void addTransaction(PGconn *conn, int user_id, const char *title, const char *description, float amount, const char *type, 
                   int category_id, int subcategory_id, int payment_method_id, const char *company_name, 
                   const char *company_location, const char *date_record, const char *purchase_date, 
                   int credit_card_id, int is_repeated, int account_id, int is_installment, 
                   int total_installments, float installment_value) {
    // Start transaction
    PGresult *res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);

    char query[1024];
    char account_id_str[20];
    char subcategory_id_str[20];
    char is_installment_str[10] = "FALSE";
    char installment_number_str[10] = "NULL";
    char total_installments_str[10] = "NULL";
    char installment_value_str[20] = "NULL";
    char original_transaction_id_str[20] = "NULL";
    char company_id_str[20] = "NULL";
    char credit_card_id_str[20] = "NULL";
    char debit_card_id_str[20] = "NULL";

    // Handle NULL values
    if (account_id == -1) strcpy(account_id_str, "NULL");
    else snprintf(account_id_str, sizeof(account_id_str), "%d", account_id);

    if (subcategory_id == -1) strcpy(subcategory_id_str, "NULL");
    else snprintf(subcategory_id_str, sizeof(subcategory_id_str), "%d", subcategory_id);

    // Handle credit_card_id (only for credit card payments)
    if (payment_method_id == 2) {
        snprintf(credit_card_id_str, sizeof(credit_card_id_str), "%d", credit_card_id);
    }

    // Handle debit_card_id (only for debit card payments)
    if (payment_method_id == 3) {
        viewDebitCards(conn, user_id);
        int debit_card_id;
        printf("Enter debit card ID: ");
        scanf("%d", &debit_card_id);
        snprintf(debit_card_id_str, sizeof(debit_card_id_str), "%d", debit_card_id);
        
        // Get the linked account ID for the debit card
        char get_account_query[256];
        snprintf(get_account_query, sizeof(get_account_query),
                "SELECT account_id FROM debit_cards WHERE id = %d", debit_card_id);
        PGresult *account_res = PQexec(conn, get_account_query);
        if (PQresultStatus(account_res) != PGRES_TUPLES_OK || PQntuples(account_res) == 0) {
            fprintf(stderr, "Failed to get debit card account: %s", PQerrorMessage(conn));
            PQclear(account_res);
            PQexec(conn, "ROLLBACK");
            return;
        }
        account_id = atoi(PQgetvalue(account_res, 0, 0));
        snprintf(account_id_str, sizeof(account_id_str), "%d", account_id);
        PQclear(account_res);
    }

    // Handle company information if provided
    if (company_name[0] != '\0') {
        // Insert company if it doesn't exist
        char company_query[256];
        snprintf(company_query, sizeof(company_query), 
                 "INSERT INTO companies (name, location) VALUES ('%s', '%s') ON CONFLICT (name) DO NOTHING RETURNING id", 
                 company_name, company_location);
        
        PGresult *company_res = PQexec(conn, company_query);
        if (PQresultStatus(company_res) != PGRES_TUPLES_OK) {
            fprintf(stderr, "Company insertion failed: %s", PQerrorMessage(conn));
            PQclear(company_res);
            PQexec(conn, "ROLLBACK");
            return;
        }
        
        if (PQntuples(company_res) > 0) {
            snprintf(company_id_str, sizeof(company_id_str), "%s", PQgetvalue(company_res, 0, 0));
        } else {
            // Company exists, get its ID
            snprintf(company_query, sizeof(company_query),
                     "SELECT id FROM companies WHERE name = '%s'", company_name);
            PQclear(company_res);
            company_res = PQexec(conn, company_query);
            if (PQresultStatus(company_res) != PGRES_TUPLES_OK || PQntuples(company_res) == 0) {
                fprintf(stderr, "Failed to get company ID: %s", PQerrorMessage(conn));
                PQclear(company_res);
                PQexec(conn, "ROLLBACK");
                return;
            }
            snprintf(company_id_str, sizeof(company_id_str), "%s", PQgetvalue(company_res, 0, 0));
        }
        PQclear(company_res);
    }

    // Set installment fields
    if (is_installment) {
        strcpy(is_installment_str, "TRUE");
        snprintf(total_installments_str, sizeof(total_installments_str), "%d", total_installments);
        snprintf(installment_value_str, sizeof(installment_value_str), "%.2f", installment_value);
        snprintf(installment_number_str, sizeof(installment_number_str), "%d", 1);
        
        // Use installment value for the amount (not full amount)
        amount = installment_value;
    }

    // Insert the main transaction
    snprintf(query, sizeof(query), 
             "INSERT INTO transactions (title, description, amount, type, category_id, subcategory_id, "
             "payment_method_id, date_record, purchase_date, credit_card_id, debit_card_id, is_repeated, account_id, user_id, "
             "is_installment, installment_number, total_installments, installment_value, company_id, original_transaction_id) "
             "VALUES ('%s', '%s', %.2f, '%s', %d, %s, %d, '%s', '%s', %s, %s, %s, %s, %d, %s, %s, %s, %s, %s, %s) RETURNING id",
             title, description, amount, type, category_id, subcategory_id_str, payment_method_id, 
             date_record, purchase_date, 
             credit_card_id_str,  // Credit card ID (NULL if not credit card)
             debit_card_id_str,   // Debit card ID (NULL if not debit card)
             is_repeated ? "TRUE" : "FALSE", 
             account_id_str, user_id, is_installment_str, installment_number_str, 
             total_installments_str, installment_value_str, company_id_str, original_transaction_id_str);

    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Transaction failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }

    // Get the ID of the newly inserted transaction (for installments)
    int original_transaction_id = -1;
    if (is_installment && PQntuples(res) > 0) {
        original_transaction_id = atoi(PQgetvalue(res, 0, 0));
        snprintf(original_transaction_id_str, sizeof(original_transaction_id_str), "%d", original_transaction_id);
    }
    PQclear(res);

    // Handle installment payments
    if (is_installment && total_installments > 1) {
        // Parse the original purchase date
        struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
        if (sscanf(purchase_date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) {
            fprintf(stderr, "Failed to parse purchase date: %s\n", purchase_date);
            PQexec(conn, "ROLLBACK");
            return;
        }
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;

        // Create each subsequent installment
        for (int i = 2; i <= total_installments; i++) {
            // Calculate the date for this installment
            char installment_purchase_date[11];
            struct tm installment_tm = tm;
            installment_tm.tm_mon += (i - 1);
            mktime(&installment_tm);
            strftime(installment_purchase_date, sizeof(installment_purchase_date), "%Y-%m-%d", &installment_tm);

            // Create the installment transaction
            snprintf(query, sizeof(query), 
                     "INSERT INTO transactions (title, description, amount, type, category_id, subcategory_id, "
                     "payment_method_id, date_record, purchase_date, credit_card_id, debit_card_id, is_repeated, account_id, user_id, "
                     "is_installment, installment_number, total_installments, installment_value, company_id, original_transaction_id) "
                     "VALUES ('%s (Installment %d/%d)', '%s', %.2f, '%s', %d, %s, %d, '%s', '%s', %s, %s, %s, %s, %d, TRUE, %d, %d, %.2f, %s, %d)", 
                     title, i, total_installments, description, installment_value, type, category_id, subcategory_id_str, 
                     payment_method_id, date_record, installment_purchase_date, 
                     credit_card_id_str, debit_card_id_str,
                     is_repeated ? "TRUE" : "FALSE", 
                     account_id_str, user_id, i, total_installments, installment_value, company_id_str, original_transaction_id);

            res = PQexec(conn, query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "Failed to create installment %d: %s", i, PQerrorMessage(conn));
                PQexec(conn, "ROLLBACK");
                PQclear(res);
                return;
            }
            PQclear(res);
        }
    }

    // Update account balance for cash, debit card, or pix payments
    if (payment_method_id == 1 || payment_method_id == 3 || payment_method_id == 4) {
        if (account_id != -1) {
            char update_query[256];
            snprintf(update_query, sizeof(update_query),
                     "UPDATE account SET balance = balance - %.2f WHERE id = %d",
                     amount, account_id);
            
            res = PQexec(conn, update_query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "Account balance update failed: %s", PQerrorMessage(conn));
                PQexec(conn, "ROLLBACK");
                PQclear(res);
                return;
            }
            PQclear(res);
        }
    }

    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Print success message
    printf("Transaction added successfully!\n");
    if (is_installment) {
        printf("Installment plan created with %d installments of %.2f\n", total_installments, installment_value);
    }
    if (payment_method_id == 1 || payment_method_id == 3 || payment_method_id == 4) {
        printf("Account balance updated (reduced by %.2f)\n", amount);
    }
}


















void addAccount(PGconn *conn, int user_id, const char *title_account, float balance, 
    int banks_company_id, const char *institution_name, 
    const char *agency_number, const char *account_number, 
    int account_type_id) {
char query[512];
snprintf(query, sizeof(query), 
 "INSERT INTO account (title_account, balance, banks_company_id, user_id, "
 "institution_name, agency_number, account_number, account_type_id) "
 "VALUES ('%s', %.2f, %d, %d, '%s', '%s', '%s', %d)", 
 title_account, balance, banks_company_id, user_id, 
 institution_name, agency_number, account_number, account_type_id);

PGresult *res = PQexec(conn, query);
if (PQresultStatus(res) != PGRES_COMMAND_OK) {
fprintf(stderr, "Account insertion failed: %s", PQerrorMessage(conn));
PQclear(res);
return;
}
PQclear(res);
printf("Account added successfully!\n");
}












void viewAccounts(PGconn *conn, int user_id) {
    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT a.id, a.title_account, a.balance, b.name AS bank_name, "
             "a.institution_name, a.agency_number, a.account_number, at.name AS account_type "
             "FROM account a "
             "LEFT JOIN banks_company b ON a.banks_company_id = b.id "
             "LEFT JOIN account_type at ON a.account_type_id = at.id "
             "WHERE a.user_id = %d "
             "ORDER BY a.id", user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Account Title     | Balance  | Bank Name       | Institution    | Agency   | Account Number | Type\n");
    printf("--------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-17s | %-8s | %-15s | %-14s | %-8s | %-14s | %s\n",
               PQgetvalue(res, i, 0),  // ID
               PQgetvalue(res, i, 1),  // title_account
               PQgetvalue(res, i, 2),  // balance
               PQgetvalue(res, i, 3),  // bank_name
               PQgetvalue(res, i, 4),  // institution_name
               PQgetvalue(res, i, 5),  // agency_number
               PQgetvalue(res, i, 6),  // account_number
               PQgetvalue(res, i, 7)); // account_type
    }
    PQclear(res);
}











// Function to add income
void addIncome(PGconn *conn, const char *description, float amount, int category_income_id, int payment_method_id, const char *date_record, int account_id) {
    // Start transaction
    PGresult *res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Insert income record
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO income (description, amount, category_income_id, payment_method_id, date_record, account_id) "
             "VALUES ('%s', %.2f, %d, %d, '%s', %d)", 
             description, amount, category_income_id, payment_method_id, date_record, account_id);

    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Income insertion failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Update account balance
    char update_query[256];
    snprintf(update_query, sizeof(update_query),
             "UPDATE account SET balance = balance + %.2f WHERE id = %d",
             amount, account_id);
    
    res = PQexec(conn, update_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Account balance update failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    printf("Income added and account updated successfully!\n");
}



















void viewTransactions(PGconn *conn, int user_id) {
    char query[2048]; // Increased buffer size
    snprintf(query, sizeof(query),
             "SELECT t.id, t.title, t.description, t.amount, t.type, "
             "c.name AS category, sc.name AS subcategory, pm.method AS payment_method, "
             "co.name AS company, t.date_record, t.purchase_date, t.date, "
             "cc.card_name, dc.card_name AS debit_card, t.is_repeated, a.title_account AS account_name, "
             "t.is_installment, t.installment_number, t.total_installments, t.installment_value "
             "FROM transactions t "
             "LEFT JOIN categories c ON t.category_id = c.id "
             "LEFT JOIN subcategories sc ON t.subcategory_id = sc.id "
             "LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id "
             "LEFT JOIN companies co ON t.company_id = co.id "
             "LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id "
             "LEFT JOIN debit_cards dc ON t.debit_card_id = dc.id "
             "LEFT JOIN account a ON t.account_id = a.id "
             "WHERE t.user_id = %d "
             "ORDER BY t.date DESC",
             user_id);

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
        // Updated header to include Debit Card column
        printf("ID | Title            | Description       | Amount  | Type    | Category      | Subcategory    | Payment Method | Company        | Date Record | Purchase Date | Date       | Credit Card | Debit Card  | Repeated | Account | Installment\n");
        printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            const char *is_installment = PQgetvalue(res, i, 16); // Updated index for is_installment
            const char *installment_info = "";
            if (strcmp(is_installment, "t") == 0) {
                char buf[50];
                snprintf(buf, sizeof(buf), "%s/%s (%s)", 
                         PQgetvalue(res, i, 17), // installment_number
                         PQgetvalue(res, i, 18), // total_installments
                         PQgetvalue(res, i, 19)); // installment_value
                installment_info = buf;
            }
            
            // Updated printf to include debit card information
            printf("%s | %-15s | %-17s | %-7s | %-7s | %-13s | %-14s | %-14s | %-14s | %-11s | %-12s | %s | %-11s | %-11s | %s | %s | %s\n",
                   PQgetvalue(res, i, 0),   // ID
                   PQgetvalue(res, i, 1),   // Title
                   PQgetvalue(res, i, 2),   // Description
                   PQgetvalue(res, i, 3),   // Amount
                   PQgetvalue(res, i, 4),   // Type
                   PQgetvalue(res, i, 5),   // Category
                   PQgetvalue(res, i, 6),   // Subcategory
                   PQgetvalue(res, i, 7),   // Payment Method
                   PQgetvalue(res, i, 8),   // Company
                   PQgetvalue(res, i, 9),   // Date Record
                   PQgetvalue(res, i, 10),  // Purchase Date
                   PQgetvalue(res, i, 11),  // Date
                   PQgetvalue(res, i, 12),  // Credit Card
                   PQgetvalue(res, i, 13),  // Debit Card
                   PQgetvalue(res, i, 14),  // is_repeated
                   PQgetvalue(res, i, 15),  // Account
                   installment_info);       // Installment info
        }
    }
    PQclear(res);
}











// Function to view income
void viewIncome(PGconn *conn) {
    const char *query = "SELECT i.id, i.description, i.amount, ci.name AS category, "
                        "pm.method AS payment_method, a.title_account AS account, "
                        "i.date_record, i.date "
                        "FROM income i "
                        "LEFT JOIN categories_income ci ON i.category_income_id = ci.id "
                        "LEFT JOIN payment_methods pm ON i.payment_method_id = pm.id "
                        "LEFT JOIN account a ON i.account_id = a.id "
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
        printf("ID | Description       | Amount  | Category      | Payment Method | Account       | Date Record | Date\n");
        printf("------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-7s | %-13s | %-14s | %-13s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),  // ID
                   PQgetvalue(res, i, 1),  // Description
                   PQgetvalue(res, i, 2),  // Amount
                   PQgetvalue(res, i, 3),  // Category
                   PQgetvalue(res, i, 4),  // Payment Method
                   PQgetvalue(res, i, 5),  // Account
                   PQgetvalue(res, i, 6),  // Date Record
                   PQgetvalue(res, i, 7)); // Date
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


void viewSubcategories(PGconn *conn, int category_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT id, name FROM subcategories WHERE category_id = %d ORDER BY id", 
             category_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Subcategory Name\n");
    printf("-------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}


void addSubcategory(PGconn *conn) {
    viewCategories(conn);
    
    int category_id;
    char subcategory_name[100];
    
    printf("Enter category ID for the new subcategory: ");
    scanf("%d", &category_id);
    printf("Enter new subcategory name: ");
    scanf(" %[^\n]", subcategory_name);
    
    char query[256];
    snprintf(query, sizeof(query), 
             "INSERT INTO subcategories (name, category_id) VALUES ('%s', %d)", 
             subcategory_name, category_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to add subcategory: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Subcategory added successfully!\n");
}




// Function to transfer money between accounts
void transferBetweenAccounts(PGconn *conn, int user_id) {
    int from_account_id, to_account_id;
    float amount;
    char description[100];
    char current_date[11];
    
    // View user's accounts
    viewAccounts(conn, user_id);
    
    // Get transfer details
    printf("\nEnter FROM account ID: ");
    scanf("%d", &from_account_id);
    printf("Enter TO account ID: ");
    scanf("%d", &to_account_id);
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    printf("Enter description (optional): ");
    scanf(" %[^\n]", description);
    
    // Get current date
    getCurrentDate(current_date);
    
    // Start transaction
    PGresult *res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Check if FROM account has sufficient balance
    char check_balance_query[256];
    snprintf(check_balance_query, sizeof(check_balance_query),
             "SELECT balance FROM account WHERE id = %d AND user_id = %d",
             from_account_id, user_id);
    
    res = PQexec(conn, check_balance_query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        fprintf(stderr, "Failed to check account balance: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    
    float current_balance = atof(PQgetvalue(res, 0, 0));
    PQclear(res);
    
    if (current_balance < amount) {
        printf("Insufficient balance in the source account!\n");
        PQexec(conn, "ROLLBACK");
        return;
    }
    
    // Deduct from FROM account
    char deduct_query[256];
    snprintf(deduct_query, sizeof(deduct_query),
             "UPDATE account SET balance = balance - %.2f WHERE id = %d",
             amount, from_account_id);
    
    res = PQexec(conn, deduct_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to deduct from account: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Add to TO account
    char add_query[256];
    snprintf(add_query, sizeof(add_query),
             "UPDATE account SET balance = balance + %.2f WHERE id = %d",
             amount, to_account_id);
    
    res = PQexec(conn, add_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to add to account: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Record the transfer
    char transfer_query[512];
    snprintf(transfer_query, sizeof(transfer_query),
             "INSERT INTO transf_account (from_account_id, to_account_id, amount, transfer_date, description) "
             "VALUES (%d, %d, %.2f, '%s', '%s')",
             from_account_id, to_account_id, amount, current_date, description);
    
    res = PQexec(conn, transfer_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to record transfer: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    printf("Successfully transferred %.2f from account %d to account %d\n", amount, from_account_id, to_account_id);
}

// Function to view transfer history
void viewTransferHistory(PGconn *conn, int user_id) {
    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT t.id, a1.title_account AS from_account, "
             "a2.title_account AS to_account, t.amount, t.transfer_date, t.description "
             "FROM transf_account t "
             "JOIN account a1 ON t.from_account_id = a1.id "
             "JOIN account a2 ON t.to_account_id = a2.id "
             "WHERE a1.user_id = %d OR a2.user_id = %d "
             "ORDER BY t.transfer_date DESC",
             user_id, user_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No transfer history found.\n");
    } else {
        printf("ID | From Account      | To Account        | Amount  | Transfer Date | Description\n");
        printf("--------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-17s | %-7s | %-13s | %s\n",
                   PQgetvalue(res, i, 0),  // ID
                   PQgetvalue(res, i, 1),  // From Account
                   PQgetvalue(res, i, 2),  // To Account
                   PQgetvalue(res, i, 3),  // Amount
                   PQgetvalue(res, i, 4),  // Transfer Date
                   PQgetvalue(res, i, 5)); // Description
        }
    }
    PQclear(res);
}




void viewAccountTypes(PGconn *conn) {
    const char *query = "SELECT * FROM account_type ORDER BY id";
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Account Type\n");
    printf("----------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }
    PQclear(res);
}




// Function to add a new spending limit
void addSpendingLimit(PGconn *conn, int user_id) {
    int category_id;
    float limit_amount;
    char period[20];
    
    // View available categories
    viewCategories(conn);
    
    printf("Enter category ID for the spending limit: ");
    scanf("%d", &category_id);
    
    printf("Enter limit amount: ");
    scanf("%f", &limit_amount);
    
    printf("Enter period (monthly/yearly): ");
    scanf(" %[^\n]", period);
    
    // Validate period
    if (strcmp(period, "monthly") != 0 && strcmp(period, "yearly") != 0) {
        printf("Invalid period. Must be 'monthly' or 'yearly'.\n");
        return;
    }
    
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO spending_limits (category_id, user_id, limit_amount, period) "
             "VALUES (%d, %d, %.2f, '%s')", 
             category_id, user_id, limit_amount, period);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to add spending limit: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Spending limit added successfully!\n");
}



// Function to view spending limits with current spending
void viewSpendingLimits(PGconn *conn, int user_id) {
    char query[1024];
    
    // First, update all current_values based on actual spending
    snprintf(query, sizeof(query),
             "UPDATE spending_limits sl "
             "SET current_value = ("
             "    SELECT COALESCE(SUM(t.amount), 0) "
             "    FROM transactions t "
             "    WHERE t.category_id = sl.category_id "
             "    AND t.user_id = sl.user_id "
             "    AND t.type = 'expense' "
             "    AND CASE "
             "        WHEN sl.period = 'monthly' THEN "
             "            EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE) "
             "            AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE) "
             "        WHEN sl.period = 'yearly' THEN "
             "            EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE) "
             "    END"
             ") "
             "WHERE sl.user_id = %d", user_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to update spending limits: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Now select the limits with current spending
    snprintf(query, sizeof(query),
             "SELECT sl.id, c.name AS category, sl.limit_amount, sl.current_value, "
             "sl.period, sl.start_date, sl.is_enabled, "
             "CASE WHEN sl.current_value >= sl.limit_amount THEN 'OVER LIMIT' "
             "     WHEN sl.current_value >= sl.limit_amount * 0.9 THEN 'NEAR LIMIT' "
             "     ELSE 'UNDER LIMIT' END AS status "
             "FROM spending_limits sl "
             "JOIN categories c ON sl.category_id = c.id "
             "WHERE sl.user_id = %d "
             "ORDER BY status, c.name", user_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No spending limits found.\n");
    } else {
        printf("ID | Category          | Limit Amount | Current Spending | Period   | Start Date | Status      | Enabled\n");
        printf("------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-17s | %-12s | %-16s | %-8s | %-10s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),  // ID
                   PQgetvalue(res, i, 1),  // Category
                   PQgetvalue(res, i, 2),  // Limit Amount
                   PQgetvalue(res, i, 3),  // Current Value
                   PQgetvalue(res, i, 4),  // Period
                   PQgetvalue(res, i, 5),  // Start Date
                   PQgetvalue(res, i, 7),  // Status
                   PQgetvalue(res, i, 6)); // Is Enabled
        }
    }
    PQclear(res);
}








void generateInvoices(PGconn *conn, int user_id) {
    char current_date[11];
    getCurrentDate(current_date);
    
    // Start transaction
    PGresult *res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Corrected query that joins with transactions to get user_id
    char query[2048];
    snprintf(query, sizeof(query),
        "WITH user_credit_cards AS ("
        "  SELECT DISTINCT cc.id, cc.card_name, cc.closes_on_day, cc.due_day "
        "  FROM credit_cards cc "
        "  JOIN transactions t ON cc.id = t.credit_card_id "
        "  WHERE t.user_id = %d"
        "), "
        "card_periods AS ("
        "  SELECT "
        "    ucc.id AS credit_card_id, "
        "    CASE "
        "      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN "
        "        DATE_TRUNC('month', CURRENT_DATE)::date + (ucc.closes_on_day - 1) * INTERVAL '1 day' "
        "      ELSE "
        "        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day' "
        "    END AS invoice_date, "
        "    CASE "
        "      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN "
        "        DATE_TRUNC('month', CURRENT_DATE)::date + (ucc.due_day - 1) * INTERVAL '1 day' "
        "      ELSE "
        "        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.due_day - 1) * INTERVAL '1 day' "
        "    END AS due_date, "
        "    CASE "
        "      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN "
        "        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day' "
        "      ELSE "
        "        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '2 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day' "
        "    END AS period_start "
        "  FROM user_credit_cards ucc"
        ") "
        "INSERT INTO invoices (credit_card_id, user_id, invoice_date, due_date, total_amount) "
        "SELECT cp.credit_card_id, %d, cp.invoice_date, cp.due_date, "
        "       COALESCE(SUM(t.amount), 0) AS total_amount "
        "FROM card_periods cp "
        "LEFT JOIN transactions t ON t.credit_card_id = cp.credit_card_id "
        "                       AND t.purchase_date >= cp.period_start "
        "                       AND t.purchase_date < cp.invoice_date "
        "                       AND t.user_id = %d "
        "WHERE NOT EXISTS ("
        "  SELECT 1 FROM invoices i "
        "  WHERE i.credit_card_id = cp.credit_card_id "
        "  AND i.invoice_date = cp.invoice_date"
        ") "
        "GROUP BY cp.credit_card_id, cp.invoice_date, cp.due_date "
        "HAVING COALESCE(SUM(t.amount), 0) > 0",
        user_id, user_id, user_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Invoice generation failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    
    int rows_affected = atoi(PQcmdTuples(res));
    PQclear(res);
    
    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    printf("Generated %d new invoices.\n", rows_affected);
}






// Function to view invoices
void viewInvoices(PGconn *conn, int user_id) {
    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT i.id, cc.card_name, i.invoice_date, i.due_date, "
        "i.total_amount, i.paid_amount, i.status, "
        "(i.total_amount - i.paid_amount) AS remaining_amount "
        "FROM invoices i "
        "JOIN credit_cards cc ON i.credit_card_id = cc.id "
        "WHERE i.user_id = %d "
        "ORDER BY i.status, i.due_date", user_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No invoices found.\n");
    } else {
        printf("ID | Credit Card      | Invoice Date | Due Date   | Total Amount | Paid Amount | Remaining | Status\n");
        printf("------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-16s | %-12s | %-10s | %-12s | %-11s | %-9s | %s\n",
                   PQgetvalue(res, i, 0),  // ID
                   PQgetvalue(res, i, 1),  // Card Name
                   PQgetvalue(res, i, 2),  // Invoice Date
                   PQgetvalue(res, i, 3),  // Due Date
                   PQgetvalue(res, i, 4),  // Total Amount
                   PQgetvalue(res, i, 5),  // Paid Amount
                   PQgetvalue(res, i, 7),  // Remaining Amount
                   PQgetvalue(res, i, 6)); // Status
        }
    }
    PQclear(res);
}

// Function to pay an invoice
void payInvoice(PGconn *conn, int user_id) {
    // Show unpaid invoices first
    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT i.id, cc.card_name, i.invoice_date, i.due_date, "
        "i.total_amount, i.paid_amount, "
        "(i.total_amount - i.paid_amount) AS remaining_amount "
        "FROM invoices i "
        "JOIN credit_cards cc ON i.credit_card_id = cc.id "
        "WHERE i.user_id = %d AND i.status != 'paid' "
        "ORDER BY i.due_date", user_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No unpaid invoices found.\n");
        PQclear(res);
        return;
    }
    
    printf("Unpaid Invoices:\n");
    printf("ID | Credit Card      | Invoice Date | Due Date   | Total Amount | Paid Amount | Remaining\n");
    printf("---------------------------------------------------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-16s | %-12s | %-10s | %-12s | %-11s | %s\n",
               PQgetvalue(res, i, 0),  // ID
               PQgetvalue(res, i, 1),  // Card Name
               PQgetvalue(res, i, 2),  // Invoice Date
               PQgetvalue(res, i, 3),  // Due Date
               PQgetvalue(res, i, 4),  // Total Amount
               PQgetvalue(res, i, 5),  // Paid Amount
               PQgetvalue(res, i, 6)); // Remaining Amount
    }
    PQclear(res);
    
    // Get invoice ID to pay
    int invoice_id;
    printf("Enter invoice ID to pay: ");
    scanf("%d", &invoice_id);
    
    float payment_amount;
    printf("Enter payment amount: ");
    scanf("%f", &payment_amount);
    
    int account_id;
    viewAccounts(conn, user_id);
    printf("Select account to pay from: ");
    scanf("%d", &account_id);
    
    // Start transaction
    res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // 1. Verify invoice exists and get remaining amount
    snprintf(query, sizeof(query),
        "SELECT (total_amount - paid_amount) AS remaining "
        "FROM invoices WHERE id = %d AND user_id = %d FOR UPDATE", 
        invoice_id, user_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        fprintf(stderr, "Invalid invoice ID or not your invoice: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    
    float remaining_amount = atof(PQgetvalue(res, 0, 0));
    PQclear(res);
    
    if (payment_amount > remaining_amount) {
        printf("Payment amount (%.2f) exceeds remaining amount (%.2f).\n", payment_amount, remaining_amount);
        PQexec(conn, "ROLLBACK");
        return;
    }
    
    // 2. Verify account has sufficient balance
    snprintf(query, sizeof(query),
        "SELECT balance FROM account WHERE id = %d AND user_id = %d FOR UPDATE", 
        account_id, user_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        fprintf(stderr, "Invalid account ID or not your account: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    
    float account_balance = atof(PQgetvalue(res, 0, 0));
    PQclear(res);
    
    if (account_balance < payment_amount) {
        printf("Insufficient funds in account (balance: %.2f).\n", account_balance);
        PQexec(conn, "ROLLBACK");
        return;
    }
    
    // 3. Update invoice
    snprintf(query, sizeof(query),
        "UPDATE invoices "
        "SET paid_amount = paid_amount + %.2f, "
        "    status = CASE WHEN (paid_amount + %.2f) >= total_amount THEN 'paid' ELSE status END, "
        "    updated_at = CURRENT_TIMESTAMP "
        "WHERE id = %d", 
        payment_amount, payment_amount, invoice_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Invoice update failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // 4. Deduct from account
    snprintf(query, sizeof(query),
        "UPDATE account SET balance = balance - %.2f WHERE id = %d", 
        payment_amount, account_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Account update failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // 5. Record the payment transaction
    char current_date[11];
    getCurrentDate(current_date);
    
    snprintf(query, sizeof(query),
        "INSERT INTO transactions (title, description, amount, type, "
        "category_id, payment_method_id, date_record, purchase_date, "
        "account_id, user_id) "
        "VALUES ('Invoice Payment', 'Payment for invoice #%d', %.2f, 'expense', "
        "6, 3, '%s', '%s', %d, %d)",  // Assuming category_id 6 is "Credit Card Payments"
        invoice_id, payment_amount, current_date, current_date, account_id, user_id);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Transaction recording failed: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    
    printf("Payment of %.2f applied to invoice #%d successfully!\n", payment_amount, invoice_id);
}




// Function to view invoice details (transactions)
void viewInvoiceDetails(PGconn *conn, int user_id) {
    // Show invoices first
    viewInvoices(conn, user_id);
    
    int invoice_id;
    printf("Enter invoice ID to view details: ");
    scanf("%d", &invoice_id);
    
    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT t.id, t.title, t.description, t.amount, t.purchase_date, "
        "c.name AS category, sc.name AS subcategory "
        "FROM transactions t "
        "JOIN invoices i ON t.credit_card_id = i.credit_card_id "
        "LEFT JOIN categories c ON t.category_id = c.id "
        "LEFT JOIN subcategories sc ON t.subcategory_id = sc.id "
        "WHERE i.id = %d AND i.user_id = %d "
        "AND t.purchase_date >= ("
        "  SELECT DATE_TRUNC('month', i.invoice_date - INTERVAL '1 month') + "
        "         (cc.closes_on_day - 1) * INTERVAL '1 day' "
        "  FROM credit_cards cc "
        "  WHERE cc.id = i.credit_card_id"
        ") "
        "AND t.purchase_date < i.invoice_date "
        "ORDER BY t.purchase_date", invoice_id, user_id);
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    if (rows == 0) {
        printf("No transactions found for this invoice.\n");
    } else {
        printf("Invoice #%d Details:\n", invoice_id);
        printf("ID | Title            | Description       | Amount  | Purchase Date | Category    | Subcategory\n");
        printf("-------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < rows; i++) {
            printf("%s | %-15s | %-17s | %-7s | %-13s | %-11s | %s\n",
                   PQgetvalue(res, i, 0),  // ID
                   PQgetvalue(res, i, 1),  // Title
                   PQgetvalue(res, i, 2),  // Description
                   PQgetvalue(res, i, 3),  // Amount
                   PQgetvalue(res, i, 4),  // Purchase Date
                   PQgetvalue(res, i, 5),  // Category
                   PQgetvalue(res, i, 6)); // Subcategory
        }
    }
    PQclear(res);
}



// Function to add a new debit card
void addDebitCard(PGconn *conn, int user_id) {
    char card_name[100], card_number[20], expiration_date[11], cvv[5];
    int account_id;
    
    viewAccounts(conn, user_id);
    printf("Enter account ID to link to this debit card: ");
    scanf("%d", &account_id);
    
    printf("Enter card name: ");
    scanf(" %[^\n]", card_name);
    printf("Enter card number: ");
    scanf(" %[^\n]", card_number);
    printf("Enter expiration date (YYYY-MM-DD): ");
    scanf(" %[^\n]", expiration_date);
    printf("Enter CVV: ");
    scanf(" %[^\n]", cvv);
    
    char query[512];
    snprintf(query, sizeof(query), 
             "INSERT INTO debit_cards (card_name, account_id, card_number, expiration_date, cvv, user_id) "
             "VALUES ('%s', %d, '%s', '%s', '%s', %d)", 
             card_name, account_id, card_number, expiration_date, cvv, user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Debit card insertion failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);
    printf("Debit card added successfully!\n");
}

// Function to view debit cards
void viewDebitCards(PGconn *conn, int user_id) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT dc.id, dc.card_name, a.title_account, dc.card_number, "
             "dc.expiration_date, dc.created_at "
             "FROM debit_cards dc "
             "JOIN account a ON dc.account_id = a.id "
             "WHERE dc.user_id = %d "
             "ORDER BY dc.id", user_id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    printf("ID | Card Name       | Linked Account   | Card Number   | Expiration | Created At\n");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < rows; i++) {
        printf("%s | %-15s | %-16s | %-13s | %-10s | %s\n",
               PQgetvalue(res, i, 0),  // ID
               PQgetvalue(res, i, 1),  // Card Name
               PQgetvalue(res, i, 2),  // Account
               PQgetvalue(res, i, 3),  // Card Number
               PQgetvalue(res, i, 4),  // Expiration
               PQgetvalue(res, i, 5)); // Created At
    }
    PQclear(res);
}



void updateDebitCard(PGconn *conn, int user_id) {
    int debit_card_id;
    char new_card_name[100], new_expiration_date[11], new_cvv[5];
    int update_name = 0, update_expiry = 0, update_cvv = 0;
    
    // Show user's debit cards
    viewDebitCards(conn, user_id);
    
    printf("Enter the ID of the debit card to update: ");
    if (scanf("%d", &debit_card_id) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    
    // Verify the debit card belongs to the user
    char verify_query[256];
    snprintf(verify_query, sizeof(verify_query),
             "SELECT card_name, expiration_date FROM debit_cards WHERE id = %d AND user_id = %d",
             debit_card_id, user_id);
    
    PGresult *verify_res = PQexec(conn, verify_query);
    if (PQresultStatus(verify_res) != PGRES_TUPLES_OK || PQntuples(verify_res) == 0) {
        fprintf(stderr, "Invalid debit card ID or not your card\n");
        PQclear(verify_res);
        return;
    }
    
    // Get current values
    const char *current_name = PQgetvalue(verify_res, 0, 0);
    const char *current_expiry = PQgetvalue(verify_res, 0, 1);
    
    // Ask which fields to update
    printf("\nCurrent name: %s\n", current_name);
    printf("Update name? (1 for Yes, 0 for No): ");
    scanf("%d", &update_name);
    
    if (update_name) {
        printf("Enter new card name: ");
        scanf(" %[^\n]", new_card_name);
    }
    
    printf("\nCurrent expiration date: %s\n", current_expiry);
    printf("Update expiration date? (1 for Yes, 0 for No): ");
    scanf("%d", &update_expiry);
    
    if (update_expiry) {
        printf("Enter new expiration date (YYYY-MM-DD): ");
        scanf(" %[^\n]", new_expiration_date);
    }
    
    printf("\nUpdate CVV? (1 for Yes, 0 for No): ");
    scanf("%d", &update_cvv);
    
    if (update_cvv) {
        printf("Enter new CVV: ");
        scanf(" %[^\n]", new_cvv);
    }
    
    PQclear(verify_res);
    
    // Build the update query
    char update_query[512] = "UPDATE debit_cards SET ";
    int fields_to_update = 0;
    
    if (update_name) {
        char escaped_name[200];
        PQescapeStringConn(conn, escaped_name, new_card_name, strlen(new_card_name), NULL);
        strcat(update_query, "card_name = '");
        strcat(update_query, escaped_name);
        strcat(update_query, "'");
        fields_to_update++;
    }
    
    if (update_expiry) {
        if (fields_to_update > 0) strcat(update_query, ", ");
        strcat(update_query, "expiration_date = '");
        strcat(update_query, new_expiration_date);
        strcat(update_query, "'");
        fields_to_update++;
    }
    
    if (update_cvv) {
        if (fields_to_update > 0) strcat(update_query, ", ");
        strcat(update_query, "cvv = '");
        strcat(update_query, new_cvv);
        strcat(update_query, "'");
        fields_to_update++;
    }
    
    if (fields_to_update == 0) {
        printf("No fields selected for update.\n");
        return;
    }
    
    // Complete the query
    strcat(update_query, " WHERE id = ");
    char id_str[20];
    snprintf(id_str, sizeof(id_str), "%d", debit_card_id);
    strcat(update_query, id_str);
    
    // Execute the update
    PGresult *res = PQexec(conn, update_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Update failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    
    PQclear(res);
    printf("\nDebit card updated successfully!\n");
    
    // Show the updated card
    char show_query[256];
    snprintf(show_query, sizeof(show_query),
             "SELECT dc.id, dc.card_name, dc.expiration_date, dc.updated_at "
             "FROM debit_cards dc "
             "WHERE dc.id = %d", debit_card_id);
    
    res = PQexec(conn, show_query);
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        printf("\nUpdated Information:\n");
        printf("ID: %s\n", PQgetvalue(res, 0, 0));
        printf("Name: %s\n", PQgetvalue(res, 0, 1));
        printf("Expiration Date: %s\n", PQgetvalue(res, 0, 2));
        printf("Last Updated: %s\n", PQgetvalue(res, 0, 3));
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
                    viewAccounts(conn, user_id);  // Show available accounts
                    printf("Enter account ID to receive the income: ");
                    scanf("%d", &account_id);
                    printf("Enter date record (YYYY-MM-DD): ");
                    scanf(" %[^\n]", date_record);
                    addIncome(conn, description, amount, category_income_id, payment_method_id, date_record, account_id);
                break;
                case 2: { // Add Expense
                    char title[100];
                    char purchase_date[11]; // YYYY-MM-DD format
                    printf("Enter title: ");
                    scanf(" %[^\n]", title);
                    printf("Enter description: ");
                    scanf(" %[^\n]", description);
                    printf("Enter amount: ");
                    scanf("%f", &amount);
                    
                    // Show categories
                    viewCategories(conn);
                    printf("Enter category ID: ");
                    scanf("%d", &category_id);
                    
                    // Ask if user wants to add a subcategory
                    int use_subcategory;
                    int subcategory_id = -1; // Default to NULL
                    printf("Do you want to add a subcategory? (1 for Yes, 0 for No): ");
                    scanf("%d", &use_subcategory);
                    
                    if (use_subcategory == 1) {
                        viewSubcategories(conn, category_id);
                        printf("Enter subcategory ID (or -1 to skip): ");
                        scanf("%d", &subcategory_id);
                    }
                    
                    viewPaymentMethods(conn);
                    printf("Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card, 4 for Pix): ");
                    scanf("%d", &payment_method_id);
                    
                    int is_installment = 0;
                    int total_installments = 1;
                    float installment_value = amount;
                    
                    if (payment_method_id == 2) { // Credit Card
                        viewCreditCards(conn);
                        printf("Enter credit card ID: ");
                        scanf("%d", &credit_card_id);
                        
                        // Ask about installments
                        printf("Is this an installment purchase? (1 for Yes, 0 for No): ");
                        scanf("%d", &is_installment);
                        
                        if (is_installment) {
                            printf("Enter total number of installments: ");
                            scanf("%d", &total_installments);
                            installment_value = amount / total_installments;
                            printf("Each installment will be %.2f\n", installment_value);
                        }
                    } else {
                        credit_card_id = 0; // Not applicable for non-credit card payments
                    }
                    
                    printf("Is this transaction repeated? (1 for Yes, 0 for No): ");
                    scanf("%d", &is_repeated);
                    
                    // Company handling section
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
                    
                    // Ask for account ID for Cash, Debit Card, or Pix payments
                    if (payment_method_id == 1 || payment_method_id == 3 || payment_method_id == 4) {
                        viewAccounts(conn, user_id);
                        printf("Enter account ID to deduct from: ");
                        scanf("%d", &account_id);
                        
                        // Verify account has sufficient balance
                        char balance_query[256];
                        snprintf(balance_query, sizeof(balance_query),
                                "SELECT balance FROM account WHERE id = %d AND user_id = %d",
                                account_id, user_id);
                        
                        PGresult *balance_res = PQexec(conn, balance_query);
                        if (PQresultStatus(balance_res) != PGRES_TUPLES_OK || PQntuples(balance_res) == 0) {
                            fprintf(stderr, "Failed to check account balance: %s", PQerrorMessage(conn));
                            PQclear(balance_res);
                            break;
                        }
                        
                        float current_balance = atof(PQgetvalue(balance_res, 0, 0));
                        PQclear(balance_res);
                        
                        if (current_balance < amount) {
                            printf("Error: Insufficient balance in account (Current balance: %.2f)\n", current_balance);
                            break;
                        }
                    } else {
                        account_id = -1; // Not applicable for credit card payments
                    }
                    
                    addTransaction(conn, user_id, title, description, amount, "expense", 
                                category_id, subcategory_id, payment_method_id, 
                                company_name, company_location, date_record, purchase_date, 
                                credit_card_id, is_repeated, account_id, is_installment, 
                                total_installments, installment_value);
                    break;
                }
                

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
            case 11:  // Add Account
                    char title_account[100], institution_name[100], agency_number[20], account_number[30];
                    float balance;
                    int banks_company_id, account_type_id;
                    
                    printf("Enter account title: ");
                    scanf(" %[^\n]", title_account);
                    printf("Enter initial balance: ");
                    scanf("%f", &balance);
                    printf("Enter institution name: ");
                    scanf(" %[^\n]", institution_name);
                    printf("Enter agency number: ");
                    scanf(" %[^\n]", agency_number);
                    printf("Enter account number: ");
                    scanf(" %[^\n]", account_number);
                    
                    viewBankCompanies(conn);
                    printf("Enter bank company ID: ");
                    scanf("%d", &banks_company_id);
                    
                    viewAccountTypes(conn);
                    printf("Enter account type ID: ");
                    scanf("%d", &account_type_id);
                    
                    addAccount(conn, user_id, title_account, balance, banks_company_id, 
                               institution_name, agency_number, account_number, account_type_id);
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
            case 16: // Add Subcategory
                addSubcategory(conn);
                break;
            case 17: // Transfer Between Accounts
                transferBetweenAccounts(conn, user_id);
                break;
            case 18: // View Transfer History
                viewTransferHistory(conn, user_id);
                break;
            // In the main function's switch statement:
            case 19: // Add Spending Limit
                addSpendingLimit(conn, user_id);
                break;
            case 20: // View Spending Limits
                viewSpendingLimits(conn, user_id);
                break;
            // In the main function's switch statement:
            case 21: // Generate Invoices
                generateInvoices(conn, user_id);
                break;
            case 22: // View Invoices
                viewInvoices(conn, user_id);
                break;
            case 23: // Pay Invoice
                payInvoice(conn, user_id);
                break;
            case 24: // View Invoice Details
                viewInvoiceDetails(conn, user_id);
                break;
            case 25: // Add Debit Card
                addDebitCard(conn, user_id);
                break;
            case 26: // View Debit Cards
                viewDebitCards(conn, user_id);
                break;
            case 27: // Update Debit Card
                updateDebitCard(conn, user_id);
                break;                
            case 28: // Exit
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






        -- Subcategories for transactions
        CREATE TABLE subcategories (
            id SERIAL PRIMARY KEY,
            name VARCHAR(50) NOT NULL,
            category_id INT NOT NULL,
            UNIQUE (name, category_id),
            FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE
        );




        -- Insert extended list of transaction categories
        INSERT INTO categories (name) VALUES
        ('Grocery'),                  -- ID 19 (Translated "Mercado")
        ('Pet'),                      -- ID 20
        ('Vehicle');                  -- ID 21












        -- Insert subcategories for categories

        INSERT INTO subcategories (name, category_id) VALUES
        -- Food (1)
        ('Bar', 1),
        ('Restaurant', 1),
        ('Drinks', 1),
        ('Alcoholic Drinks', 1),
        ('Snacks', 1),
        ('Breakfast', 1),
        ('Lunch', 1),
        ('Afternoon Coffee', 1),
        ('Dinner', 1),
        ('Coffee', 1),
        ('Delivery', 1),
        ('Butcher', 1),
        ('Farmers Market', 1),
        ('Bakery', 1),

        -- Health (10)
        ('Gym and Fitness', 10),
        ('Well-being (spa, therapy)', 10),
        ('Health Insurance', 10),
        ('Pharmacy and Medication', 10),
        ('Appointments and Treatments', 10),
        ('Exams', 10),
        ('Dentists', 10),

        -- Transport (12)
        ('Bus', 12),
        ('Airplane', 12),
        ('Subway', 12),
        ('Train', 12),
        ('Taxi and Ride Apps', 12),

        -- Education and Development (15)
        ('Courses and Training', 15),
        ('Books and Materials', 15),
        ('School Supplies', 15),
        ('School Fees', 15),
        ('College Tuition', 15),
        ('Uniforms', 15),

        -- Pet (20)
        ('Food', 20),
        ('Bath and Grooming', 20),
        ('Medication', 20),
        ('Veterinarian', 20),
        ('Accessories', 20),

        -- Vehicle (21)
        ('Rentals', 21),
        ('Fuel', 21),
        ('Parking', 21),
        ('Leasing', 21),
        ('Car Maintenance', 21),
        ('Car Insurance', 21);






        ALTER TABLE transactions 
        ADD COLUMN subcategory_id INT REFERENCES subcategories(id) ON DELETE SET NULL;

        CREATE TABLE transf_account (
            id SERIAL PRIMARY KEY,
            from_account_id INT NOT NULL REFERENCES account(id) ON DELETE CASCADE,
            to_account_id INT NOT NULL REFERENCES account(id) ON DELETE CASCADE,
            amount DECIMAL(10, 2) NOT NULL CHECK (amount > 0),
            transfer_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            description TEXT
        );

 
        -- NEW CHANGE:about reference the income fk to account 
        ALTER TABLE income
        ADD COLUMN account_id INT REFERENCES account(id) ON DELETE SET NULL;







        -- TITLE: Add Account Type and Account Metadata Enhancements       
        CREATE TABLE account_type (
            id SERIAL PRIMARY KEY,
            name VARCHAR(50) UNIQUE NOT NULL
        );

        INSERT INTO account_type (name) VALUES
        ('Savings'),
        ('Checking'),
        ('Investment');


        -- NEW CHANGE: add foreign key account_type_id to account table
        ALTER TABLE account
        ADD COLUMN account_type_id INT REFERENCES account_type(id) ON DELETE SET NULL;


        ALTER TABLE account
        ADD COLUMN institution_name VARCHAR(100),
        ADD COLUMN agency_number VARCHAR(20),
        ADD COLUMN account_number VARCHAR(30);





        -- Spending Limits Table using NUMERIC for financial accuracy
        CREATE TABLE spending_limits (
            id SERIAL PRIMARY KEY,
            category_id INT NOT NULL REFERENCES categories(id) ON DELETE CASCADE,
            user_id INT REFERENCES users(id) ON DELETE CASCADE,
            limit_amount NUMERIC(10, 2) NOT NULL CHECK (limit_amount >= 0),
            current_value NUMERIC(10, 2) NOT NULL DEFAULT 0 CHECK (current_value >= 0),
            period VARCHAR(20) CHECK (period IN ('monthly', 'yearly')) NOT NULL DEFAULT 'monthly',
            start_date DATE NOT NULL DEFAULT CURRENT_DATE,
            is_enabled BOOLEAN NOT NULL DEFAULT TRUE
        );




        CREATE TABLE invoices (
            id SERIAL PRIMARY KEY,
            credit_card_id INTEGER REFERENCES credit_cards(id),
            user_id INTEGER REFERENCES users(id),
            invoice_date DATE NOT NULL,
            due_date DATE NOT NULL,
            total_amount DECIMAL(10, 2) NOT NULL,
            paid_amount DECIMAL(10, 2) DEFAULT 0.00,
            status VARCHAR(20) DEFAULT 'pending' CHECK (status IN ('pending', 'paid', 'overdue')),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );        




        -- about installment
        ALTER TABLE public.transactions
        ADD COLUMN is_installment boolean DEFAULT false,
        ADD COLUMN installment_number integer,
        ADD COLUMN total_installments integer,
        ADD COLUMN installment_value numeric(10,2);

        ALTER TABLE transactions ADD COLUMN original_transaction_id bigint REFERENCES transactions(id);        






        -- about debit card
        CREATE TABLE debit_cards (
            id SERIAL PRIMARY KEY,
            card_name VARCHAR(100) NOT NULL,
            account_id INTEGER REFERENCES account(id) NOT NULL,
            card_number VARCHAR(20),
            expiration_date DATE,
            cvv VARCHAR(4),
            user_id INTEGER REFERENCES users(id),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );        


        ALTER TABLE transactions ADD COLUMN debit_card_id INTEGER REFERENCES debit_cards(id);


        ALTER TABLE debit_cards
        ADD COLUMN updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP;

        -- Create trigger function to auto-update 'updated_at'
        CREATE OR REPLACE FUNCTION set_updated_at()
        RETURNS TRIGGER AS $$
        BEGIN
        NEW.updated_at = now();
        RETURN NEW;
        END;
        $$ LANGUAGE plpgsql;

        -- Drop old trigger if it exists (optional, for idempotency)
        DROP TRIGGER IF EXISTS trg_set_updated_at ON debit_cards;

        -- Create trigger to update 'updated_at' before any row update
        CREATE TRIGGER trg_set_updated_at
        BEFORE UPDATE ON debit_cards
        FOR EACH ROW
        EXECUTE FUNCTION set_updated_at();










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
















































































































































































































