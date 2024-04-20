# micro-service++

This project was built primarily as a proof of concept, utilizing modern C++, specifically the C++20 standard. As an 
additional exercise, it leverages the features of the aforementioned standard. Furthermore, it utilizes Boost Beast, providing the network capabilities of the service.

## Prerequisites

Building the project requires the following dependencies:
* CMake >= 3.25
* C++20 standard capable compiler, e.g. gcc & clang >= 10.x
* cURL

## Dependencies

* gtest
* boost >= 1.83.0

`gtest` as well as `boost` are being managed and configured via `CMake`. Once the download has finished, the 
dependencies will be deployed into the build folder, so no extra steps are required. 

## Building the project

To build the project, follow these steps:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release <path/to/source_folder>
```

Alternatively, we can also build the project with the tests enabled
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_UNIT_TESTS=On <path/to/source_folder>
```

When the configuration step is finished, simply run:
```bash
make -j<Number of threads> # You can adjust the number to your liking
```

After the program has been compiled, you can execute the binary from within the build folder via:
```
./micro-service
```

The program deploys itself onto port 8080 and will be ready to receive requests.

## Running test application
To verify the integrity of the program, a slim test suite is being provided:
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
* Thread-safety
* Implement a logger
* Refactor the service layer
* Bump to cpp 23
* Add an actual database
* Doxygen documentation
