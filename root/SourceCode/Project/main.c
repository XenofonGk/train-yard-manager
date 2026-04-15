
#include <stdio.h>
#include "train_yard.h"

int main(void)
{
    Train train = { 0 };
    char choice = ' ';
    int type;
    int weight;
    int index;

    printf("Welcome to the Train Yard Management System (Mac/Linux Edition)\n");

    while (choice != 'Q' && choice != 'q')
    {
        printf("\n[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: ");
        if (scanf(" %c", &choice) != 1) break;

        switch (choice)
        {
        case 'A':
        case 'a':
        {
            printf("Enter type (0=Engine, 1=Food, 2=Wood, 3=Oil): ");
            if (scanf("%d", &type) != 1) break;

            printf("Enter weight: ");
            if (scanf("%d", &weight) != 1) break;

            if (addCar(&train, type, weight) == 0)
            {
                printf("Car added successfully.\n");
            }
            else
            {
                printf("Failed to add car.\n");
            }
            break;
        }

        case 'R':
        case 'r':
        {
            printf("Enter index to remove: ");
            if (scanf("%d", &index) != 1) break;

            if (removeCar(&train, index) == 0)
            {
                printf("Car removed successfully.\n");
            }
            else
            {
                printf("Failed to remove car.\n");
            }
            break;
        }

        case 'V':
        case 'v':
            printf(">> Display Train\n");
            displayTrain(&train);
            break;

        case 'W':
        case 'w':
            printf(">> Show train cars and weight\n");
            displayCarsAndWeight(&train);
            break;

        case 'S':
        case 's':
            if (isTrainSafe(&train))
            {
                printf(">> Status: SAFE\n");
            }
            else
            {
                printf(">> Status: NOT SAFE\n");
            }
            break;

        case 'Q':
        case 'q':
            printf("Exiting program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }

    return 0;
}
