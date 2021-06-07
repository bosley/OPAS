# Bosley's Software Libraries (BSL)

These libraries are a work in progress, and are meant to be a sort of tool-kit for my side projects. 

## Documentation
Within each library's subdirectory there will be tests and examples that show off how to use them. In some cases there are README.md files within the subdirectory that give a nice overview of how to use the library. 

I would like to ensure all of the code comments are in the doxygen style, but I know that some aren't as they were merged from other older projects. Eventually I would like to update everything and get doxygen setup, but that is on the back burner.

## License

The BSL libraries are under the MIT license, but some of the libraries it uses might not be. 

## Building

### CPPUTest is required for all libraries - 

```bash
    bash install_cpputest.sh
```

### JSON is required for bslAi -

```bash
    bash install_json.sh
```

### OpenCV is required for bslCv

```bash
    bash install_opencv.sh
```

### BslCore is required for other BSL libraries

```bash
    mkdir build-bslCore
    cd build-bslCore
    ccmake ../bslCore
    make -j4
    sudo make install
```

### BslCv

```bash
    mkdir build-bslCv
    cd build-bslCv
    ccmake ../bslCv
    make -j4
    sudo make install
```

### BslAi

```bash
    mkdir build-bslAi
    cd build-bslAi
    ccmake ../bslAi
    make -j4
    sudo make install
```

## External Usage

In the root of the library directories a directory called *example_usage* can be found that shows how to setup and link that library to a project.