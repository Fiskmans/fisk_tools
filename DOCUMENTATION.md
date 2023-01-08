# FISK_TOOLS
## Utility
<details id="event"> <summary>Event</summary>
 
 ```
 #include "tools/Event.h"
 fisk::tools::Event<...>
 ```
 Stores callbacks and executes them when fired, all callbacks needs to be unregistered before destruction of the event
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
 Helps with iterating over a pair of pointers in e.g. range-for
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
 Holds and automatically closes a platform agnostic socket when destructed
</details>
<details id="tcp-socket"> <summary>TCP Socket</summary>
 
 ```
 #include "tools/TCPSocket.h"
 fisk::tools::TCPSocket
 ```
 Allows interaction with a platform agnostic TCP socket

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
 Platform agnostic TCP listen socket, Always listens on all available interfaces
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
 Interface for classes that implements utility to process objects  
 Handles: `uint8_t`, `int8_t`, `uint16_t`, `int16_t`, `uint32_t`, `int32_t`, `uint64_t`, `int64_t`, `std::string`, and anything that fulfills the [serializable](#serializable) concept
 ___
 `bool Process(T& aValue)`  
 Processes a value  
 `aValue`: A reference to the value to process  
 Returns `true` if it was successful `false` otherwise
 ___
</details>
<details id="serializable"> <summary>Serializable</summary>
 
 ```
 #include "tools/concepts.h"
 fisk::tools::Serializable
 ```
 **concept**  
 `true` if the type can be processed by a [data processor](#data-processor)
</details>
<details id="read-stream"> <summary>Read Stream</summary>

 ```
 #include "tools/Stream.h"
 fisk::tools::ReadStream
 ```
 Buffers a stream of data for reading
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
 Writes data into stream segments
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
 Helps you read objects from a [read stream](#read-stream)  
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
 Helps you write objects to a WriteStream  
 Implements the [Data Processor](#data-processor) interface
 ___
 **constructor**  
 `StreamWriter(WriteStream& aStream)`  
 Takes in the [stream](#write-stream) to write to
 ___
</details>
<details id="json"> <summary>Json</summary>
 
 ```
 #include "tools/JSON.h"
 fisk::tools::Json
 ```
 Json with a low amount of boilerplate and no rough edges
 ___
 `Json& operator[](const char* aKey)`  
 Accesses the child named `aKey`  
 If `this` is not of `ObjectType` always returns a reference to the [NullObject](#nullobject)  
 Returns a reference to the child or to the [NullObject](#nullobject) if non-existant
 ___
 `Json& operator[](int aIndex)`  
 Accesses the child at `aIndex`  
 If `this` is not of `ArrayType` always returns a reference to the [NullObject](#nullobject)  
 Returns a reference to the child or to the [NullObject](#nullobject) if out-of-bounds
 ___
 `bool Parse(const char* aString)`  
 Parses a json string  
 Returns `true` if successful and `false` otherwise. 
 ___
 `Json& AddValue(std::string aKey, T aValue)`  
Creates a named child and sets it equal to `aValue`  
 Upgrades `this` from `NullType` to `ObjectType`, fails if any other type  
 Returns a reference to the newly created child or the [NullObject](#nullobject) if failed
 ___
 `Json& AddValue(std::string aKey, std::unique_ptr<Json> aChild)`  
 Adds a named child  
 Upgrades `this` from `NullType` to `ObjectType`, fails if any other type  
 Returns a reference to the child or the [NullObject](#nullobject) if failed
 ___
 `Json& PushValue(T aValue)`  
 Pushes a child and sets it equal to `aValue`  
 Upgrades `this` from `NullType` to `ArrayType`, fails if any other type  
 Returns a reference to the newly created child or the [NullObject](#nullobject) if failed
 ___
 `Json& AddValue(std::string aKey, std::unique_ptr<Json> aChild)`  
 Pushes a child  
 Upgrades `this` from `NullType` to `ArrayType`, fails if any other type  
 Returns a reference to the child or the [NullObject](#nullobject) if failed
 ___
 `bool HasChild(const char* aKey)`  
 Checks if `this` has a child named `aKey`  
 Returns `false` if this is not of type `Object` otherwise:  
 Returns `true` if there is a child named `aKey`
 ___
 `bool IsNull()`  
  Returns `true` if `this` is of `NullType`
 ___
 `operator bool()`  
  Returns the inverse of `IsNull()`
 ___
 `std::string Serialize(bool aPretty)`  
 Returns a string representing `this` and all of it's children recursivly  
 Formated in a human readable way if `aPretty` is `true` otherwise as compact as possible
 ___
 `Json& operator=(T aValue)`  
 Assigns to `this`, changing type as necessary  
 Returns a referense to `this` to allow chaining  
 Supports `NumberType`, `StringType`, and `BooleanType` and anything convertable to any of these
 ___
 `bool GetIf(T& aValue)`  
 Gets the stored value of `this` if it is of a compatible type  
 Returns `true` if the value could be loaded `false` otherwise  
 Supports `long long`, `long`, `size_t`, `int`, `double`, `float`, `std::string`, and `bool` 
 ___
 `JsonObjectProxy IterateObject()`  
 Returns a proxy object of `this` that can be used to iterate over all the named children in no particular order  
 **example**  
 ```cpp
 fisk::tools::Json root;
 root.Parse(R"(
 {
  "foo": 10,
  "bar": {}
 })");
 
 for (const auto& [key, value] : root.IterateObject()) // the full type returned from the iterator is `JsonObjectIterator::DereferenceType` i.e. 'std::pair<const std::string, Json&>'
 {
  // key is a 'std::string' with the key
  // value is a 'fisk::tools::Json&' with the value associated with key
 }
 ```
 ___
 `JsonObjectProxy IterateObject()`  
 Returns a proxy object of `this` that can be used to iterate over all the unamed children in order  
 **example**  
 ```cpp
 fisk::tools::Json root;
 root.Parse(R"(
  [ 1, 2, 3, "hello", 0.01, true, null, 4]
 )");
 
 for (fisk::tools::Json& value : root.IterateArray())
 { 
  // goes through the sequence 1, 2, 3, "hello", 0.01, true, null, 4 
 }
 ```
 ___
 ### NullObject
 The `NullObject` is a special `Json` object that cannot be modified and any access on it will get you a reference back to it  
 It tests true for `IsNull()`, so does any default constructed `Json` or parsed `null` as well so don't use this to explicitly check for the `NullObject`  
 Attempting to add children to the `NullObject` always fails and returns a reference back to the `NullObject`  
 Attempting to get the stored value of the `NullObject` with `GetIf()` will always fail and return `false`  
 Attempting to access any child of the `NullObject` yields a reference back to the `NullObject` this allows safetly chaining any number of accesses and `GetIf()`'s  
 **example**
 ```cpp
 fisk::tools::Json root;
 root.Parse(R"({"outer":{"inner":[{"foo":{"value": 4}}]}})");
 
 root["outer"]["inner"][0]["foo"]; // succeeds all the way through

 root["outer"]["missing"][2]["bar"]; // fails at "missing" and then keeps accessing the NullObject until the chain completes
 
 int value;
 if (root["outer"]["inner"][0]["foo"]["value"].GetIf(value))
 {
  //this will execute
 }
 if (root["outer"]["missing"][2]["bar"]["value"].GetIf(value)) // GetIf on the NullObject always fails
 {
  //this will not
 }
 ```
</details>
























