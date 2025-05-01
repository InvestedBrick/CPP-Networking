WARNING: **This code is Linux exculive and has only been tested on Ubuntu**

This currently only listens and connects on 127.0.0.1 (localhost)

To start the server run `make` in `server/` to compile and then run with `./server`

You can also compile the client with `make` 

The server can connect to multiple clients

## Making the server accessible

If you want to allow clients connecting from somewhere other than your own PC, do the following:

- Create a tunnel with pinggy, which should look something like this: `ssh -p 443 -R0:localhost:8080 -o StrictHostKeyChecking=no -o ServerAliveInterval=30 <your access token here>+force+tcp@free.pinggy.io`
- Clients just have to copy the created tunnel ip without the `tcp://` part into `client/connection_target` 