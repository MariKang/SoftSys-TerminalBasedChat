# Proposal for Apellatum: Creating a Chatting Application
Jackie Zeng, Joon Kang, Mari Kang

## Goal of the Project
The goal of this project is to create a chatting application using the TCP protocol in the C language. In addition to standard chatting methods, we will implement text type emojis into the system for them to be sent to other people who are on the same system. 
 
We are planning to base the application on a terminal, but if we have time left over we will create a GUI for better user experience of the application. As mentioned above, the chat application is going to be based on the TCP protocol, which means that it only allows local network connections. 
 
For this problem, we will implement IP configurations for the users that would allow the connection to a host computer to chat to. 
 
MVP: Implement a system where two users can talk to each other in an unrefined fashion. If this is successful, we will move onto more users participating in the chat and have some kind of a cleaned interface that allows the users to understand what state of the application they are in. If we are successful in implementing a full on text based chatting application, then we will move onto creating a GUI for housing the system that would allow users to interact with the software with ease.

## Learning Goals
- Learn about computer networking in general
- Learn about sockets
- Gain experience researching a field we don’t know much about
- Get better at C

## Plan/ Resources
We have some initial resources that we got from softsys projects but we would love it if the teaching team can point us to relevant resources. 
 
https://github.com/shrutiyer/SoftSysServerClient/blob/master/reports/report.md
https://matthewruehle.github.io/SoftSysQuestingQuail/
https://github.com/yorickdewid/Chat-Server
 
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

## First Steps

All of us will complete these tasks:

Research about networking and connections between computers that are implementable with the C language until we fully understand the concepts enough.
Come up with different connection methods - either use a p2p server or use a provided server. 
Create an introduction for the program that announces the ip address of the user, name of the connected WAP, and options to join or create a hosting server on their machine. 
