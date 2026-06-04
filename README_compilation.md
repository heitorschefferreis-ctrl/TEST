# HOW TO COMPILE AND LOAD YOUR GEODE MOD (Geometry Dash 2.2081)

A ".geode" file that the game loads directly is a compiled package containing operating system-specific dynamic libraries (.dll for Windows, .so for Android, .dylib for macOS). Since a web browser cannot execute native C++ compilers inside a sandbox, this download gives you the COMPLETE Geode C++ Source Project.

Follow these simple steps on your local system to build and compile:

---

### Step 1: Install the Geode SDK CLI
Open a Command Prompt or PowerShell terminal and run:

Windows (PowerShell):
iwr -useb https://raw.githubusercontent.com/geode-sdk/cli/main/install.ps1 | iex

macOS / Linux / Android (Bash):
curl --proto '=https' --tlsv1.2 -sSf https://raw.githubusercontent.com/geode-sdk/cli/main/install.sh | sh

---

### Step 2: Install Compiler Tools
- Windows: Install Visual Studio 2022 (Community Edition works!) with the "Desktop development with C++" workload checked.
- macOS: Install Xcode from the App Store and make sure command line tools are installed.
- Android: Install Android NDK via Android Studio if compiling for mobile.

---

### Step 3: Run the Compilation
Open VS Code in this extracted folder:
1. Install the "CMake Tools" extension.
2. Select your build platform (Windows, macOS, or Android).
3. Click "Build" (or run the command "cmake --build . --config Release" in a "build" folder).

The Geode compiler will package everything into "omni-pathfinder.geode" and automatically copy it to your Geometry Dash game folder!