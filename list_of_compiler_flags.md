### List of relevant compiler options

##### Compiler
```makefile
--include_path="${PROJECT_ROOT}"
--include_path="${PROJECT_ROOT}/database"
--include_path="${PROJECT_ROOT}/fann/inc"
--include_path="${PROJECT_ROOT}/utils"
--printf_support=full # to print floats
--define=PROFILE # to enable time profiling
```

##### Linker
```makefile
--heap_size=2500 # for dynamic memory allocation
-i"${PROJECT_ROOT}/utils/libs"
```