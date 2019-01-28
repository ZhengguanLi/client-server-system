# Client Server System

The client-server system is a socket programming in C language (Linux environment) which implements a pair of client and server that can achieve simple password verification to indicate the current market value of the stock.

## Usage

### Compile

- Server
    ```
    gcc server.c -o server.o
    ```

- Client
    ```
    gcc client.c -o client.o
    ```

### Run

- Server
    ```
    ./server.o [port number]
    ```

- Client
    ```
    ./client.o [IP address] [port number]
    ```

## Running Test

- Valid username and password
  - Server
    ```
    $./server.o 53598
    Socket created!
    Bind completed!
    --------------------------------
    Received username
    Received password
    Verified, sent back result
    ```

  - Client
    ```
    $ ./client.o 192/168.11.131 53598
    Socket created!
    Connection succeed!
    --------------------------------
    Please enter username: apple
    Please enter password: jP79bNs2
    The corresponding market value is: 158.63
    ```

- Invalid username/password
  - Server
    ```
    $./server.o 53598
    Socket created!
    Bind completed!
    --------------------------------
    Received username
    Received password
    Username/password did not match
    ```

  - Client
    ```
    $ ./client.o 192/168.11.131 53598
    Socket created!
    Connection succeed!
    --------------------------------
    Please enter username: apple
    Please enter password: 123
    Username/password did not match!
    ```

- Timeout for password
    - Server
        ```
        $./server.o 53598
        Socket created!
        Bind completed!
        --------------------------------
        Received username
        Timing out for password!
        ```
    - Client
        ```
        $ ./client.o 192/168.11.131 53598
        Socket created!
        Connection succeed!
        --------------------------------
        Please enter username: apple
        Please enter password: 123
        Verification message unreceived!
        ``` 
## Authors

- Zhengguan Li