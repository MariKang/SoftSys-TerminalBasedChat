# SoftSys Terminal Based Chat

## About
A multi-user terminal-based chat application that connects multiple clients to have a group chat using telnet with emoji features that clients can use.

## Table of Contents
>   * [About](#about)
>   * [Table of contents](#table-of-contents)
>   * [Running the Chat Server](#running-the-chat-server)
>   * [Features](#features)


## Installation

1. Clone the repo to a file
```bash
git clone https://github.com/MariKang/SoftSysApellatum.git
```

## Running the Chat Server
Compile with 

```bash
gcc Apellatum.c -o Apellatum
```
And run the chat server using

```bash
./Apellatum
```

## Features
Client can set their username, and see other clients joining the chat and their usernames.
![image 1](reports/images/client_start.png)

Clients can send their messages to all the other clients. The client's own username is not shown, but the input messages from other clients have username at the front. The clients can also send multiple message at once because each client has thread of its own and the order of sending message doesn't matter.
![image 2](reports/images/multiuser_chat.png)

Emojis are sent using brackets([]). There are three types of Emoji, [BEAR], [WHAT] and [HUH], each replaced with different emoji.
![image 3](images/receiving_emoji.png)
