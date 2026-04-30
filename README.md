# Back2me_CampusLostandFound

# Overview 
FindMe is a console-based campus lost and found system that runs entirely in the terminal/command prompt. It provides a text-based interface for students, faculty, and staff to report lost and found item, view, search, mark item as claimed, process claim, find route and exit the system.

# Purpose 
Every day on campus, valuable personal belongings go missing such as, student IDs, laptops, textbooks, water bottles, backpacks, and more. Without a centralized system, finding these items becomes frustrating and time-consuming, through this system it helps everyone easier to find and report their belongings. 

# Data structured and Algorithms used 
The Data Structures used are hash Table for storing the FOUND items, Linked list for storing LOST items, Queue for claim processing (FIFO) and Graph for campus map (for dijkstra) 
The Algorithms used are Searching for viewing the data inside the system and Dijkstra for shortest path 

# How to compile and run the application
To use the FindMe application, simply run the compiled program to see the main menu with nine numbered options. Type the number corresponding to your desired action and press Enter. For example, press 1 to add a lost item, then enter the item ID, description, and select a location number from 0 to 7 (Cultural Hall, Library, Food Court, IT Building, Registrar Office, Gymnasium, OSAS Office, or Gate). Press 2 to add a found item using the same process. Press 3 or 4 to view all lost or found items respectively. Press 5 to search for an item by ID, which will tell you if it's lost or found and its location. Press 6 to mark a found item as claimed, which adds it to a processing queue, then press 7 to process the claim and remove the item from the system. Press 8 to use Dijkstra's algorithm to find the shortest walking route between campus locations, and press 9 to exit the program. The entire application is controlled through simple number inputs, making it easy to manage lost and found items on campus. 
