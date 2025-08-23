# Flow: A Command-Line Chat Server and Client

## Project Overview

**Flow** is a minimalist, command-line chat application designed for seamless and continuous communication. Built entirely from scratch without reliance on GUI libraries, Flow aims to provide a robust and engaging Text-User Interface (TUI) chat experience. Inspired by the idea of an unbroken "flow" of conversation, this project focuses on a clear, efficient, and direct way to connect and interact with others in a terminal environment.

## Vision: The Flow Roadmap

Our journey with Flow is divided into three main phases, building upon core functionalities to create a comprehensive TUI chat experience.

### 🌊 Short-Term Vision (1-2 Days): The Foundational Current

- **Core Objective:** Establish fundamental client-server connection, immediate user identification, text-based public broadcast, and graceful disconnection. This is the absolute first "trickle" of the "Flow," ensuring basic connectivity and attributed messages.

- **Features to Implement:**
  1.  **Basic Client-Server Network Connection:**
      - **Server (`server/ChatServer.cpp`):** Initialize server socket, bind, listen, accept multiple concurrent client connections (using threads).
      - **Client (`client/ChatClient.cpp`):** Initialize client socket, connect to server IP/port, implement basic connection error handling.
  2.  **User Identification (Chosen Username):**
      - **Client:** Immediately prompt the user to "Enter your desired username:".
      - **Server:** Receive the username. Perform a **non-persistent uniqueness check** against _currently connected_ users. If taken, instruct client to choose another. Associate the unique username with the client's connection.
      - **Server Notification:** Broadcast " `<username>` has joined the chat." to all connected clients.
  3.  **Identified Broadcast Messaging:**
      - **Client:** Allow typing text, send as message to server.
      - **Server:** Receive message, broadcast to _all_ other connected clients, prefixed with their chosen username (e.g., `[<chosen_username>]: Message content`).
      - **Client:** Display received messages, appending to the terminal.
  4.  **Graceful Disconnection:**
      - **Client:** Implement `_quit` command or Ctrl+D/Ctrl+C to disconnect. Send signal/command to server.
      - **Server:** Detect client disconnections, remove client and username from active list.
      - **Server Notification:** Broadcast " `<username>` has left the chat." to remaining clients.
  5.  **Basic TUI Input/Output:** Simple line-by-line input. Messages scroll. Use `include/Color.hpp` for basic system messages (e.g., connection status, username issues).

### 💧 Medium-Term Vision (1-2 Weeks): Directed Streams and Personal Currents

- **Core Objective:** Introduce persistent user accounts with authentication, and enable direct, private communication between identified users.

- **Features to Implement:**
  1.  **User Registration and Authentication:**
      - **Server (`user/UserManager.cpp`):** Integrate `UserManager` to persistently store user credentials (username/password, initially in a JSON file).
      - **Client Commands:** `_register <username> <password>` and `_login <username> <password>`.
      - **Authentication Flow:** Clients must successfully `_login` before sending messages. Handshake and credential exchange with the server.
      - **Client Feedback:** Display clear success/failure messages for registration and login.
  2.  **Authenticated Broadcasting:**
      - Once logged in, messages sent by a client will be prefixed with their _authenticated username_ for all broadcasts.
  3.  **Private Messaging (Direct Messages):**
      - **Client Command:** `_msg <recipient_username> <message>`: Send a private message to a specific logged-in user.
      - **Server Logic:** Route private messages exclusively to the specified recipient.
      - **Client Display:** Clearly distinguish private messages from public messages (e.g., `(DM from <sender>): <message>`).
  4.  **Online User List:**
      - **Client Command:** `_who` or `_users`: Request and display a list of all currently online, authenticated users from the server.
  5.  **Enhanced Server Notifications:**
      - " `<Username>` has joined the chat." / " `<Username>` has left the chat." (using actual usernames after login).
      - " `<Username>` sent you a private message." (if not in a dedicated DM mode).
  6.  **Basic Help Command:**
      - **Client Command:** `_help`: Prints a list of all available client commands with a brief explanation.

### 🌐 Long-Term Vision (1+ Months): Deep Pools and Interconnected Deltas

- **Core Objective:** Implement friend management, group chats/channels, persistent message history, and advanced TUI usability to create a comprehensive chat ecosystem.

- **Features to Implement:**
  1.  **Friend Management System:**
      - **Client Commands:** `_friend <username>` (send request), `_pending` (view incoming), `_accept <username>`, `_decline <username>`, `_unfriend <username>`.
      - **Server-Side:** Leverage `UserManager` to persistently store and manage friend relationships.
      - **Notifications:** Real-time notifications for friend request status changes.
  2.  **Group Chats / Channels:**
      - **Server-Side:** Implement logic for multiple chat channels, membership, and message routing.
      - **Client Commands:** `_create <channel_name>`, `_join <channel_name>`, `_leave <channel_name>`, `_channel_say <message>` (send to current channel), `_channels` (list all), `_active_channel <name>` (switch context).
      - **Client Display:** Show channel affiliation (e.g., `[#channel] <user>: message`).
  3.  **Server-Side Persistent Message History (Basic for Channels):**
      - Store a limited history (e.g., 50-100 messages) per channel on the server.
      - When a user `_join`s a channel, send them this recent history for "catch-up."
  4.  **Client-Side Command History:**
      - Allow users to navigate and re-edit previous commands using up/down arrow keys in the terminal.
  5.  **User Status Messages:**
      - **Client Command:** `_status <message>`: Set a custom status (e.g., "AFK," "Busy coding Flow"). Displayable in `_who` and `_profile`.
  6.  **Advanced TUI Context Management (Pseudo-Panels):**
      - **Client Command:** `_focus <channel/user>`: Switch primary input/output to a specific channel or DM.
      - **Client Notifications:** Implement subtle terminal notifications for new messages in _unfocused_ channels/DMs (e.g., a flashing indicator or `(1 new)` count next to the channel/user in a static "active conversations" list, requiring advanced terminal manipulation).
  7.  **Full Persistent Message History (Server-Side - All Messages):**
      - Integrate a robust database (e.g., SQLite, PostgreSQL) to store _all_ public channel messages and _all_ private messages indefinitely.
      - **Client Command:** `_history <channel/user> [count]`: Retrieve comprehensive message history.
  8.  **Searchable Message History:**
      - **Client Command:** `_search <keyword> [channel/user]`: Search through stored messages.

## Prerequisites

To build and run Flow, you will need:

- **C++ Compiler:** G++ (Linux/macOS) or MSVC (Windows).
- **CMake:** Version 3.10 or higher.
- **Git:** For cloning the repository.
- **Docker (Optional):** For containerized deployment.
- **`nlohmann/json` library:** Included in `include/nlohmann/json.hpp`.

## Building and Running Flow

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/flow.git
cd flow
```

### 2. Build the Project

Flow uses CMake for cross-platform compilation.

#### On Linux/macOS:

```bash
mkdir build
cd build
cmake ..
make
```

#### On Windows (using Visual Studio / MSVC):

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" # Or your installed Visual Studio version
cmake --build . --config Release
```

_(Note: Adjust the `-G` flag to match your Visual Studio version, e.g., "Visual Studio 17 2022")_

### 3. Run the Server

From the `build` directory:

#### On Linux/macOS:

```bash
./server/FlowServer <port_number>
```

Example: `./server/FlowServer 8080`

#### On Windows:

```bash
.\Release\FlowServer.exe <port_number>
```

Example: `.\Release\FlowServer.exe 8080`

### 4. Run the Client

Open a new terminal window. From the `build` directory:

#### On Linux/macOS:

```bash
./client/FlowClient <server_ip> <port_number>
```

Example: `./client/FlowClient 127.0.0.1 8080`

#### On Windows:

```bash
.\Release\FlowClient.exe <server_ip> <port_number>
```

Example: `.\Release\FlowClient.exe 127.0.0.1 8080`

### 5. Docker (Optional)

You can also build and run Flow using Docker for a containerized environment.

```bash
docker build -t flow-chat .
docker run -p 8080:8080 flow-chat # This will run the server
# For client, you might need to run another container or connect from host.
```

## Usage (Client Commands)

Once connected, you will be prompted to enter a username. After successfully choosing a unique username, you can start typing messages.

- Simply type your message and press `Enter` to send it to the public broadcast channel.
- `_quit`: Disconnect from the chat server.

_(As the project evolves, more commands will be added as per the roadmap.)_

---

**Happy Flowing!**
