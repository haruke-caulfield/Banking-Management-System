/*
 * BANKING MANAGEMENT SYSTEM
 * Language: C
 * Concepts: Structures, Functions, Loops, Conditional Statements,
 *           and File Handling.
 *
 * Account records are stored in "accounts.dat" in the same folder
 * as the program. This is a simple educational console project,
 * not a real banking application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "accounts.dat"
#define NAME_LENGTH 50

typedef struct {
    long long accountNumber;
    char name[NAME_LENGTH];
    double balance;
} Account;

void clearInputBuffer(void);
void createAccount(void);
void depositMoney(void);
void withdrawMoney(void);
void checkBalance(void);
void displayAllAccounts(void);
int accountExists(long long accountNumber);

int main(void) {
    int choice;

    do {
        printf("\n====================================\n");
        printf("       BANKING MANAGEMENT SYSTEM\n");
        printf("====================================\n");
        printf("1. Create New Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Check Account Balance\n");
        printf("5. Display All Accounts\n");
        printf("6. Exit\n");
        printf("------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                depositMoney();
                break;
            case 3:
                withdrawMoney();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                displayAllAccounts();
                break;
            case 6:
                printf("\nThank you for using the Banking Management System!\n");
                break;
            default:
                printf("\nInvalid choice. Please select 1 to 6.\n");
        }
    } while (choice != 6);

    return 0;
}

/* Removes leftover characters from the input line. */
void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard */
    }
}

/* Checks whether an account number is already present. */
int accountExists(long long accountNumber) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Account account;

    if (fp == NULL) {
        return 0; /* File does not exist yet. */
    }

    while (fread(&account, sizeof(Account), 1, fp) == 1) {
        if (account.accountNumber == accountNumber) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Creates and saves a new account. */
void createAccount(void) {
    FILE *fp;
    Account account;

    printf("\n--- CREATE NEW ACCOUNT ---\n");

    printf("Enter account number: ");
    if (scanf("%lld", &account.accountNumber) != 1 ||
        account.accountNumber <= 0) {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (accountExists(account.accountNumber)) {
        printf("This account number already exists.\n");
        return;
    }

    printf("Enter account holder name: ");
    if (fgets(account.name, sizeof(account.name), stdin) == NULL) {
        printf("Unable to read name.\n");
        return;
    }
    account.name[strcspn(account.name, "\n")] = '\0';

    if (strlen(account.name) == 0) {
        printf("Name cannot be empty.\n");
        return;
    }

    printf("Enter opening balance: ");
    if (scanf("%lf", &account.balance) != 1 || account.balance < 0) {
        printf("Invalid opening balance.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fp = fopen(FILE_NAME, "ab");
    if (fp == NULL) {
        printf("Error: Unable to open account file.\n");
        return;
    }

    if (fwrite(&account, sizeof(Account), 1, fp) == 1) {
        printf("\nAccount created successfully!\n");
    } else {
        printf("\nError while saving account details.\n");
    }

    fclose(fp);
}

/* Adds money to an existing account and updates the file. */
void depositMoney(void) {
    FILE *fp;
    Account account;
    long long number;
    double amount;
    int found = 0;

    printf("\n--- DEPOSIT MONEY ---\n");
    printf("Enter account number: ");

    if (scanf("%lld", &number) != 1) {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }

    printf("Enter amount to deposit: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("Please enter a valid amount greater than zero.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("No account records found.\n");
        return;
    }

    while (fread(&account, sizeof(Account), 1, fp) == 1) {
        if (account.accountNumber == number) {
            account.balance += amount;

            /* Move back to the beginning of this record and update it. */
            fseek(fp, -(long)sizeof(Account), SEEK_CUR);

            if (fwrite(&account, sizeof(Account), 1, fp) == 1) {
                printf("Deposit successful!\n");
                printf("Updated balance: %.2f\n", account.balance);
            } else {
                printf("Error updating account record.\n");
            }

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Account not found.\n");
    }

    fclose(fp);
}

/* Withdraws money if the account has sufficient balance. */
void withdrawMoney(void) {
    FILE *fp;
    Account account;
    long long number;
    double amount;
    int found = 0;

    printf("\n--- WITHDRAW MONEY ---\n");
    printf("Enter account number: ");

    if (scanf("%lld", &number) != 1) {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }

    printf("Enter amount to withdraw: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("Please enter a valid amount greater than zero.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("No account records found.\n");
        return;
    }

    while (fread(&account, sizeof(Account), 1, fp) == 1) {
        if (account.accountNumber == number) {
            found = 1;

            if (amount > account.balance) {
                printf("Insufficient balance. Withdrawal cancelled.\n");
            } else {
                account.balance -= amount;

                fseek(fp, -(long)sizeof(Account), SEEK_CUR);

                if (fwrite(&account, sizeof(Account), 1, fp) == 1) {
                    printf("Withdrawal successful!\n");
                    printf("Remaining balance: %.2f\n", account.balance);
                } else {
                    printf("Error updating account record.\n");
                }
            }
            break;
        }
    }

    if (!found) {
        printf("Account not found.\n");
    }

    fclose(fp);
}

/* Displays the details and balance of one account. */
void checkBalance(void) {
    FILE *fp;
    Account account;
    long long number;
    int found = 0;

    printf("\n--- CHECK ACCOUNT BALANCE ---\n");
    printf("Enter account number: ");

    if (scanf("%lld", &number) != 1) {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No account records found.\n");
        return;
    }

    while (fread(&account, sizeof(Account), 1, fp) == 1) {
        if (account.accountNumber == number) {
            printf("\nAccount Number : %lld\n", account.accountNumber);
            printf("Account Holder : %s\n", account.name);
            printf("Available Balance: %.2f\n", account.balance);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Account not found.\n");
    }

    fclose(fp);
}

/* Displays every account saved in the file. */
void displayAllAccounts(void) {
    FILE *fp;
    Account account;
    int count = 0;

    printf("\n--- ALL BANK ACCOUNTS ---\n");

    fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No account records found.\n");
        return;
    }

    printf("\n%-18s %-25s %s\n", "Account Number", "Account Holder", "Balance");
    printf("---------------------------------------------------------------\n");

    while (fread(&account, sizeof(Account), 1, fp) == 1) {
        printf("%-18lld %-25s %.2f\n",
               account.accountNumber, account.name, account.balance);
        count++;
    }

    if (count == 0) {
        printf("No accounts available.\n");
    }

    fclose(fp);
}
