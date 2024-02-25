import socket
import time
import sys

def create_socket_connection(hostname, port, duration=10, max_connections=100000):
    start_time = time.time()
    connections = []  # List to keep track of established connections

    while True:
        current_time = time.time()
        elapsed_time = current_time - start_time
        if elapsed_time > duration:
            print("Failsafe triggered: Exiting after 10 seconds of looping connections.")
            break

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.connect((hostname, port))
            sock.settimeout(None)
            print(f"Connection to {hostname} on port {port} established.")
            connections.append(sock)  # Add the new connection
            if len(connections) > max_connections:
                # Remove and close the oldest connection to maintain the limit
                oldest_conn = connections.pop(0)
                oldest_conn.close()
                print(f"Closed the oldest connection to maintain the limit of {max_connections} connections.")
        except ConnectionRefusedError:
            pass
        except socket.timeout:
            print(f"Connection attempt to {hostname} on port {port} timed out.")
        except Exception as e:
            print(f"An error occurred: {e}")
        
        time.sleep(0.000000001)  # Sleep for 0.001 seconds before trying again

    # Close any remaining connections on script exit
    while connections:
        conn = connections.pop(0)
        conn.close()
    print("All connections have been closed.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python socket_connection.py <hostname> <port>")
        sys.exit(1)
    
    hostname = sys.argv[1]
    try:
        port = int(sys.argv[2])
    except ValueError:
        print("Error: Port must be an integer.")
        sys.exit(1)
    
    create_socket_connection(hostname, port)
