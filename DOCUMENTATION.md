# FISK_TOOLS
## Utility
### Event
```
#include "tools/Event.h"
fisk::tools::Event<...>
```
Event utility
#### `EventReg Register(std::function<void(Args...)> aCallback)`
Registers a callback to be executed when the event fires

aCallback: the callback

Return `EventReg`: an event registration handle, destruct this to unregister. needs to have a shorter lifetime than the event and any objects affected by the callback
#### `void Fire(Args... aArgs)`
Fires the event and calls all registered callbacks

aArgs: arguments to pass to all callbacks
### SingleFireEvent
```
#include "tools/Event.h"
fisk::tools::SingleFireEvent<...>
```
Event that can only have one callback at a time and only calls the callback once unless manually re-registered
#### `Register(std::function<void(Args...)> aCallback)`
Registers a callback to the event

aCallback: the callback
#### `Fire(Args... aArgs)`
Calls the callback and unregisters it
### IteratePointers
```
#include "tools/Iterators.h"
fisk::tools::IteratePointers<class>
```
Utility to pass pair of pointers to places expecting single class with `begin()` and `end()` like range-for
#### `constructor IteratePointers(T* aBegin, T* aEnd)`
aBegin: the begin pointer
aEnd: the end pointer

#### `T* begin()`
Return `T*`: the begin pointer
#### `T* end()`
Return `T*`: the end pointer 

## Network
### Socket
```
#include "tools/Socket.h"
fisk::tools::Socket
```
A platform agnostic interface that holds and automatically closes a socket when destructed
### TCPSocket
```
#include "tools/TCPSocket.h"
fisk::tools::TCPSocket
```
A platform agnostic interface that allows interaction with a tcp socket

#### `constructor TCPSocket(std::shared_ptr<Socket> aSocket)`
aSocket: the underlying socket to use

#### `bool Update()`
Updates the socket, flushes any pending writes and reads any available data

Return `bool`: `true` if all is good, `false` if the socket is no longer useful and should be discarded

#### `void Close()`
Closes the socket

#### `WriteStream& GetWriteStream()`
Return [WriteStream&](#writestream): A reference to the writestream, any data written to it will be sent over the socket

#### `ReadStream GetReadStream()`
Return [ReadStream&](#readstream): A reference to the readstream, any data read from the socket will be available from here

#### `Event<> OnDataAvailable`
This [event](#event) will be fired whenever there is new data on the readstream

#### Out of class 
`TCPSocket ConnectToTCPByName(const char* aName, const char* aServiceOrPort, std::chrono::microseconds aTimeout)`  
Attempts to create a connection to `aName`
 
aName: the hostname or address of the target e.g `"localhost"`, `"192.168.0.1"`, `"8.8.8.8"`, or `"google.com"`  
aServiceOrPort: the servicename or port of the the target e.g `"12345"`, `"http"`, `"80"`, or `"ssl"`  
aTimeout: how long to try for

Return `TCPSocket`: The resulting socket to interact with the target with, if no connection could be made it still returns a valid `TCPSocket` but it will always return false from `Update()`
### TCPListenSocket
```
#include "tools/TCPListenSocket.h"
fisk::tools::TCPListenSocket
```
A platform agnostic TCP listen socket, Always listens on all available interfaces
#### `constructor TCPListenSocket(Port aPort)`
aPort: the port to bind to, if `TCPListenSocket::AnyPort` is selected the OS will determine which port to use

#### `bool Update()`
Updates the socket checking for any pending connections

Return `bool`: `true` if all is good, `false` if the socket is no longer useful and should be discarded

#### `Port GetPort()`
Return `Port`: The port the socket is bound to, useful if `TCPListenSocket::AnyPort` was supplied in the constructor

#### `Event<std::shared_ptr<TCPSocket>> OnNewConnection`
this [event](#event) is fired for each new connection

## Data
### ReadStream
```
#include "tools/Stream.h"
fisk::tools::ReadStream
```
A helper class for buffering and reading data in a stream format
#### `void AppendData(std::shared_ptr<StreamSegment> aData)`
Append some data to the stream

aData: A linked list of data segments
#### `bool Read(uint8_t* aData, size_t aSize)`
Attempt to read some data from the stream

aData: the target buffer
aSize: the amount of data to read

Return `bool`: `true` if all the data could be read `false` otherwise

#### `size_t Peek(uint8_t* aData, size_t aSize)`
Peek at some data in the stream, read offset remains unchanged

aData: the target buffer  
aSize: the max amount of data to peek

Return `size_t`: The amount of data that was copied into `aData` from the stream
#### `void CommitRead()`
Commits the current read offset

#### `void RestoreRead()`
Restores the read offset to the last time `CommitRead()` was called or to the start of stream if it hasn't been called yet
### WriteStream
```
#include "tools/Stream.h"
fisk::tools::ReadStream
```
A tool to write data into stream segments
#### `void WriteData(const uint8_t* aData, size_t aSize)`
Writes data to the stream

aData: the data buffer  
aSize: the amount of data to write
#### `std::shared_ptr<StreamSegment> Get()`
Detach the current stream and restart the write stream

Return `std::shared_ptr<StreamSegment>`: A Linked list of all the data written to the stream since the last `Get()`
#### `bool HasData()`
Return `bool`: `true` if there is data in the stream `false` otherwise
### StreamReader
```
#include "tools/StreamReader.h"
fisk::tools::StreamReader
```
A tool to help read objects from a ReadStream  
Implements [DataProcessor](#dataprocessor)

#### `constructor StreamReader(ReadStream& aReadStream)`
aReadStream: the stream to read from
### StreamWriter
```
#include "tools/StreamWriter.h"
fisk::tools::StreamWriter
```
A tool to help write objects to a WriteStream  
Implements [DataProcessor](#dataprocessor)
#### `StreamWriter(WriteStream& aStream)`
aStream: the stream to write to
### DataProcessor
```
#include "tools/DataProcessor.h"
fisk::tools::DataProcessor
```
A pure-virtual interface that implements utility to process objects
#### `bool Process(T& aValue)`
Processes a value

aValue: A reference to the value to process

Return `bool`: `true` if it was successful `false` otherwise


























