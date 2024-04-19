# micro-service++

This project was built more or a less being a proof of concept, my main language being in fact modern cpp. As an 
additional exercise, I wanted to leverage the power of the cpp 20 standard, having little to no experience prior.

### Building

Building the project requires the following dependencies:
* cmake >= 3.5
* cpp 20 standard capable compiler, i.e. gcc & clang >= 10.x
* boost (built and tested with 1.83)

Additionally, for running the test case(s):
* gtest
* curl

`gtest` is being provided and included via cmake. Once it has been downloaded, it will be deployed into the build 
folder, so no extra steps are required. If you want to remove it from your system, simply deleting the build folder 
will suffice. `curl` has to be provided by the system itself.

The build process is being handled, as to no surprise, via `cmake`.
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

### Run test test application
To verify the integrity of the program, a slim test suite using `gtest` is being provided:
```bash
./tests/micro-service-test
```

### Testing the API vial `curl`

#### Get
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

#### Post
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

#### Delete
```
curl -X DELETE \
  http://localhost:8080/compounds/PettingZoo/animals/Cat
```

### TODO
* Make the program thread safe
* Implement a logger
* Refactor the service layer
* Bump to cpp 23
* Potentially add an actual database
* Doxygen documentation
