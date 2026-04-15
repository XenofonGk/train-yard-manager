#include "pch.h"
#include "CppUnitTest.h"

#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>

extern "C" {
#include "train_yard.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(WhiteBoxTest)
	{
	public:

        TEST_METHOD(WB001_AddCar_NullPointerBranch) {
            Train* train = NULL;

            int result = addCar(train, TYPE_FOOD, 2000);

            Assert::AreEqual(1, result);
        }

        TEST_METHOD(WB002_AddCar_BoundaryCheck) {
            Train train = {};
            train.carCount = 50; // MAX_CARS boundary [1]
            train.numEngines = 1;
            train.totalWeight = 10000;

            int result = addCar(&train, TYPE_FOOD, 2000);

            Assert::AreEqual(1, result);

            Assert::AreEqual(50, train.carCount);
        }

		/*
		Test: addCar engine branch
		Purpose: Verify numEngines increases when adding an engine car.
		Expected Result:
		- Function returns 0
		- carCount increases to 1
		- numEngines increases to 1
		*/
		TEST_METHOD(WB003_TestAddCarEngineIncrementsNumEngines)
		{
			Train train = {};
			train.carCount = 0;
			train.totalWeight = 0;
			train.numEngines = 0;

			int result = addCar(&train, TYPE_ENGINE, 5000);

			Assert::AreEqual(0, result);
			Assert::AreEqual(1, train.carCount);
			Assert::AreEqual(1, train.numEngines);
			Assert::AreEqual(TYPE_ENGINE, train.inventory[0].type);
			Assert::AreEqual((float)5000, train.inventory[0].weight);
		}

		/*
		Test: addCar freight branch
		Purpose: Verify numEngines does not increase when adding a freight car.
		Expected Result:
		- Function returns 0
		- carCount increases to 1
		- numEngines remains 0
		*/
		TEST_METHOD(WB004_TestAddCarFreightDoesNotIncrementNumEngines)
		{
			Train train = {};
			train.carCount = 0;
			train.totalWeight = 0;
			train.numEngines = 0;

			int result = addCar(&train, TYPE_FOOD, 2000);

			Assert::AreEqual(0, result);
			Assert::AreEqual(1, train.carCount);
			Assert::AreEqual(0, train.numEngines);
			Assert::AreEqual(TYPE_FOOD, train.inventory[0].type);
			Assert::AreEqual((float)2000, train.inventory[0].weight);
		}

        TEST_METHOD(WB005_RemoveCar_NullPointerGuard)
        {
            int result = removeCar(NULL, 0);
            Assert::AreEqual(1, result);
        }

        TEST_METHOD(WB006_RemoveCar_IndexBoundaryPath)
        {
            Train train = {};
            train.carCount = 1; // Train has one car (index 0)

            int result = removeCar(&train, 5); // Attempt to remove index 5

            Assert::AreEqual(1, result);
        }

        TEST_METHOD(WB007_DisplayTrain_NullGuardPath)
        {
            // Since this function is void, we test that it handles NULL without crashing
            displayTrain(NULL);
        }

        TEST_METHOD(WB008_DisplayTrain_ValidTraversal)
        {
            Train train = {};
            train.carCount = 2;

            train.inventory[0].type = TYPE_ENGINE;
            train.inventory[0].weight = 5000.0f;

            train.inventory[1].type = TYPE_FOOD;
            train.inventory[1].weight = 2000.0f;

            displayTrain(&train);
        }

        TEST_METHOD(WB009_IsTrainSafe_NullGuardReturn)
        {
            int result = isTrainSafe(NULL);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(WB010_IsTrainSafe_EngineCheckBranch)
        {
            Train train = {};
            train.numEngines = 0; // Violates safety rule [5]
            train.carCount = 1;
            train.inventory[0].type = TYPE_FOOD;

            int result = isTrainSafe(&train);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(WB011_DisplayCarsAndWeight_NullGuard)
        {
            displayCarsAndWeight(NULL);
        }

        TEST_METHOD(WB012_DisplayCarsAndWeight_LoopLogic)
        {
            Train train = {};
            train.carCount = 1;
            train.totalWeight = 2000;
            train.inventory[0].type = TYPE_FOOD;
            train.inventory[0].weight = 2000;

            displayCarsAndWeight(&train);
        }

	};


    TEST_CLASS(BlackBoxTests)
    {
    private:

        static void InitTrain(Train& train)
        {
            memset(&train, 0, sizeof(Train));
        }

        static void AddOrFail(Train& train, int type, int weight)
        {
            int rc = addCar(&train, type, weight);
            Assert::AreEqual(0, rc, L"Setup failed while adding a car.");
        }

        static std::string CaptureDisplayCarsAndWeight(const Train& train)
        {
            const char* fileName = "test_output.txt";

            FILE* dummy;
            freopen_s(&dummy, fileName, "w", stdout);

            displayCarsAndWeight(&train);
            fflush(stdout);

            freopen_s(&dummy, "CONOUT$", "w", stdout);

            std::ifstream in(fileName);
            std::stringstream buffer;
            buffer << in.rdbuf();
            in.close();

            remove(fileName);

            return buffer.str();
        }

    public:

        TEST_METHOD(BB001_AddEngine_Success)
        {
            Train train;
            InitTrain(train);

            int result = addCar(&train, TYPE_ENGINE, 5000);

            Assert::AreEqual(0, result);
            Assert::AreEqual(1, train.carCount);
            Assert::AreEqual(1, train.numEngines);
            Assert::AreEqual((float)5000, train.totalWeight);
            Assert::AreEqual(TYPE_ENGINE, train.inventory[0].type);
            Assert::AreEqual((float)5000, train.inventory[0].weight);
        }

        TEST_METHOD(BB002_LeadRuleViolation_AddEngineAfterFreight_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_FOOD, 2000);
            int result = addCar(&train, TYPE_ENGINE, 5000);

            Assert::AreNotEqual(0, result, L"Spec says engine must be at the beginning.");
        }

        TEST_METHOD(BB003_Exceed20000Weight_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 4000);
            AddOrFail(train, TYPE_WOOD, 4000);

            int result = addCar(&train, TYPE_FOOD, 3000);

            Assert::AreNotEqual(0, result, L"Spec says total weight cannot exceed 20,000 kg.");
        }

        TEST_METHOD(BB004_OilCannotBeFirstAfterEngines_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            int result = addCar(&train, TYPE_OIL, 2000);

            Assert::AreNotEqual(0, result, L"Spec says OIL cannot be the first car after engine(s).");
        }

        TEST_METHOD(BB005_RemoveMiddleCar_Success_ShiftDown)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_WOOD, 3000);

            int result = removeCar(&train, 1);

            Assert::AreEqual(0, result);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual(TYPE_ENGINE, train.inventory[0].type);
            Assert::AreEqual(TYPE_WOOD, train.inventory[1].type);
            Assert::AreEqual((float)8000, train.totalWeight);
        }

        TEST_METHOD(BB006_AdjacencyRule_RemoveFoodBetweenWoodAndOil_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_WOOD, 3000);
            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_OIL, 1500);

            int result = removeCar(&train, 2);

            Assert::AreNotEqual(0, result, L"Spec says removing Food would make WOOD and OIL adjacent.");
        }

        TEST_METHOD(BB007_MinimumEngineRule_RemoveLastEngine_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_WOOD, 3000);

            int result = removeCar(&train, 0);

            Assert::AreNotEqual(0, result, L"Spec says train must have at least one engine.");
        }

        TEST_METHOD(BB008_IndexOutOfRange_Remove_ShouldFail)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 2000);

            int result = removeCar(&train, 5);

            Assert::AreNotEqual(0, result);
        }

        TEST_METHOD(BB009_SafeTrainConfiguration_ShouldBeSafe)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_WOOD, 3000);

            int result = isTrainSafe(&train);

            Assert::AreEqual(1, result);
        }

        TEST_METHOD(BB010_NoEngineRule_ShouldBeUnsafe)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_WOOD, 3000);

            int result = isTrainSafe(&train);

            Assert::AreEqual(0, result);
        }

        TEST_METHOD(BB011_WeightLimitRule_22000_ShouldBeUnsafe)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 6000);
            AddOrFail(train, TYPE_WOOD, 6000);

            int result = isTrainSafe(&train);

            Assert::AreEqual(0, result, L"Spec says train exceeding 20,000 kg is unsafe.");
        }

        TEST_METHOD(BB012_MinimumValidTrain_OneEngineOnly_ShouldBeSafe)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);

            int result = isTrainSafe(&train);

            Assert::AreEqual(1, result);
        }

        TEST_METHOD(BB013_DisplayCarsAndWeight_MultipleCars)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_WOOD, 3000);

            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("Car 1") != std::string::npos);
            Assert::IsTrue(output.find("Car 2") != std::string::npos);
            Assert::IsTrue(output.find("10000") != std::string::npos);
        }

        TEST_METHOD(BB014_DisplayEmptyTrain)
        {
            Train train;
            InitTrain(train);

            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::IsTrue(output.find("0") != std::string::npos);
        }

        TEST_METHOD(BB015_DisplaySingleCar)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);

            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("5000") != std::string::npos);
        }

        TEST_METHOD(BB016_DisplayManyCars)
        {
            Train train;
            InitTrain(train);

            AddOrFail(train, TYPE_ENGINE, 5000);
            AddOrFail(train, TYPE_FOOD, 2000);
            AddOrFail(train, TYPE_WOOD, 3000);
            AddOrFail(train, TYPE_OIL, 1500);

            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("Car 1") != std::string::npos);
            Assert::IsTrue(output.find("Car 2") != std::string::npos);
            Assert::IsTrue(output.find("Car 3") != std::string::npos);
            Assert::IsTrue(output.find("11500") != std::string::npos);
        }
    };

	TEST_CLASS(AcceptanceTests)
	{
	public:
		/*
		Jira Ticket: TY-001
		Description: Verify that engines must be at the front of the train and freight cars follow them.
		*/
		TEST_METHOD(AT01_EngineMustLeadTrain)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			int result = addCar(&t, TYPE_FOOD, 500);
			Assert::AreEqual(0, result);
			Assert::AreEqual((int)TYPE_ENGINE, (int)t.inventory[0].type);
			Assert::AreEqual((int)TYPE_FOOD, (int)t.inventory[1].type);
		}

		/*
		Jira Ticket: TY-002
		Description: Verify that an engine cannot be added after a freight car has already been added to the train.
		*/
		TEST_METHOD(AT02_NoEngineAfterFreight)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000); // Need engine first so freight can be added
			addCar(&t, TYPE_FOOD, 500);
			int result = addCar(&t, TYPE_ENGINE, 1000);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-003
		Description: Verify that the pull capacity formula correctly calculates total capacity as number of engines multiplied by 5000 kg.
		*/
		TEST_METHOD(AT03_PullCapacityFormula)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_ENGINE, 1000);
			int expected = 2 * 5000;
			Assert::AreEqual(expected, getPullCapacity(&t));
		}

		/*
		Jira Ticket: TY-004
		Description: Verify that the system-wide maximum weight limit of 20,000 kg is strictly enforced when adding new cars.
		*/
		TEST_METHOD(AT04_MaxSystemWeightEnforced)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 5000);
			addCar(&t, TYPE_FOOD, 5000);
			addCar(&t, TYPE_FOOD, 5000);
			addCar(&t, TYPE_FOOD, 5000); // total = 20,000
			int result = addCar(&t, TYPE_FOOD, 1); // would exceed
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-005
		Description: Verify that a train without at least one engine is considered unsafe for operation.
		*/
		TEST_METHOD(AT05_NoEngineIsUnsafe)
		{
			Train t = {};
			addCar(&t, TYPE_FOOD, 500);
			int result = isTrainSafe(&t);
			Assert::AreEqual(0, result); // not safe
		}

		/*
		Jira Ticket: TY-006
		Description: Verify that OIL cars cannot be the first freight car immediately following the engines for safety reasons.
		*/
		TEST_METHOD(AT06_FirstFreightCannotBeOil)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			int result = addCar(&t, TYPE_OIL, 500);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-007
		Description: Verify that WOOD and OIL cars cannot be placed adjacent to each other in the train inventory.
		*/
		TEST_METHOD(AT07_WoodOilCannotBeAdjacent)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_WOOD, 500);
			int result = addCar(&t, TYPE_OIL, 500); // would be adjacent to WOOD
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-008
		Description: Verify that removing a car is blocked if it would result in WOOD and OIL cars becoming adjacent.
		*/
		TEST_METHOD(AT08_RemoveBlockedIfCreatesHazard)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_WOOD, 500);
			addCar(&t, TYPE_FOOD, 500); // buffer
			addCar(&t, TYPE_OIL, 500);
			// removing FOOD (index 2) makes WOOD and OIL adjacent
			int result = removeCar(&t, 2);
			Assert::AreNotEqual(0, result);
			Assert::AreEqual(4, t.carCount); // train unchanged
		}

		/*
		Jira Ticket: TY-009
		Description: Verify that adding a car with zero or negative weight is rejected by the system.
		*/
		TEST_METHOD(AT09_RejectInvalidWeight)
		{
			Train t = {};
			int result = addCar(&t, TYPE_ENGINE, 0);
			Assert::AreNotEqual(0, result);
			result = addCar(&t, TYPE_ENGINE, -100);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-010
		Description: Verify that the train cannot exceed the maximum capacity of 50 cars.
		*/
		TEST_METHOD(AT10_MaxCarsLimit)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			for (int i = 1; i < 50; i++) {
				addCar(&t, TYPE_FOOD, 10);
			}
			Assert::AreEqual(50, t.carCount);
			int result = addCar(&t, TYPE_FOOD, 10);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-011
		Description: Verify that freight load cannot exceed the total pull capacity of the engines during addition.
		*/
		TEST_METHOD(AT11_PullCapacityEnforcedOnAdd)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000); // 5000 capacity
			int result = addCar(&t, TYPE_FOOD, 5001);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-012
		Description: Verify that removing the only engine from a train is blocked.
		*/
		TEST_METHOD(AT12_MinimumOneEngineEnforcedOnRemove)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			int result = removeCar(&t, 0);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-013
		Description: Verify that removing a car is blocked if it would result in OIL becoming the first freight car.
		*/
		TEST_METHOD(AT13_FirstFreightOilEnforcedOnRemove)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_FOOD, 500);
			addCar(&t, TYPE_OIL, 500);
			// Train: E, E, F, O. 
			// If we remove index 2 (FOOD), then OIL becomes first freight.
			int result = removeCar(&t, 2);
			Assert::AreNotEqual(0, result);
		}

		/*
		Jira Ticket: TY-014
		Description: Verify that engines must remain at the front of the train after an engine removal.
		*/
		TEST_METHOD(AT14_EnginesStayAtFrontOnRemove)
		{
			Train t = {};
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_ENGINE, 1000);
			addCar(&t, TYPE_FOOD, 500);
			int result = removeCar(&t, 0); // remove first engine
			Assert::AreEqual(0, result);
			Assert::AreEqual((int)TYPE_ENGINE, (int)t.inventory[0].type);
			Assert::AreEqual((int)TYPE_FOOD, (int)t.inventory[1].type);
		}
	};

    TEST_CLASS(IntegrationTests)
    {
    private:

        static void InitTrain(Train& train)
        {
            memset(&train, 0, sizeof(Train));
        }

        static std::string CaptureDisplayCarsAndWeight(const Train& train)
        {
            const char* fileName = "integration_output.txt";

            FILE* dummy;
            freopen_s(&dummy, fileName, "w", stdout);

            displayCarsAndWeight(&train);
            fflush(stdout);

            freopen_s(&dummy, "CONOUT$", "w", stdout);

            std::ifstream in(fileName);
            std::stringstream buffer;
            buffer << in.rdbuf();
            in.close();

            remove(fileName);

            return buffer.str();
        }

    public:

        TEST_METHOD(IT001_BuildValidTrain_Food_CheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_FOOD, 2000);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual(1, safe);
        }

        TEST_METHOD(IT002_BuildValidTrain_Wood_CheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_WOOD, 3000);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual(1, safe);
        }

        TEST_METHOD(IT003_BuildValidTrain_HeavyEngineFood_CheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 6000);
            int r2 = addCar(&train, TYPE_FOOD, 1500);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual(1, safe);
        }

        TEST_METHOD(IT004_BuildTrain_DisplayWeight_EngineFood)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_FOOD, 2000);
            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual((float)7000, train.totalWeight);
            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("Car 1") != std::string::npos);
            Assert::IsTrue(output.find("7000") != std::string::npos);
        }

        TEST_METHOD(IT005_BuildTrain_DisplayWeight_EngineWood)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_WOOD, 3000);
            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual((float)8000, train.totalWeight);
            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("Car 1") != std::string::npos);
            Assert::IsTrue(output.find("8000") != std::string::npos);
        }

        TEST_METHOD(IT006_BuildTrain_DisplayWeight_HeavyEngineFood)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5500);
            int r2 = addCar(&train, TYPE_FOOD, 2500);
            std::string output = CaptureDisplayCarsAndWeight(train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(2, train.carCount);
            Assert::AreEqual((float)8000, train.totalWeight);
            Assert::IsTrue(output.find("Car 0") != std::string::npos);
            Assert::IsTrue(output.find("Car 1") != std::string::npos);
            Assert::IsTrue(output.find("8000") != std::string::npos);
        }

        TEST_METHOD(IT007_RemoveFood_RecheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_FOOD, 2000);
            int r3 = removeCar(&train, 1);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(0, r3);
            Assert::AreEqual(1, train.carCount);
            Assert::AreEqual(1, safe);
        }

        TEST_METHOD(IT008_RemoveWood_RecheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 5000);
            int r2 = addCar(&train, TYPE_WOOD, 3000);
            int r3 = removeCar(&train, 1);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(0, r3);
            Assert::AreEqual(1, train.carCount);
            Assert::AreEqual(1, safe);
        }

        TEST_METHOD(IT009_RemoveFoodFromHeavyEngine_RecheckSafety)
        {
            Train train;
            InitTrain(train);

            int r1 = addCar(&train, TYPE_ENGINE, 6000);
            int r2 = addCar(&train, TYPE_FOOD, 1500);
            int r3 = removeCar(&train, 1);
            int safe = isTrainSafe(&train);

            Assert::AreEqual(0, r1);
            Assert::AreEqual(0, r2);
            Assert::AreEqual(0, r3);
            Assert::AreEqual(1, train.carCount);
            Assert::AreEqual(1, safe);
        }

        // IT010: Engine Power vs Load (manual injection to test isTrainSafe)
        TEST_METHOD(IT010_EnginePowerValidation)
        {
            Train t = {};
            addCar(&t, TYPE_ENGINE, 1000); // capacity 5000
            addCar(&t, TYPE_FOOD, 3000);
            // Manually add car to bypass addCar pull capacity check
            t.inventory[t.carCount].type = TYPE_WOOD;
            t.inventory[t.carCount].weight = 3000;
            t.carCount++;
            t.totalWeight += 3000;
            Assert::AreEqual(0, isTrainSafe(&t));
        }

        // IT011: Safety Rule Persistence after Removal
        TEST_METHOD(IT011_SafetyPersistenceAfterRemoval)
        {
            Train t = {};
            addCar(&t, TYPE_ENGINE, 1000);
            addCar(&t, TYPE_WOOD, 500);
            addCar(&t, TYPE_FOOD, 500); // index 2
            addCar(&t, TYPE_OIL, 500);

            // Try removing the buffer car
            int res = removeCar(&t, 2);
            Assert::AreNotEqual(0, res); // Should fail to protect adjacency rule
            Assert::AreEqual(1, isTrainSafe(&t)); // Should still be safe
        }

        // IT012: Heavy Train with Multiple Engines
        TEST_METHOD(IT012_MultipleEnginesHeavyLoad)
        {
            Train t = {};
            addCar(&t, TYPE_ENGINE, 1000);
            addCar(&t, TYPE_ENGINE, 1000); // capacity 10000
            addCar(&t, TYPE_FOOD, 4000);
            addCar(&t, TYPE_WOOD, 4000);
            Assert::AreEqual(1, isTrainSafe(&t)); // 10000 total, 10000 capacity
        }

        // IT013: System Limit vs Engine Capacity
        TEST_METHOD(IT013_SystemLimitEnforced)
        {
            Train t = {};
            addCar(&t, TYPE_ENGINE, 5000);
            addCar(&t, TYPE_ENGINE, 5000);
            addCar(&t, TYPE_ENGINE, 5000);
            addCar(&t, TYPE_ENGINE, 5000); // 4 engines = 20000 capacity

            // Adding 1kg more should fail system limit even if engines could pull more
            int res = addCar(&t, TYPE_FOOD, 1);
            Assert::AreNotEqual(0, res);
        }
    };
}
