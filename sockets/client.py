import socket
import threading


# Function to handle sending data
def send_data(host, port, data):
    try:
        print("Sending to server")
        # Create a socket connection
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))

        print("connected to server")
        # Send data to the server
        sock.sendall(data.encode())

        # Receive response (optional)
        response = sock.recv(10)  # Adjust buffer size as needed
        print(f"Received from server: {response.decode()}")

    except Exception as e:
        print(f"Error sending data: {e}")
    else:
        sock.close()


# Main function
def main():
    # Server host and port
    # host = "127.0.0.1"  # Change to your server's host
    host = "localhost"  # Change to your server's host
    port = 3496  # Change to your server's port

    # Data to be sent (example)
    data_list = [f"Hello from Thread {i}" for i in range(15)]

    # Create threads for sending data
    threads = []
    for data in data_list:
        thread = threading.Thread(target=send_data, args=(host, port, data))
        threads.append(thread)
        thread.start()

    # Wait for all threads to complete
    for thread in threads:
        thread.join()


if __name__ == "__main__":
    main()
