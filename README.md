# The Ray Tracing Series

This is a repository following along with [TheCherno's Ray Tracing Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl) - built initially from the [Walnut Template App](https://github.com/StudioCherno/WalnutAppTemplate), which includes [Walnut](https://github.com/StudioCherno/Walnut/) as a submodule. 

Updated to use CMake and a [Forked Version of Walnut's Dev Branch](https://github.com/PatrickMiller728/Walnut/tree/cmake) as a submodule, as an exercise to learn [CMake](https://cmake.org/cmake/help/latest/).

## Building
Clone this repository:
```commandline
git clone
```

Initialize the Walnut Submodule:
```commandline
git submodule update --init --recursive
```

The fork of Walnut uses [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview) as a package manager, included as a submodule in Walnut.
Navigate to the vcpkg directory and run bootstrap-vcpkg.bat:
```commandline
cd <path\to\vcpkg\bootstrap-vcpkg.bat>
bootstrap-vcpkg.bat
```

Included is a default CMake Preset, but you may want to create your own preset for release builds.
This is most easily done by creating a CMakeUserPresets.json file in the root directory with the following contents:
```json
{
  "version": 2,
  "configurePresets": [
    {
      "name": "release",
      "inherits": "default",
      "displayName": "Release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo"
      }
    }
  ]
}
```

Run CMake
```commandline
cmake --build --preset default
```