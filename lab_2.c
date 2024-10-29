#include <stdio.h>

int main() {
    int nums[100], count = 0, sum = 0, product = 1;
    float average;
    int min, max;

    printf("Enter integers separated by spaces (at least three): ");
    while (scanf("%d", &nums[count]) == 1) count++; // Read input until non-integer
    
    sum = nums[0];
    product = nums[0];
    min = nums[0];
    max = nums[0];

    for (int i = 1; i < count; i++) {
        sum += nums[i];
        product *= nums[i];
        min = (nums[i] < min) ? nums[i] : min;
        max = (nums[i] > max) ? nums[i] : max;
    }

    average = (float)sum / count;
    
    printf("Sum: %d\nAverage: %.2f\nProduct: %d\nMin: %d\nMax: %d\n", sum, average, product, min, max);
    return 0;
}