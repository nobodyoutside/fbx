
### FbxViewer Project Guidelines

#### Build & Configuration
- **Prerequisites**: The project depends on the Autodesk FBX SDK.
- **SDK Path**: The FBX SDK is expected to be located in the `SDK/fbxsdk` directory within the project root.
- **CMake Configuration**:
  - The `CMakeLists.txt` handles the inclusion of FBX SDK headers and libraries.
  - It currently targets `SDK/fbxsdk/lib/vs2017/x64/debug` for libraries.
  - `FBXSDK_SHARED` is defined to link against the dynamic library (`libfbxsdk.dll`).
  - A post-build command is used to copy `libfbxsdk.dll` to the executable directory.
- **Compilation**:
  ```powershell
  cmake -S . -B cmake-build-debug
  cmake --build cmake-build-debug --target FbxViewer
  ```

#### Testing
- **Configuration**: Testing is enabled via `enable_testing()` in `CMakeLists.txt`.
- **Running Tests**:
  Use `ctest` to run the defined tests:
  ```powershell
  ctest --test-dir cmake-build-debug --output-on-failure
  ```
- **Adding New Tests**:
  1. Add the test using `add_test` in `CMakeLists.txt`.
  2. You can target the main executable or create a separate test executable.
  3. Ensure any required DLLs are copied to the test executable directory using `add_custom_command`.

#### Development Information
- **Code Style**: Follow the existing C++ style.
- **FBX SDK Usage**: Always ensure the `FbxManager` is properly created and destroyed.
- **Environment**: The project is configured for Windows using MSVC (Visual Studio 2017/2019/2022 compatibility via the `vs2017` SDK folder).
