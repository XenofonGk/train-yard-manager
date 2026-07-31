# Train Yard Management System

A C-based console application for building and validating train configurations in a rail yard. It lets an operator add and remove cars (engines, food, wood, oil), then checks the resulting train against a set of industrial safety rules: engines must lead, wood and oil cars cannot sit next to each other, the first freight car cannot be oil, and the locomotives must be able to pull the attached weight.

## Features

- **Dynamic Car Management:** Add and remove various car types (Engine, Food, Wood, Oil) with real-time validation.
- **Safety Protocol Enforcement:**
  - **Lead Rule:** Engines must always lead the train.
  - **Adjacency Rule:** Prevents hazardous placement (e.g., WOOD and OIL cars cannot be adjacent).
  - **First Freight Rule:** Ensures the first car following the locomotives is not volatile (OIL).
- **Weight & Power Validation:**
  - **Pull Capacity:** Calculates total locomotive power (5,000 kg per engine).
  - **System Limits:** Enforces a hard system-wide weight limit of 20,000 kg.
  - **Safety Checks:** Validates if the current locomotive count can safely pull the attached freight weight.
- **Inventory Reporting:** Generates detailed weight and configuration reports.

## Project Structure

- `root/SourceCode/Project/`: Core C library and main application.
- `root/SourceCode/UnitTest/`: C++ unit testing suite using MSTest (Windows/Visual Studio only).
- `root/Documents/Testing/`: RTM and Final Testing Reports.

## Getting Started

### Prerequisites

- **Mac/Linux:** `gcc` or `clang` and `make`.
- **Windows:** Visual Studio 2022 with C++ Development tools.

## Build and run

All commands below are run from `root/SourceCode/Project`.

Build the interactive application:

```bash
cd root/SourceCode/Project
make all
./train_yard_app
```

Build and run the automated test suite:

```bash
cd root/SourceCode/Project
make test
```

Remove build output:

```bash
make clean
```

On Windows, open `root/SourceCode/Project/Project.sln` in Visual Studio, build the solution (Ctrl+Shift+B), and run the MSTest suite in `root/SourceCode/UnitTest` from the Test Explorer.

## Example session

The following is real captured output from running `./train_yard_app` and adding an engine, a food car, and a wood car, then viewing the train, checking the weight, and running the safety check:

```
Welcome to the Train Yard Management System (Mac/Linux Edition)

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: Enter type (0=Engine, 1=Food, 2=Wood, 3=Oil): Enter weight: Car added successfully.

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: Enter type (0=Engine, 1=Food, 2=Wood, 3=Oil): Enter weight: Car added successfully.

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: Enter type (0=Engine, 1=Food, 2=Wood, 3=Oil): Enter weight: Car added successfully.

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: >> Display Train
Car Count: 3
Total Weight: 12500.00
Number of Engines: 1
Car 0: Type 0, Weight 8000.00
Car 1: Type 1, Weight 3000.00
Car 2: Type 2, Weight 1500.00

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: >> Show train cars and weight
Car 0: Type 0, Weight 8000.00
Car 1: Type 1, Weight 3000.00
Car 2: Type 2, Weight 1500.00
Total Weight: 12500.00

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: >> Status: SAFE

[A]dd | [V]iew | [W]eight | [S]afety | [R]emove | [Q]uit: Exiting program.
```

(Input in this session, in order: `A 0 8000`, `A 1 3000`, `A 2 1500`, `V`, `W`, `S`, `Q`.)

## Design

All console I/O (`printf`/`scanf` prompts and menu handling) lives in `main.c`. The logic in `train_yard.c` — `addCar`, `removeCar`, `displayTrain`, `isTrainSafe`, `displayCarsAndWeight`, `getPullCapacity` — contains no I/O calls at all; it operates purely on the `Train` struct passed in and returns values or prints reports, but never reads from stdin. This separation is what makes the code testable: `test_runner.c` and the MSTest suite can drive the same functions directly with in-memory `Train` values, without needing to simulate console input.

## Testing Architecture

The project employs a three-tier testing strategy to ensure 100% reliability:
1. **Unit Testing (Whitebox):** Validates internal logic and branch coverage of core functions.
2. **Integration Testing:** Verifies the interaction between car management, weight calculations, and safety checks.
3. **Acceptance Testing (UAT):** Maps directly to business requirements to ensure full protocol compliance.

A **pre-push Git hook** is included to prevent the deployment of code that does not pass the full test suite.

Continuous integration runs `make test` on every push and pull request to `main` via GitHub Actions (see `.github/workflows/ci.yml`). The MSTest C++ suite in `root/SourceCode/UnitTest` is Visual Studio only and is not part of the CI pipeline.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
