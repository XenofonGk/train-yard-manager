#pragma once
#ifndef TRAIN_YARD_H
#define TRAIN_YARD_H

#define MAX_CARS 50
#define MAX_TRAINS 10

#define TYPE_ENGINE 0
#define TYPE_FOOD   1
#define TYPE_WOOD   2
#define TYPE_OIL    3

/* structure to store one car */
typedef struct {
    float weight;
    int type;
} Car;

/* structure to store one train */
typedef struct {
    int numEngines;
    int carCount;
    float totalWeight;
    Car inventory[MAX_CARS];
    int status;
} Train;

/*
* Adds a new car to the end of the train. The car is appended to inventory.
* Returns 0 on success; returns a non-zero value if the train is full
* (carCount >= MAX_CARS) or on other failure.
*/
int addCar(Train* train, int type, int weight);

/*
* Removes the car at the given index from the train. Indices are 0-based.
* Cars after the removed one are shifted down; carCount and totalWeight
* should be updated accordingly. Returns 0 on success; non-zero if index
* is out of range.
*/
int removeCar(Train* train, int index);

/*
* Displays a summary of the train to stdout (e.g. car count, types, and
* possibly layout). Does not modify the train. train must not be NULL.
*/
void displayTrain(const Train* train);

/*
* Checks whether the train meets safety criteria (e.g. sufficient engines,
* weight limits). Returns 1 if the train is safe, 0 if it is not safe.
* train must not be NULL.
*/
int isTrainSafe(const Train* train);

/*
* Prints each car in the train (type and weight) and the total weight.
* Useful for a detailed weight report. train must not be NULL.
*/
void displayCarsAndWeight(const Train* train);
/*
* Returns total pull capacity (engines * 5000)
*/
int getPullCapacity(const Train* train);

#endif
