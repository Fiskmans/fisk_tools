## Sniffer

Small example project of how to create a server that listens to data

Listens to tcp port 12345, prints when a new connection is made, prints when one is closed, prints any data that comes in over tcp, prints when any close. Closes gracefully if the listen socket gets closed