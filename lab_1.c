#include <stdio.h>

int main() {
    int rows, columns;
    printf("Enter the number of rows and columns: ");
    scanf("%d %d", &rows, &columns);

    // Using nested loops to print grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("*");
        }
        printf("\n");
    }

    return 0;
}