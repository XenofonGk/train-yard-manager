#include <stdio.h>
#include <stddef.h>
#include "train_yard.h"
#include <stdio.h>
#include <stdlib.h>

/* Set at every refusal; read back with getLastRejectReason(). */
static int g_lastRejectReason = REJECT_NONE;

int getLastRejectReason(void)
{
    return g_lastRejectReason;
}

/* Records why a call is being refused and returns the refusal code, so each
   rejection site stays a single line. */
static int reject(int reason)
{
    g_lastRejectReason = reason;
    return 1;
}

/*
Function: addCar
Purpose: Adds a new car to the end of the train inventory.
Returns: 0 if the car was added successfully.
         1 if the train is full or invalid.
*/
int addCar(Train* train, int type, int weight)
{
    int i;
    float freightWeight = 0.0f;

    /* check if train pointer is valid */
    if (train == NULL)
        return reject(REJECT_NULL_TRAIN);

    /* check if train already has maximum cars */
    if (train->carCount >= MAX_CARS)
        return reject(REJECT_TRAIN_FULL);

    /* check if type is valid */
    if (type < TYPE_ENGINE || type > TYPE_OIL)
        return reject(REJECT_BAD_TYPE);

    /* check if weight is valid */
    if (weight <= 0)
        return reject(REJECT_BAD_WEIGHT);

    /* check if adding this car breaks total weight rule */
    if (train->totalWeight + weight > 20000)
        return reject(REJECT_TOTAL_WEIGHT);

    /* engine cars must stay at the front */
    if (type == TYPE_ENGINE)
    {
        for (i = 0; i < train->carCount; i++)
        {
            if (train->inventory[i].type != TYPE_ENGINE)
                return reject(REJECT_ENGINE_ORDER);
        }
    }
    else
    {
        /* first freight car cannot be oil */
        if (train->carCount == train->numEngines && type == TYPE_OIL)
            return reject(REJECT_OIL_FIRST_FREIGHT);

        /* wood and oil cannot be next to each other */
        if (train->carCount > 0)
        {
            int lastType = train->inventory[train->carCount - 1].type;

            if ((lastType == TYPE_WOOD && type == TYPE_OIL) ||
                (lastType == TYPE_OIL && type == TYPE_WOOD))
            {
                return reject(REJECT_WOOD_OIL_ADJACENT);
            }
        }

        /* calculate current freight weight */
        for (i = 0; i < train->carCount; i++)
        {
            if (train->inventory[i].type != TYPE_ENGINE)
                freightWeight = freightWeight + train->inventory[i].weight;
        }

        /* check pull capacity rule */
        if (freightWeight + weight > train->numEngines * 5000)
            return reject(REJECT_PULL_CAPACITY);
    }

    /* add the car at the end of the inventory */
    train->inventory[train->carCount].type = type;
    train->inventory[train->carCount].weight = (float)weight;

    /* update counts */
    train->carCount = train->carCount + 1;
    train->totalWeight = train->totalWeight + weight;

    /* if the car is an engine, update engine count */
    if (type == TYPE_ENGINE)
        train->numEngines = train->numEngines + 1;

    g_lastRejectReason = REJECT_NONE;
    return 0;
}

/*
Function: removeCar
Purpose: Removes a car from the given index and shifts remaining cars.
Returns: 0 if successful.
         1 if index is invalid or train is NULL.
*/
int removeCar(Train* train, int index)
{
    int i;
    Train temp;

    /* check if train pointer is valid */
    if (train == NULL)
        return reject(REJECT_NULL_TRAIN);

    /* check if index is valid */
    if (index < 0 || index >= train->carCount)
        return reject(REJECT_BAD_INDEX);

    /* copy current train into temp for safe checking */
    temp = *train;

    /* subtract weight of removed car */
    temp.totalWeight = temp.totalWeight - temp.inventory[index].weight;

    /* update engine count if needed */
    if (temp.inventory[index].type == TYPE_ENGINE)
        temp.numEngines = temp.numEngines - 1;

    /* shift cars left */
    for (i = index; i < temp.carCount - 1; i++)
    {
        temp.inventory[i] = temp.inventory[i + 1];
    }

    /* reduce car count */
    temp.carCount = temp.carCount - 1;

    /* train must still have at least one engine */
    if (temp.numEngines < 1)
        return reject(REJECT_LAST_ENGINE);

    /* engine cars must still remain at the front */
    for (i = 0; i < temp.carCount; i++)
    {
        if (temp.inventory[i].type == TYPE_ENGINE)
        {
            int j;
            for (j = 0; j < i; j++)
            {
                if (temp.inventory[j].type != TYPE_ENGINE)
                    return reject(REJECT_ENGINE_ORDER);
            }
        }
    }

    /* first freight car cannot be oil */
    for (i = 0; i < temp.carCount; i++)
    {
        if (temp.inventory[i].type != TYPE_ENGINE)
        {
            if (temp.inventory[i].type == TYPE_OIL)
                return reject(REJECT_OIL_FIRST_FREIGHT);
            break;
        }
    }

    /* wood and oil cannot be adjacent after removal */
    for (i = 0; i < temp.carCount - 1; i++)
    {
        if ((temp.inventory[i].type == TYPE_WOOD && temp.inventory[i + 1].type == TYPE_OIL) ||
            (temp.inventory[i].type == TYPE_OIL && temp.inventory[i + 1].type == TYPE_WOOD))
        {
            return reject(REJECT_WOOD_OIL_ADJACENT);
        }
    }

    /* if valid, copy temp back into real train */
    g_lastRejectReason = REJECT_NONE;
    *train = temp;
    return 0;
}

/*
Function: displayTrain
Purpose: Displays basic information about the train.
Returns: Nothing.
*/
void displayTrain(const Train* train)
{
    int i;

    /* check if train pointer is valid */
    if (train == NULL)
        return;

    printf("Car Count: %d\n", train->carCount);
    printf("Total Weight: %.2f\n", train->totalWeight);
    printf("Number of Engines: %d\n", train->numEngines);

    /* display each car in the train */
    for (i = 0; i < train->carCount; i++)
    {
        printf("Car %d: Type %d, Weight %.2f\n",
            i,
            train->inventory[i].type,
            train->inventory[i].weight);
    }
}

/*
Function: isTrainSafe
Purpose: Checks if the train meets safety conditions.
Returns: 1 if safe.
         0 if not safe.
*/
int isTrainSafe(const Train* train)
{
    int i;
    float freightWeight = 0.0f;
    int seenFreight = 0;

    /* check if pointer is valid */
    if (train == NULL)
        return 0;

    /* train must have at least one engine */
    if (train->numEngines < 1)
        return 0;

    /* check total weight limit */
    if (train->totalWeight > 20000)
        return 0;

    /* check engine-front rule and calculate freight weight */
    for (i = 0; i < train->carCount; i++)
    {
        if (train->inventory[i].type == TYPE_ENGINE)
        {
            if (seenFreight)
                return 0;
        }
        else
        {
            seenFreight = 1;
            freightWeight = freightWeight + train->inventory[i].weight;
        }
    }

    /* check pull capacity */
    if (freightWeight > train->numEngines * 5000)
        return 0;

    /* check wood and oil adjacency */
    for (i = 0; i < train->carCount - 1; i++)
    {
        if ((train->inventory[i].type == TYPE_WOOD && train->inventory[i + 1].type == TYPE_OIL) ||
            (train->inventory[i].type == TYPE_OIL && train->inventory[i + 1].type == TYPE_WOOD))
        {
            return 0;
        }
    }

    /* check that first freight is not oil */
    for (i = 0; i < train->carCount; i++)
    {
        if (train->inventory[i].type != TYPE_ENGINE)
        {
            if (train->inventory[i].type == TYPE_OIL)
                return 0;
            break;
        }
    }

    return 1;
}

/*
Function: displayCarsAndWeight
Purpose: Displays each car and its weight, and total weight.
Returns: Nothing.
*/
void displayCarsAndWeight(const Train* train)
{
    int i;

    /* check if train pointer is valid */
    if (train == NULL)
        return;

    /* display all cars with weight */
    for (i = 0; i < train->carCount; i++)
    {
        printf("Car %d: Type %d, Weight %.2f\n",
            i,
            train->inventory[i].type,
            train->inventory[i].weight);
    }

    printf("Total Weight: %.2f\n", train->totalWeight);
}

int getPullCapacity(const Train* train)
{
    if (train == NULL)
        return 0;

    return train->numEngines * 5000;
}
