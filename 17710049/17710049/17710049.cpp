//Lufuno Mulaudzi std no: 17710049
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
#include <conio.h>

using namespace std;

struct Passenger {
    char route;
    int processingTime;
    int remainingTime;
};

struct Taxi {
    char route;
    const int capacity = 5;
    int availableSeats = 5;
    stack<Passenger> passengers;
};

// Function to print the header for the table
void static printHeader() {
    cout << left
        << setw(6) << "T"
        << setw(10) << "Next"
        << setw(6) << "S"
        << setw(6) << "L"
        << setw(6) << "C"
        << setw(8) << "WQS"
        << setw(8) << "WQL"
        << setw(8) << "WQC"
        << setw(6) << "CS"
        << setw(6) << "CL"
        << setw(6) << "CC"
        << endl;
}
// Function to print a single row of simmulation data
void static printRow(int time,const string& next,const string& s, const string& l, const string& c,int wqs, int wql, int wqc,int cs, int cl, int cc) {
    
   cout << left
        << setw(6) << time
        << setw(10) << next
        << setw(6) << s
        << setw(6) << l
        << setw(6) << c
        << setw(8) << wqs
        << setw(8) << wql
        << setw(8) << wqc
        << setw(6) << cs
        << setw(6) << cl
        << setw(6) << cc
        << endl;
}
// Function to run the taxi queue simulation
void static processTaxiRank(const string& fileName) {
    // Check if the file exists first
    if (!filesystem::exists(fileName)) { 
        cerr << "Error File not found at " << fileName << endl;
        return;
    }
    // Queues for each route
    queue<Passenger> sQueue, lQueue, cQueue;
    queue<Passenger> sWaitingQueue, lWaitingQueue, cWaitingQueue;

    Taxi sTaxi = { 'S' };
    Taxi lTaxi = { 'L' };
    Taxi cTaxi = { 'C' };

    Passenger currentS = { ' ', 0, 0 };
    Passenger currentL = { ' ', 0, 0 };
    Passenger currentC = { ' ', 0, 0 };

    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cerr << "Error Can not open the File " << fileName << endl;
        return;
    }

    printHeader();
    printRow(0, "", "", "", "", 0, 0, 0, 0, 0, 0);

    int time = 0;
    string line;

    while (getline(inputFile, line) || !sQueue.empty() || !lQueue.empty() || !cQueue.empty() ||currentS.route != ' ' || currentL.route != ' ' || currentC.route != ' ') {

        time++;
        string nextPassenger = "";

        // Read in the next passenger if available
        if (getline(inputFile, line)) {
            size_t comma1 = line.find(',');
            size_t coma2 = line.find(',', comma1 + 1);

            int arivalTime = stoi(line.substr(0, comma1));
            char route = line[comma1 + 1];
            int processingTime = stoi(line.substr(coma2 + 1));

            Passenger p = { route, processingTime, processingTime };

            switch (route) {
            case 'S': sQueue.push(p); break;
            case 'L': lQueue.push(p); break;
            case 'C': cQueue.push(p); break;
            }

            if (arivalTime == time) {
                nextPassenger = string(1, route) + "(" + to_string(processingTime) + ")";
            }
        }

        // Lambda to handle each route boarding
        auto processRoute = [time](queue<Passenger>& mainQ, queue<Passenger>& waitingQ, Passenger& current, Taxi& taxi) -> pair<string, int> {
                string status = "";
                int waitingCount = waitingQ.size();

                if (current.route == ' ' && !mainQ.empty()) {
                    current = mainQ.front();
                    mainQ.pop();
                    status = string(1, current.route) + "(" + to_string(current.remainingTime) + ")";

                    // Passengers behind the current one must wait
                    while (!mainQ.empty()) {
                        waitingQ.push(mainQ.front());
                      mainQ.pop();
                        waitingCount++;
                    }
                }

                if (current.route != ' ') {
                  current.remainingTime--;
                if (current.remainingTime == 0) {
                        taxi.passengers.push(current);
                        taxi.availableSeats--;
                 if (taxi.passengers.size() == taxi.capacity) {
                         cout << "Taxi " << taxi.route << " departed with " << taxi.capacity << " passengers at time " << time << endl;
                      taxi.passengers = {};
                     taxi.availableSeats = taxi.capacity;
                        }
                        current = { ' ', 0, 0 };
                    }
                }
                return { status, waitingCount };
            };

      auto [sStatus, wqs] = processRoute(sQueue, sWaitingQueue, currentS, sTaxi);
      auto [lStatus, wql] = processRoute(lQueue, lWaitingQueue, currentL, lTaxi);
       auto [cStatus, wqc] = processRoute(cQueue, cWaitingQueue, currentC, cTaxi);

        printRow(time, nextPassenger,sStatus, lStatus, cStatus,wqs, wql, wqc,sTaxi.capacity - sTaxi.availableSeats,lTaxi.capacity - lTaxi.availableSeats,
            cTaxi.capacity - cTaxi.availableSeats);
    }

    inputFile.close();
}

int main() {

    string filepath = "C:\\data\\taxiData.txt";
    processTaxiRank(filepath);

    cout << "\nSimulation complete. Press any key to exit...";
    _getch();

    return 0;
}