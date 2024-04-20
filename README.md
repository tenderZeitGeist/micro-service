# micro-service++

This project was built primarily a proof of concept, utilizing modern C++ as the main programming language. As an 
additional exercise, it leverages the features of the C++20 standard. Furthermore, it utilizes Boost Beast to provide 
the network capabilities for the service.

## Prerequisites

Building the project requires the following dependencies:
* CMake >= 3.25
* C++20 standard capable compiler, e.g. gcc & clang >= 10.x
* cURL

## Dependencies

* gtest
* boost 

`gtest` as well as `boost` are being managed and configured via `CMake`. Once the download has finished, the 
dependencies will be deployed into the build folder, so no extra steps are required. 

## Building the project

To build the project, follow these steps:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Alternatively, we can also build the project with the tests enabled
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_UNIT_TESTS=On ..
```

Once cmake is done, simply run:
```bash
make -j<n> # You can adjust the number of threads you to your liking
./micro-service
```

The program will automatically start and deploy itself onto port 8080.

## Run test test application
To verify the integrity of the program, a slim test suite using `gtest` is being provided:
```bash
./tests/micro-service-test
```

## Testing the API vial `curl`

### Get
```
curl -X GET \
  http://localhost:8080/compounds
```

```
curl -X GET \
  http://localhost:8080/compounds/PettingZoo/animals
```

```
curl -X GET \
  http://localhost:8080/animals/Cat
```

```
curl -X GET \
  http://localhost:8080/animals/birds
```

### Post
```
curl -X POST \
  http://localhost:8080/compounds/PettingZoo/animals \
  -H 'Content-Type: application/json' \
  -d '{
    "name": "Possum",
    "age": 3,
    "species": "Mammal"
}'
```

### Delete
```
curl -X DELETE \
  http://localhost:8080/compounds/PettingZoo/animals/Cat
```

## TODO
* Make the program thread safe
* Implement a logger
* Refactor the service layer
* Bump to cpp 23
* Potentially add an actual database
* Doxygen documentation
