WARNING: **This code is Linux exclusive and has only been tested on Ubuntu**

To start the server run `make` in `server/` to compile and then run with `./server`

You can also compile the client with `make` 

The server can connect to multiple clients

## Making the server accessible

If you want to allow clients connecting from somewhere other than your own PC, do the following:
- install Rust and install bore with `cargo install bore-cli`
- run `bore local 8080 --to bore.pub` to open a tunnel
- paste the tunnel link into `client/connection_target.txt`
- start the server by running `./server` in `server/`