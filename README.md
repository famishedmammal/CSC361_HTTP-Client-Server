#HTTP Server-Client Architecture 
**Overview** : A simple TCP-based HTTP Server/Client System for learning purposes.

TO RUN MAKE FILE
- type "make all"

###CLIENT
- The client uses 3 base functions under main()
  * parse_URI
  * open_connection  
  * perform HTTP
1. Parse URI
  * The client will parse whatever is entered into the terminal into its base components
  * It will look for the IP, PORT, and RESOURCE  
  * If the input is preceded by "http://", it will ignore it.
  * It looks for a ":" character, to find the PORT. If ":" is not found, it uses the default port 30.
  * It then looks for the next "/" character. Every character following that "/" is marked as the RESOURCE.
2. Open_Connection
  * If a character string is detected as the hostname, it will convert it into an IP using gethostbyname()
  * It then uses connect() to establish a base connection with the host, with the given PORT
3. Perform HTTP
  * It sends an request to the connected server in the form of "GET host/resource HTTP/1.0"
  * If a response is received, it breaks it into header and body by looking for the "/r/n/r/n" string.


###SERVER-SIDE
- The server uses a main loop, and a function for handling incoming requests
1. Main
  * The server first binds a socket with the given port, for listening
  * It then sets the socket to listen mode and begins listening for incoming requests
  * It then initiates the main loop, which waits for messages.
2. Handle Request
  * It opens a socket for the new connection, and checks the directory for the file requested
  * If the file is found, a 200 OK message is sent back, along with the content of the file
  * If the file is not found, a 404 Not Found message is sent back.
  * If the command is not recognized, a 501 message is sent back.
  * Once the request has been dealt with, the socket is closed, and the loop reiterates, to listen for new requests.
