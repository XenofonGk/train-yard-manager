# Train Yard Management System

A robust C-based management and safety validation system for rail inventory and train configurations. This project provides a programmatic interface for building trains while strictly enforcing industrial safety protocols, weight limits, and locomotive power requirements.

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
- `root/SourceCode/UnitTest/`: Comprehensive C++ unit testing suite using MSTest.
- `root/Documents/Testing/`: RTM and Final Testing Reports.

## Getting Started

### Prerequisites

- **Mac/Linux:** `gcc` or `clang` and `make`.
- **Windows:** Visual Studio 2022 with C++ Development tools.

### Building and Running Tests

#### Mac/Linux
To compile and execute the automated test suite:
```bash
cd root/SourceCode/Project
make test
```

#### Windows
1. Open `root/SourceCode/Project/Project.sln` in Visual Studio.
2. Build the solution (Ctrl+Shift+B).
3. Run tests using the **Test Explorer**.

## Testing Architecture

The project employs a three-tier testing strategy to ensure 100% reliability:
1. **Unit Testing (Whitebox):** Validates internal logic and branch coverage of core functions.
2. **Integration Testing:** Verifies the interaction between car management, weight calculations, and safety checks.
3. **Acceptance Testing (UAT):** Maps directly to business requirements to ensure full protocol compliance.

A **pre-push Git hook** is included to prevent the deployment of code that does not pass the full test suite.

## License

This project is open-source and developed as a personal technical demonstration.
