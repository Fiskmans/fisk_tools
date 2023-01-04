# FISK_TOOLS
## Utility
<details id="event"> <summary>Event</summary>
 
 ```
 #include "tools/Event.h"
 fisk::tools::Event<...>
 ```
 Event utility, all callbacks needs to be unregistered before destruction of the event
 ___
 `EventReg Register(std::function<void(Args...)> aCallback)`  
 Registers a callback to be executed when the event fires  
 Returns an `EventReg` handle, destruct this to unregister
 ___
 `void Fire(Args... aArgs)`  
 Fires the event and calls all registered callbacks  
 `aArgs`: arguments to pass to all callbacks
 ___
</details>
<details id="single-fire-event"> <summary>Single Fire Event</summary>
 
 ```
 #include "tools/Event.h"
 fisk::tools::SingleFireEvent<...>
 ```
 Event that can only have one callback at a time and only calls the callback once unless manually re-registered, callback does not need to be unregistered before destruction
 ___
 `Register(std::function<void(Args...)> aCallback)`  
 Registers a callback to the event  
 ___
 `Fire(Args... aArgs)`  
 Calls the callback and unregisters it
 ___
</details>
<details id="iterate-pointers"> <summary>Iterate pointers</summary>
 
 ```
 #include "tools/Iterators.h"
 fisk::tools::IteratePointers<class>
 ```
 Utility to pass pair of pointers to places expecting single class with `begin()` and `end()` e.g. range-for
 ___
 **constructor**  
 `IteratePointers(T* aBegin, T* aEnd)`  
 `aBegin`: the begin pointer  
 `aEnd`: the end pointer
 ___
 `T* begin()`  
 Returns the begin pointer
 ___
 `T* end()`  
 Returns the end pointer
 ___
</details>
 
## Network
<details id="socket"> <summary>Socket</summary>
 
 ```
 #include "tools/Socket.h"
 fisk::tools::Socket
 ```
 A platform agnostic interface that holds and automatically closes a socket when destructed
</details>
<details id="tcp-socket"> <summary>TCP Socket</summary>
 
 ```
 #include "tools/TCPSocket.h"
 fisk::tools::TCPSocket
 ```
 A platform agnostic interface that allows interaction with a tcp socket

 ___
 **constructor**  
 `TCPSocket(std::shared_ptr<Socket> aSocket)`  
 Takes in an open and connected [socket](#socket) to use for communications
 ___
 `bool Update()`  
 Updates the socket, flushes any pending writes and reads any available data  
 Returns `true` if all is good and `false` if the socket is no longer useful and should be discarded
 ___
 `void Close()`  
 Closes the socket  
 ___
 `WriteStream& GetWriteStream()`  
 Returns a reference to the [write stream](#writestream), any data written to it will be sent over the socket
 ___
 `ReadStream GetReadStream()`  
 Returns a reference to the [read stream](#readstream), any data read from the socket will be available from here
 ___
 `Event<> OnDataAvailable`  
 This [event](#event) will be fired whenever there is new data on the readstream
 ___
 **Out of class**  
 `TCPSocket ConnectToTCPByName(const char* aName, const char* aServiceOrPort, std::chrono::microseconds aTimeout)`  
 Attempts to create a connection to `aName`  
 `aName`: the hostname or address of the target e.g `"localhost"`, `"192.168.0.1"`, `"8.8.8.8"`, or `"google.com"`  
 `aServiceOrPort`: the servicename or port of the the target e.g `"12345"`, `"http"`, `"80"`, or `"ssl"`  
 `aTimeout`: how long to try for  
 Returns a `TCPSocket`, if no connection could be made it still returns a valid `TCPSocket` but it will always return false from `Update()`
 ___
</details>
<details id="tcp-listen-socket"> <summary>TCP Listen Socket</summary>

 ```
 #include "tools/TCPListenSocket.h"
 fisk::tools::TCPListenSocket
 ```
 A platform agnostic TCP listen socket, Always listens on all available interfaces
 ___
 **constructor**  
 `TCPListenSocket(Port aPort)`  
 Takes in the port to bind to, if `TCPListenSocket::AnyPort` is specified the OS will determine which port to use
 ___
 `bool Update()`  
 Updates the socket checking for any pending connections  
 Returns `true` if all is good, `false` if the socket is no longer useful and should be discarded
 ___
 `Port GetPort()`  
 Returns the `Port` the socket is bound to, useful if `TCPListenSocket::AnyPort` was supplied in the constructor
 ___
 `Event<std::shared_ptr<TCPSocket>> OnNewConnection`  
 This [event](#event) is fired for each new connection
 ___
</details>
 
## Data
<details id="data-processor"> <summary>Data processor</summary>

 ```
 #include "tools/DataProcessor.h"
 fisk::tools::DataProcessor
 ```
 An interface that implements utility to process objects
 ___
 `bool Process(T& aValue)`  
 Processes a value  
 `aValue`: A reference to the value to process  
 Returns `true` if it was successful `false` otherwise
 ___
</details>
<details id="read-stream"> <summary>Read Stream</summary>

 ```
 #include "tools/Stream.h"
 fisk::tools::ReadStream
 ```
 A helper class for buffering and reading data in a stream format
 ___
 `void AppendData(std::shared_ptr<StreamSegment> aData)`  
 Append some data to the stream
 ___
 `bool Read(uint8_t* aData, size_t aSize)`  
 Attempt to read some data from the stream and step the read offset forward  
 `aData`: the target buffer  
 `aSize`: the amount of data to read  
 Returns `true` if all the data could be read `false` otherwise
 ___
 `size_t Peek(uint8_t* aData, size_t aSize)`  
 Peek at some data in the stream, read offset remains unchanged  
 `aData`: the target buffer  
 `aSize`: the max amount of data to peek  
 Returns the amount of data that was copied into `aData` from the stream
 ___
 `void CommitRead()`  
 Commits the current read offset
 ___
 `void RestoreRead()`  
 Restores the read offset to the last time `CommitRead()` was called or to the start of stream if it hasn't been called yet
 ___
</details>
<details id="write-stream"> <summary>Write Stream</summary>
 
 ```
 #include "tools/Stream.h"
 fisk::tools::ReadStream
 ```
 A tool to write data into stream segments
 ___
 `void WriteData(const uint8_t* aData, size_t aSize)`  
 Writes data to the stream  
 `aData`: the data buffer  
 `aSize`: the amount of data to write
 ___
 `std::shared_ptr<StreamSegment> Get()`  
 Detach the current stream and restart the write stream  
 Returns a Linked list of [stream segments](#stream-segment) with all the data written to the stream since the last `Get()`
 ___
 `bool HasData()`  
 Returns `true` if there is data in the stream `false` otherwise
 ___
</details>
<details id="stream-reader"> <summary>Stream Reader</summary>

 ```
 #include "tools/StreamReader.h"
 fisk::tools::StreamReader
 ```
 A tool to help read objects from a [read stream](#read-stream)  
 Implements the [Data Processor](#data-processor) interface
 ___
 **constructor**  
 `StreamReader(ReadStream& aReadStream)`  
 takes in the [stream](#read-stream) to read from
 ___
</details>
<details id="stream-writer"> <summary>Stream Writer</summary>
 
 ```
 #include "tools/StreamWriter.h"
 fisk::tools::StreamWriter
 ```
 A tool to help write objects to a WriteStream  
 Implements the [Data Processor](#data-processor) interface
 ___
 **constructor**  
 `StreamWriter(WriteStream& aStream)`  
 Takes in the [stream](#write-stream) to write to
 ___
</details>


























