# Object Pool Library in C

## Table of Contents

- [Object Pool Library in C](#object-pool-library-in-c)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Clone the Repository](#clone-the-repository)
    - [Build the Project](#build-the-project)
  - [Usage](#usage)
    - [API Overview](#api-overview)
      - [Initialization](#initialization)
      - [Acquiring an Object](#acquiring-an-object)
      - [Releasing an Object](#releasing-an-object)
      - [Resizing the Pool](#resizing-the-pool)
      - [Iterating Over Acquired Objects](#iterating-over-acquired-objects)
      - [Destroying the Pool](#destroying-the-pool)
    - [Example](#example)
  - [Testing](#testing)
    - [Running Tests](#running-tests)
    - [Test Cases](#test-cases)
    - [Interpreting Test Results](#interpreting-test-results)
  - [Contributing](#contributing)
  - [License](#license)
  - [Documentation](#documentation)

## Overview

The **Object Pool Library** is a robust and efficient C library designed to manage a pool of reusable objects. It optimizes memory usage and enhances performance by minimizing dynamic memory allocations and deallocations. This library is thread-safe, supports dynamic resizing, and ensures that all operations on the pool are performed reliably in multi-threaded environments.

## Features

- **Efficient Memory Management:** Reuse objects to reduce the overhead of frequent allocations and deallocations.
- **Thread-Safe Operations:** Built with mutexes to ensure safe concurrent access in multi-threaded applications.
- **Dynamic Resizing:** Easily expand the pool size at runtime to accommodate growing demands.
- **Opaque API Design:** Encapsulates internal structures, promoting clean and maintainable code.
- **Comprehensive Logging:** Integrates with a CLI logger for detailed operational insights.
- **Flexible Testing:** Includes multithreaded and simple test cases to validate functionality and performance.

## Installation

### Prerequisites

- **C Compiler:** GCC or any compatible C compiler.
- **Make:** For building the project using the provided Makefile.
- **pthread Library:** Ensure that the pthread library is available on your system.

### Clone the Repository

Use Git to clone the repository to your local machine:

```bash
git clone https://github.com/orcalinux/object-pool-library.git
cd object-pool-library
```

### Build the Project

The project uses a Makefile to manage compilation and linking. To build the library and test cases, run:

```bash
make
```

**Makefile Targets:**

- `all`: Compiles the library and all test cases.
- `make run_tests`: Builds and runs all test binaries.
- `make clean`: Removes all build artifacts.

## Usage

### API Overview

The library provides a simple and intuitive API for managing object pools.

#### Initialization

Initialize the object pool by allocating memory for the pool and its internal structures.

#### Acquiring an Object

Acquire an object from the pool for use in your application.

#### Releasing an Object

Release the object back to the pool once you're done using it.

#### Resizing the Pool

Dynamically resize the pool to accommodate more objects as needed.

#### Iterating Over Acquired Objects

Iterate over all currently acquired objects to perform bulk operations or inspections.

#### Destroying the Pool

Destroy the object pool and free all associated memory when it's no longer needed.

### Example

Here's a simple example demonstrating how to use the Object Pool Library:

1. **Initialize the Object Pool:**
   - Allocate and set up the pool with a specified number of objects.
2. **Acquire an Object:**
   - Retrieve an object from the pool for use.
3. **Use the Object:**
   - Perform operations using the acquired object.
4. **Release the Object:**
   - Return the object back to the pool for future reuse.
5. **Destroy the Pool:**
   - Clean up all resources when the pool is no longer needed.

## Testing

The library includes comprehensive test cases to ensure functionality and performance.

### Running Tests

Execute the following command to run all tests:

```bash
make run_tests
```

### Test Cases

1. **Multithreaded Test:**

   - Simulates concurrent access by multiple threads acquiring and releasing objects.
   - Tests the dynamic resizing functionality.
   - Logs detailed information about acquisitions, releases, and resizing.

2. **Simple Test:**
   - Verifies basic acquire and release operations.
   - Ensures that the pool correctly manages object states.

### Interpreting Test Results

After running the tests, you should see logs indicating successful initialization, operations, and destruction of the object pool. Any warnings or errors will be logged to help identify issues.

## Contributing

Contributions are welcome! Please follow these steps to contribute to the project:

1. **Fork the Repository**
2. **Create a Feature Branch**
   ```bash
   git checkout -b feature/YourFeatureName
   ```
3. **Commit Your Changes**
   ```bash
   git commit -m "Add some feature"
   ```
4. **Push to the Branch**
   ```bash
   git push origin feature/YourFeatureName
   ```
5. **Open a Pull Request**

Please ensure your code adheres to the project's coding standards and passes all tests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Documentation

Comprehensive documentation is available using Doxygen. To generate the documentation:

1. **Install Doxygen**
   ```bash
   sudo apt-get install doxygen
   ```
2. **Generate Default Doxyfile**
   ```bash
   doxygen -g
   ```
3. **Customize `Doxyfile` as Needed**
4. **Generate Documentation**
   ```bash
   doxygen Doxyfile
   ```
5. **View Documentation**
   - Open the `html/index.html` file in your browser.
