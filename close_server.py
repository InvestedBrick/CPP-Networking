import socket

def connect_to_server():
    host = "127.0.0.1"  # Server IP address
    port = 4352         # Server port

    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            # Connect to the server
            client_socket.connect((host, port))
            print(f"Connected to {host} on port {port}")

    except ConnectionRefusedError:
        print(f"Connection to {host}:{port} was refused. Is the server running?")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    connect_to_server()