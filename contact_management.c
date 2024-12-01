/* Contact Management System */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CONTACTS 100         // Maximum number of contacts
#define NAME_LENGTH 50           // Maximum length for name
#define PHONE_LENGTH 15          // Maximum length for phone number
#define EMAIL_LENGTH 50          // Maximum length for email
#define FILENAME "contacts.txt"  // File to save and load contacts
#define BATCH_FILENAME "batch_contacts.txt" // File for batch operations
#define KEY 5                    // Encryption key for Caesar cipher

// Structure to hold contact information
typedef struct {
    char name[NAME_LENGTH];
    char phone[PHONE_LENGTH];
    char email[EMAIL_LENGTH];
} Contact;

// Global array to store contacts and a counter
Contact contactList[MAX_CONTACTS];
int contactCount = 0;

// Function prototypes
void loadContactsFromFile();
void saveContactsToFile();
void addContact();
void displayContacts();
void searchContact();
void deleteContact();
void sortContacts();
void validateInput(char *input, int length);
int validateEmail(const char *email);
int validatePhoneNumber(const char *phone);
void clearInputBuffer();
void displayMenu();
void advancedSearch();
void batchAddContacts();
void encryptData(char *data);
void decryptData(char *data);

int main() {
    int choice;

    // Load contacts from file at the start
    loadContactsFromFile();

    while (1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer(); // Clear newline character from input buffer

        switch (choice) {
            case 1:
                addContact();
                break;
            case 2:
                displayContacts();
                break;
            case 3:
                searchContact();
                break;
            case 4:
            case 5:
                sortContacts();
                break;
            case 6:
                advancedSearch();
                break;
            case 7:
                batchAddContacts();
                break;
            case 8:
                saveContactsToFile();
                printf("Contacts saved to %s successfully.\n", FILENAME);
                break;
            case 9:
                saveContactsToFile();
                printf("Exiting program. Contacts saved to %s.\n", FILENAME);
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}

/**
 * Display the main menu options to the user.
 */
void displayMenu() {
    printf("\n--- Contact Management System ---\n");
    printf("1. Add New Contact\n");
    printf("2. Display All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Sort Contacts\n");
    printf("6. Advanced Search\n");
    printf("7. Batch Add Contacts\n");
    printf("8. Save Contacts\n");
    printf("9. Exit\n");
}

/**
 * Clear the input buffer to remove any leftover characters.
 */
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/**
 * Load contacts from the file into the contact list.
 * Decrypts sensitive data after loading.
 */
void loadContactsFromFile() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        // File does not exist, no contacts to load
        return;
    }

    while (fscanf(file, "%49[^,],%14[^,],%49[^\n]\n",
                  contactList[contactCount].name,
                  contactList[contactCount].phone,
                  contactList[contactCount].email) != EOF) {
        // Decrypt sensitive data
        decryptData(contactList[contactCount].phone);
        decryptData(contactList[contactCount].email);
        contactCount++;
    }

    fclose(file);
}

/**
 * Save contacts from the contact list into the file.
 * Encrypts sensitive data before saving.
 */
void saveContactsToFile() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        // Encrypt sensitive data
        encryptData(contactList[i].phone);
        encryptData(contactList[i].email);

        fprintf(file, "%s,%s,%s\n",
                contactList[i].name,
                contactList[i].phone,
                contactList[i].email);

        // Decrypt back after saving
        decryptData(contactList[i].phone);
        decryptData(contactList[i].email);
    }

    fclose(file);
}

/**
 * Add a new contact to the contact list.
 */
void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Contact list is full. Cannot add more contacts.\n");
        return;
    }

    Contact newContact;

    printf("Enter Name: ");
    fgets(newContact.name, NAME_LENGTH, stdin);
    validateInput(newContact.name, NAME_LENGTH);

    printf("Enter Phone Number: ");
    fgets(newContact.phone, PHONE_LENGTH, stdin);
    validateInput(newContact.phone, PHONE_LENGTH);

    if (!validatePhoneNumber(newContact.phone)) {
        printf("Invalid phone number format.\n");
        return;
    }

    printf("Enter Email: ");
    fgets(newContact.email, EMAIL_LENGTH, stdin);
    validateInput(newContact.email, EMAIL_LENGTH);

    if (!validateEmail(newContact.email)) {
        printf("Invalid email format.\n");
        return;
    }

    contactList[contactCount++] = newContact;
    printf("Contact added successfully.\n");
}

/**
 * Display all contacts in the contact list.
 */
void displayContacts() {
    if (contactCount == 0) {
        printf("No contacts to display.\n");
        return;
    }

    printf("\n--- Contact List ---\n");
    for (int i = 0; i < contactCount; i++) {
        printf("Contact %d:\n", i + 1);
        printf(" Name: %s\n", contactList[i].name);
        printf(" Phone: %s\n", contactList[i].phone);
        printf(" Email: %s\n", contactList[i].email);
    }
}

/**
 * Search for a contact by name, phone, or email.
 */
void searchContact() {
    char searchTerm[NAME_LENGTH];
    int found = 0;

    printf("Enter the term to search: ");
    fgets(searchTerm, NAME_LENGTH, stdin);
    validateInput(searchTerm, NAME_LENGTH);

    for (int i = 0; i < contactCount; i++) {
        if (strcasecmp(contactList[i].name, searchTerm) != NULL ||
            strcasecmp(contactList[i].phone, searchTerm) != NULL ||
            strcasecpm(contactList[i].email, searchTerm) != NULL) {
            printf("Contact found:\n");
            printf(" Name: %s\n", contactList[i].name);
            printf(" Phone: %s\n", contactList[i].phone);
            printf(" Email: %s\n", contactList[i].email);
            found = 1;
        }
    }

    if (!found) {
        printf("No matching contacts found.\n");
    }
}

/**
 * Delete a contact by name.
 */
void deleteContact() {
    char deleteName[NAME_LENGTH];
    int found = 0;

    printf("Enter the name of the contact to delete: ");
    fgets(deleteName, NAME_LENGTH, stdin);
    validateInput(deleteName, NAME_LENGTH);

    for (int i = 0; i < contactCount; i++) {
        if (strcasecmp(contactList[i].name, deleteName) == 0) {
            // Shift contacts to fill the gap
            for (int j = i; j < contactCount - 1; j++) {
                contactList[j] = contactList[j + 1];
            }
            contactCount--;
            printf("Contact deleted successfully.\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Contact not found.\n");
    }
}

/**
 * Sort contacts by name using bubble sort algorithm.
 */
void sortContacts() {
    if (contactCount < 2) {
        printf("Not enough contacts to sort.\n");
        return;
    }

    for (int i = 0; i < contactCount - 1; i++) {
        for (int j = 0; j < contactCount - i - 1; j++) {
            if (strcasecmp(contactList[j].name, contactList[j + 1].name) > 0) {
                // Swap contacts
                Contact temp = contactList[j];
                contactList[j] = contactList[j + 1];
                contactList[j + 1] = temp;
            }
        }
    }

    printf("Contacts sorted by name successfully.\n");
}

/**
 * Perform advanced search with options.
 */
/**void advancedSearch() {
    int choice;
    char searchTerm[NAME_LENGTH];
    int found = 0;

    printf("\n--- Advanced Search ---\n");
    printf("1. Search by Partial Name\n");
    printf("2. Search by Phone Number\n");
    printf("3. Search by Email\n");
    printf("4. Search by Multiple Fields\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    clearInputBuffer();

    switch (choice) {
        case 1:
            printf("Enter partial name to search: ");
            fgets(searchTerm, NAME_LENGTH, stdin);
            validateInput(searchTerm, NAME_LENGTH);
            for (int i = 0; i < contactCount; i++) {
                if (strcasestr(contactList[i].name, searchTerm) != NULL) {
                    printf("Contact found:\n");
                    printf(" Name: %s\n", contactList[i].name);
                    printf(" Phone: %s\n", contactList[i].phone);
                    printf(" Email: %s\n", contactList[i].email);
                    found = 1;
                }
            }
            break;
        case 2:
            printf("Enter phone number to search: ");
            fgets(searchTerm, PHONE_LENGTH, stdin);
            validateInput(searchTerm, PHONE_LENGTH);
            for (int i = 0; i < contactCount; i++) {
                if (strcasestr(contactList[i].phone, searchTerm) != NULL) {
                    printf("Contact found:\n");
                    printf(" Name: %s\n", contactList[i].name);
                    printf(" Phone: %s\n", contactList[i].phone);
                    printf(" Email: %s\n", contactList[i].email);
                    found = 1;
                }
            }
            break;
        case 3:
            printf("Enter email to search: ");
            fgets(searchTerm, EMAIL_LENGTH, stdin);
            validateInput(searchTerm, EMAIL_LENGTH);
            for (int i = 0; i < contactCount; i++) {
                if (strcasestr(contactList[i].email, searchTerm) != NULL) {
                    printf("Contact found:\n");
                    printf(" Name: %s\n", contactList[i].name);
                    printf(" Phone: %s\n", contactList[i].phone);
                    printf(" Email: %s\n", contactList[i].email);
                    found = 1;
                }
            }
            break;
        case 4:
            printf("Enter term to search in all fields: ");
            fgets(searchTerm, NAME_LENGTH, stdin);
            validateInput(searchTerm, NAME_LENGTH);
            for (int i = 0; i < contactCount; i++) {
                if (strcasestr(contactList[i].name, searchTerm) != NULL ||
                    strcasestr(contactList[i].phone, searchTerm) != NULL ||
                    strcasestr(contactList[i].email, searchTerm) != NULL) {
                    printf("Contact found:\n");
                    printf(" Name: %s\n", contactList[i].name);
                    printf(" Phone: %s\n", contactList[i].phone);
                    printf(" Email: %s\n", contactList[i].email);
                    found = 1;
                }
            }
            break;
        default:
            printf("Invalid choice.\n");
            return;
    }

    if (!found) {
        printf("No matching contacts found.\n");
    }
}
**/
/**
 * Batch add contacts from a formatted text file.
 */
void batchAddContacts() {
    FILE *file = fopen(BATCH_FILENAME, "r");
    if (file == NULL) {
        printf("Batch file %s not found.\n", BATCH_FILENAME);
        return;
    }

    int addedCount = 0;
    Contact tempContact;

    while (fscanf(file, "%49[^,],%14[^,],%49[^\n]\n",
                  tempContact.name,
                  tempContact.phone,
                  tempContact.email) != EOF) {
        if (contactCount >= MAX_CONTACTS) {
            printf("Contact list is full. Cannot add more contacts.\n");
            break;
        }

        // Validate data
        if (!validatePhoneNumber(tempContact.phone) || !validateEmail(tempContact.email)) {
            printf("Invalid data for contact: %s. Skipping.\n", tempContact.name);
            continue;
        }

        contactList[contactCount++] = tempContact;
        addedCount++;
    }

    fclose(file);
    printf("Batch add complete. %d contacts added.\n", addedCount);
}

/**
 * Validate and remove newline character from input string.
 */
void validateInput(char *input, int length) {
    size_t ln = strlen(input) - 1;
    if (input[ln] == '\n') {
        input[ln] = '\0';
    } else {
        // Clear the input buffer if input is too long
        clearInputBuffer();
    }
}

/**
 * Validate the email format.
 * Simple validation to check for presence of '@' and '.' characters.
 */
int validateEmail(const char *email) {
    const char *atSign = strchr(email, '@');
    if (atSign == NULL) {
        return 0;
    }

    const char *dot = strchr(atSign, '.');
    if (dot == NULL) {
        return 0;
    }

    return 1;
}

/**
 * Validate the phone number format.
 * Ensure it contains only digits and has acceptable length.
 */
int validatePhoneNumber(const char *phone) {
    int length = strlen(phone);
    for (int i = 0; i < length; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+' && phone[i] != '-' && phone[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/**
 * Encrypt data using a simple Caesar cipher.
 */
void encryptData(char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] += KEY;
    }
}

/**
 * Decrypt data using a simple Caesar cipher.
 */
void decryptData(char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] -= KEY;
    }
}
