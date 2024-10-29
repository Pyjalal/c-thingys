#include <stdio.h>

int main() {
    int product_type;
    float total_cost = 0, price, weight, quantity, discount;

    while (1) {
        printf("Enter product type code (1-Fruits, 2-Vegetables, 3-Dairy, 4-Canned, 0 to Exit): ");
        scanf("%d", &product_type);

        if (product_type == 0) break;

        switch (product_type) {
            case 1:
                printf("Enter weight (kg): ");
                scanf("%f", &weight);
                printf("Enter price per kg: ");
                scanf("%f", &price);
                total_cost += weight * price;
                break;
            case 2:
                printf("Enter weight (kg): ");
                scanf("%f", &weight);
                printf("Enter price per kg: ");
                scanf("%f", &price);
                total_cost += weight * price;
                break;
            case 3:
                printf("Enter quantity: ");
                scanf("%f", &quantity);
                printf("Enter price per item: ");
                scanf("%f", &price);
                total_cost += quantity * price;
                break;
            case 4:
                printf("Enter quantity: ");
                scanf("%f", &quantity);
                printf("Enter price per item: ");
                scanf("%f", &price);
                total_cost += quantity * price;
                break;
            default:
                printf("Invalid product type!\n");
                break;
        }
    }

    printf("Enter discount percentage (0 if none): ");
    scanf("%f", &discount);
    total_cost = (discount > 0) ? total_cost * (1 - discount / 100) : total_cost;

    printf("Final cost after discount: %.2f\n", total_cost);
    return 0;
}