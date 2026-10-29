#include <stdio.h>
#include <string.h>
#include "train_yard.h"

// --- ORIGINAL UNIT TESTS ---
int test_addCar_success() {
    Train train = {0};
    addCar(&train, TYPE_ENGINE, 5000);
    int result = addCar(&train, TYPE_WOOD, 120);
    return (result == 0 && train.carCount == 2 && train.inventory[1].type == TYPE_WOOD);
}

int test_addCar_full() {
    Train train = {0};
    train.carCount = MAX_CARS;
    int result = addCar(&train, TYPE_WOOD, 120);
    return (result != 0);
}

int test_addCar_engine() {
    Train train = {0};
    int result = addCar(&train, TYPE_ENGINE, 5000);
    return (result == 0 && train.numEngines == 1);
}

// --- ACCEPTANCE TESTS ---

/* Jira Ticket: TY-001 - Engines must lead the train */
int AT01_EngineMustLeadTrain() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    int result = addCar(&t, TYPE_FOOD, 500);
    if (result != 0) return 0;
    return (t.inventory[0].type == TYPE_ENGINE && t.inventory[1].type == TYPE_FOOD);
}

/* Jira Ticket: TY-002 - No engine after freight */
int AT02_NoEngineAfterFreight() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_FOOD, 500);
    int result = addCar(&t, TYPE_ENGINE, 1000);
    return (result != 0);
}

/* Jira Ticket: TY-003 - Pull capacity formula */
int AT03_PullCapacityFormula() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_ENGINE, 1000);
    return (getPullCapacity(&t) == 10000);
}

/* Jira Ticket: TY-004 - Max system weight enforced */
int AT04_MaxSystemWeightEnforced() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 5000);
    addCar(&t, TYPE_FOOD, 5000);
    addCar(&t, TYPE_FOOD, 5000);
    addCar(&t, TYPE_FOOD, 5000);
    int result = addCar(&t, TYPE_FOOD, 1);
    return (result != 0);
}

/* Jira Ticket: TY-005 - No engine is unsafe */
int AT05_NoEngineIsUnsafe() {
    Train t = {0};
    addCar(&t, TYPE_FOOD, 500);
    return (isTrainSafe(&t) == 0);
}

/* Jira Ticket: TY-006 - First freight cannot be oil */
int AT06_FirstFreightCannotBeOil() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    int result = addCar(&t, TYPE_OIL, 500);
    return (result != 0);
}

/* Jira Ticket: TY-007 - Wood and oil cannot be adjacent */
int AT07_WoodOilCannotBeAdjacent() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_WOOD, 500);
    int result = addCar(&t, TYPE_OIL, 500);
    return (result != 0);
}

/* Jira Ticket: TY-008 - Remove blocked if it creates hazard */
int AT08_RemoveBlockedIfCreatesHazard() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_WOOD, 500);
    addCar(&t, TYPE_FOOD, 500);
    addCar(&t, TYPE_OIL, 500);
    int result = removeCar(&t, 2);
    return (result != 0);
}

/* Jira Ticket: TY-009 - Reject invalid weight */
int AT09_RejectInvalidWeight() {
    Train t = {0};
    int r1 = addCar(&t, TYPE_ENGINE, 0);
    int r2 = addCar(&t, TYPE_ENGINE, -100);
    return (r1 != 0 && r2 != 0);
}

/* Jira Ticket: TY-010 - Max cars limit */
int AT10_MaxCarsLimit() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    for (int i = 1; i < 50; i++) {
        addCar(&t, TYPE_FOOD, 10);
    }
    int result = addCar(&t, TYPE_FOOD, 10);
    return (t.carCount == 50 && result != 0);
}

/* Jira Ticket: TY-011 - Pull capacity enforced on add */
int AT11_PullCapacityEnforcedOnAdd() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000); // 5000 cap
    int result = addCar(&t, TYPE_FOOD, 5001);
    return (result != 0);
}

/* Jira Ticket: TY-012 - Minimum one engine enforced on remove */
int AT12_MinimumOneEngineEnforcedOnRemove() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    int result = removeCar(&t, 0);
    return (result != 0);
}

/* Jira Ticket: TY-013 - First freight oil enforced on remove */
int AT13_FirstFreightOilEnforcedOnRemove() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_FOOD, 500);
    addCar(&t, TYPE_OIL, 500);
    int result = removeCar(&t, 2);
    return (result != 0);
}

/* Jira Ticket: TY-014 - Engines stay at front on remove */
int AT14_EnginesStayAtFrontOnRemove() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_FOOD, 500);
    int result = removeCar(&t, 0);
    return (result == 0 && t.inventory[0].type == TYPE_ENGINE);
}

/* Pull capacity enforced on remove: two engines pull 10000, the freight is
   6000, so either engine is needed. Removing one must be refused, and the
   refusal must leave the train exactly as it was. */
int AT15_PullCapacityEnforcedOnRemove() {
    Train t = {0};
    Train before;
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_FOOD, 3000);
    addCar(&t, TYPE_FOOD, 3000);
    before = t;
    int result = removeCar(&t, 0);
    return (result != 0 &&
            getLastRejectReason() == REJECT_PULL_CAPACITY &&
            memcmp(&before, &t, sizeof t) == 0 &&
            isTrainSafe(&t) == 1);
}

// --- INTEGRATION TESTS ---
int IT01_EnginePowerValidation() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000); // 5000 cap
    addCar(&t, TYPE_FOOD, 3000);
    // Manually add car to bypass addCar pull capacity check
    t.inventory[t.carCount].type = TYPE_WOOD;
    t.inventory[t.carCount].weight = 3000;
    t.carCount++;
    t.totalWeight += 3000;
    return (isTrainSafe(&t) == 0);
}

int IT02_SafetyPersistenceAfterRemoval() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_WOOD, 500);
    addCar(&t, TYPE_FOOD, 500);
    addCar(&t, TYPE_OIL, 500);
    int res = removeCar(&t, 2); // Should fail
    return (res != 0 && isTrainSafe(&t) == 1);
}

int IT03_MultipleEnginesHeavyLoad() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 1000);
    addCar(&t, TYPE_ENGINE, 1000); // 10000 cap
    addCar(&t, TYPE_FOOD, 4000);
    addCar(&t, TYPE_WOOD, 4000); // 10000 total
    return (isTrainSafe(&t) == 1);
}

int IT04_SystemLimitEnforced() {
    Train t = {0};
    addCar(&t, TYPE_ENGINE, 5000);
    addCar(&t, TYPE_ENGINE, 5000);
    addCar(&t, TYPE_ENGINE, 5000);
    addCar(&t, TYPE_ENGINE, 5000); // 20000 total
    int res = addCar(&t, TYPE_FOOD, 1);
    return (res != 0);
}

int main() {
    int passed = 0;
    int total = 22;

    printf("--- ORIGINAL UNIT TESTS ---\n");
    if (test_addCar_success()) { printf("Original 01: PASSED\n"); passed++; } else printf("Original 01: FAILED\n");
    if (test_addCar_full()) { printf("Original 02: PASSED\n"); passed++; } else printf("Original 02: FAILED\n");
    if (test_addCar_engine()) { printf("Original 03: PASSED\n"); passed++; } else printf("Original 03: FAILED\n");

    printf("\n--- ACCEPTANCE TESTS ---\n");
    if (AT01_EngineMustLeadTrain()) { printf("AT-01 [TY-001]: PASSED\n"); passed++; } else printf("AT-01 [TY-001]: FAILED\n");
    if (AT02_NoEngineAfterFreight()) { printf("AT-02 [TY-002]: PASSED\n"); passed++; } else printf("AT-02 [TY-002]: FAILED\n");
    if (AT03_PullCapacityFormula()) { printf("AT-03 [TY-003]: PASSED\n"); passed++; } else printf("AT-03 [TY-003]: FAILED\n");
    if (AT04_MaxSystemWeightEnforced()) { printf("AT-04 [TY-004]: PASSED\n"); passed++; } else printf("AT-04 [TY-004]: FAILED\n");
    if (AT05_NoEngineIsUnsafe()) { printf("AT-05 [TY-005]: PASSED\n"); passed++; } else printf("AT-05 [TY-005]: FAILED\n");
    if (AT06_FirstFreightCannotBeOil()) { printf("AT-06 [TY-006]: PASSED\n"); passed++; } else printf("AT-06 [TY-006]: FAILED\n");
    if (AT07_WoodOilCannotBeAdjacent()) { printf("AT-07 [TY-007]: PASSED\n"); passed++; } else printf("AT-07 [TY-007]: FAILED\n");
    if (AT08_RemoveBlockedIfCreatesHazard()) { printf("AT-08 [TY-008]: PASSED\n"); passed++; } else printf("AT-08 [TY-008]: FAILED\n");
    if (AT09_RejectInvalidWeight()) { printf("AT-09 [TY-009]: PASSED\n"); passed++; } else printf("AT-09 [TY-009]: FAILED\n");
    if (AT10_MaxCarsLimit()) { printf("AT-10 [TY-010]: PASSED\n"); passed++; } else printf("AT-10 [TY-010]: FAILED\n");
    if (AT11_PullCapacityEnforcedOnAdd()) { printf("AT-11 [TY-011]: PASSED\n"); passed++; } else printf("AT-11 [TY-011]: FAILED\n");
    if (AT12_MinimumOneEngineEnforcedOnRemove()) { printf("AT-12 [TY-012]: PASSED\n"); passed++; } else printf("AT-12 [TY-012]: FAILED\n");
    if (AT13_FirstFreightOilEnforcedOnRemove()) { printf("AT-13 [TY-013]: PASSED\n"); passed++; } else printf("AT-13 [TY-013]: FAILED\n");
    if (AT14_EnginesStayAtFrontOnRemove()) { printf("AT-14 [TY-014]: PASSED\n"); passed++; } else printf("AT-14 [TY-014]: FAILED\n");
    if (AT15_PullCapacityEnforcedOnRemove()) { printf("AT-15: PASSED\n"); passed++; } else printf("AT-15: FAILED\n");

    printf("\n--- INTEGRATION TESTS ---\n");
    if (IT01_EnginePowerValidation()) { printf("IT-01: PASSED\n"); passed++; } else printf("IT-01: FAILED\n");
    if (IT02_SafetyPersistenceAfterRemoval()) { printf("IT-02: PASSED\n"); passed++; } else printf("IT-02: FAILED\n");
    if (IT03_MultipleEnginesHeavyLoad()) { printf("IT-03: PASSED\n"); passed++; } else printf("IT-03: FAILED\n");
    if (IT04_SystemLimitEnforced()) { printf("IT-04: PASSED\n"); passed++; } else printf("IT-04: FAILED\n");

    printf("\nSummary: %d/%d tests passed.\n", passed, total);
    /* A nonzero exit is what lets make, CI and the pre-push hook see a
       failure; printing it is not enough. */
    return passed == total ? 0 : 1;
}
