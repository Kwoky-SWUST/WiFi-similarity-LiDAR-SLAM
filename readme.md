* This is the code for WiFi similarity based SLAM *

1. go to src/exlib/carmen/src and run ./configure,
2. when asked to select the laser info, please use NO,
3. when asked to choose the robot, select 6
4. compile this folder
5. go back to exlib and try to compile the rest
6. wifi slam is located in carmenx/ambisense_slam/example_wifi_slam


# WiFi-similarity-LiDAR-SLAM

This repository contains the source code for a WiFi-similarity-based LiDAR SLAM (Simultaneous Localization and Mapping) system.

## 🚀 Getting Started

These instructions will guide you on how to build and run the project.

### Prerequisites

Before you begin, ensure you have the following installed:

*   **C++ Compiler:** A C++ compiler that supports C++11 or later (e.g., g++).
*   **CMake:**  A cross-platform build system generator.
*   **Make:** A build automation tool.
*   **ROS (Robot Operating System):** (If the project integrates with ROS) Install the appropriate ROS distribution.
*   **LiDAR Driver:** Driver for your specific LiDAR sensor.
*   **WiFi Card and Driver:** Driver for your specific WiFi card.

### Installation

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/Kwoky-SWUST/WiFi-similarity-LiDAR-SLAM.git
    cd WiFi-similarity-LiDAR-SLAM
    ```

2.  **Build the project:**

    ```bash
    cd src
    ./prepare_make.sh
    make
    cd ..
    ```

    **Note:** The `prepare_make.sh` script likely sets up the build environment based on the `Makefile.conf` and `Makefile.rules` files.  Examine these files if you encounter build issues.

3.  **Copy Libraries (if needed):**

    The build process creates static libraries (`libnewmat.a` and `libutils.a`) in the `lib/` directory.  You might need to copy these to a system library directory or link them appropriately depending on how you intend to use the project.

## ✨ Key Features

*   **LiDAR SLAM:** Implements a LiDAR-based SLAM algorithm for creating maps and localizing the robot.
*   **WiFi Similarity:** Integrates WiFi signal strength information to improve SLAM accuracy and robustness.
*   **Modular Design:** The code is organized into modules (e.g., `carmenx`, `exlib`, `utils`) for easier maintenance and extension.

## 🤝 Contributing

Contributions are welcome!  Here's how you can contribute:

1.  **Fork the repository.**
2.  **Create a new branch for your feature or bug fix.**
3.  **Make your changes and commit them with clear, concise messages.**
4.  **Submit a pull request.**

Please follow these guidelines when contributing:

*   **Code Style:** Adhere to the existing code style.
*   **Testing:**  Write unit tests for your code.
*   **Documentation:**  Document your code changes.

## 🐛 Troubleshooting

*   **Compilation Errors:**
    *   Ensure that all dependencies are installed correctly.
    *   Check the `Makefile.conf` file for correct paths to libraries and headers.
    *   Verify that your C++ compiler supports the required language features.
*   **Runtime Errors:**
    *   Check the console output for error messages.
    *   Use a debugger to step through the code and identify the source of the error.
    *   Ensure that your LiDAR and WiFi drivers are properly configured.
*   **SLAM Performance Issues:**
    *   Tune the SLAM parameters for your specific environment.
    *   Ensure that the LiDAR and WiFi data are properly calibrated.
    *   Consider using a more powerful computer if the SLAM algorithm is too computationally intensive.

## 🧑‍💻 Contributors

*   Kwoky-SWUST
```
