# micro-service
A small REST-application


## Testing the API vial `curl`

### Post
```
curl -X POST \
  http://localhost:8080/compounds/PettingZoo/animals \
  -H 'Content-Type: application/json' \
  -d '{
    "name": "Fluffy",
    "age": 3,
    "species": "Cat"
}'
```
