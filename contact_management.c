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

