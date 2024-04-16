# micro-service++
A small REST-application, written in cpp using the cpp20 & boost.


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
