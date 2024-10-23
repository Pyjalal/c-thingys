// this a comment for a single line by just adding "//"
/* this is a also comment but you can use it for multiple lines */

/*this code is ignored by the compiler */
/* you make a comment by using  by adding "/*" to start a comment 
  and add this to end  the comment*/
/*Fun Fact:
 *You can use this to
 * block out lines of code as well
 * */
// I'm going to use this to block out lines of code so remove the comments and compile to run the code 
// and see what it does
// remove the next line
//#include <stdio.h>
// this is call to the standard input and output libray
// Think of a library as file with prepackaged for us to use
// it provides function printf("") for output to the screen and scanf("") for input from the keyboard
// this makes our lives easier and we don't have the write code for doing  these actions
// now what if we removed the # part of <studio.h>, comment out the previous #include and run the next line
// remove the comment of the next line
//include <stdio.h>
//try compiling now and see what happens :)
// Remove the multiline comment block
/*int main()
{
  
  return 0;
} 
//why do we this? 
//simple because the main function is where the where the action happens
//Our proagram logic runs in the main block
//now lets play around and create some program errors
//So what happens if we remove the curly braces
/*int main()

  return 0;
 */
//code doesn't  run because the curly braces indicate where the main function ends 
//and the where the code begins
//inside the code block enclosed by the curly braces
//each line must end by  ;
//for example you write printf, you must write like printf("");
//comment out the previous lines
/* #include <stdio.h>
    int main()
{
  printf(" Hello World");
  return 0;
}*/
//
// now what happens if we change printf("Hello World"); to printf()
// it will show error as you didn't show end of line
// Is this valid code ?
// int x= 0; float y = 0;
// try running it
// It is indeed valid code , ; indicates line break, you're just writing multiples together in one line
// What is a variable and data types?
// Best to think that a variable is a box to store data you can name and data type of the variable
// determines what you can store 
// like int can only store numbers  and can't store text
// char on the other hand holds a single character
// but how do you hold text then? if char can only hold one text character
// Simple char store multiple characters as 1d Array,
// Arrays are just  matrixes
// char will hold strings as 1d array with one 1 row and columns depending how many characters 
// are stored
// char greetings[] = "Hello World";
// printf(" %s", greetings);
// you use  %s to format characters arrays into a string
// you declare a variable like this date type variable name = data;
//
// let's now learn about print function
// printf()
// a basic function would look like this printf("")
// this prints a blank line 
// now we learn how to print a integer or a float in printf
// printf displays a string 
// so to printf a integer which a different data type
// you need to format a it first then pass it on to printf function as a string
// so you use the format symbols
// for integer put  %d and it takes the int variable outside and put into string and displays it
// for integer you can also %i as well
// you have  2 options lah
// int yourVariable = 24;
// printf(" %d", yourVariable);
// So what if you want to print multiple variables
// it takes variables in order
// int yourVariable2 = 25;
// printf(" %d %d", yourVariable, yourVariable2);
//floats are data types that contain decimals
//double are data types like float but can go to higher decimal places
//so it gives a precision but takes more space
// you can print out floats using the %f 
// you can print out double using %lf because lf stands for long float
// double six = 6.000000000000001;
// printf(" %lf " , six );
// float five = 5.0;
// printf(" %f ", five)
// you can print out   both floats and integers in the same line
// 
// printf(" %d, %f" , x ,  y);
// now let's make a simple function to make the sum of 2 numbers
/* int number1 = 2; int number2 = 2; int sum;
 * sum = number1 + number2;
 * printf("%i " , sum)*/
//now let's try to get some to get number1 and number2 from the keyboard
//so that the function is more useable
//we use scanf() for this 
//scanf("%i ", &number1);
//scanf("%i ", &number2 );
//so here it's taking value from the keyboard as string and inputing it into 
//the variable with & command to add it into the variable
//now we can see the values with 
//printf(" %i , %i", &number1, &number2);
//try running that 
//did anything strange happened?
//yeah it did  as you printed the memory address of the variable
//instead of the variable value
//just remove the & to make it show the value
//printf(" %i , %i", number1, number2);
//this prints out the number
//alright now let's move onto loops 
//these are to run a block of code multiple times
//there's 2 different types of loops
//for loops that run for a certain length
//and while loops which run as long as condition is true
//let's update the sum program to sum up all the numbers until a certain number
//this code will use for loops
/*int mySum(int num) {
    int running_sum = 0;
    for (int i = 1; i <= num; ++i) {
        running_sum += i;
    }
    return running_sum;
}

 *
 * */
// so I'll explain how this for loop works
// as usual the {} curly braces indicate a block of code
// there's 3 lines in the for loop inside the brackets () each lines ends in ;
// first line declares the variable as int and sets it to one
// next it continues until the number is less than or equals to num
// then the next line i++, increases the number by one
//so as the loop iterates as the it increases the value of i by one 
//for every time the code block is runned
//so the for loop is known as an iterative loop
//


