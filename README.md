# CS165 Project
## Ashly Hernandez, Arlene Gonzalez
------------------------------------




### Files
- src/client
  - client code, just pass in filename( e.g. ./client.c filename01)
- src/proxy
  - proxy code, pass in port number and server port (called by: ./proxy.c 9991 9999)
  - proxy ports are: 99991,9992,9993,9994,9995,9996
- src/server
  - server code (called by: ./server 9999)
- files/cache.txt
  - holds cached files from proxies

## Project Description
- The client passes in a file. Rendevous hashing is used to find the correct proxy to ask for the file. It then connects to that proxy
- The proxy checks if the file is blacklisted by using a bloom filter
  - if it is, it sends an error message to the client
  - if it isnt, it checks the local cache for the file
  - if the file is in the local cache, it sents it to the client
  - if the file is not in the local cache, it asks the server for the file
  - it gets the file from the server and sends it to the client
- The server sends files to the proxy when they request it
