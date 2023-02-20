#include <iostream>
#include <chrono>
#include <thread>
#include <queue>

using namespace std;

const int NUM_KSLACKS = 5;

queue<int> request_queue; // Queue to store requests for the synchronizer
int next_kslack = 0; // Index of the next kslack to process
bool stop_processing_requests = false; // Flag to indicate whether to stop processing requests


void synchronizer_function() {
    // Process requests for the synchronizer
    while (stop_processing_requests) {
        // Wait for a request to arrive
        while (request_queue.empty()) {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        // Process the next request
        int kslack_id = request_queue.front();
        request_queue.pop();

        // Access the synchronizer
        cout << "kslack " << kslack_id << " accessed the synchronizer" << endl;

        // Sleep for a random amount of time to simulate work being done
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        // Release the synchronizer

        // Check if there are more requests waiting to be processed
        if (!request_queue.empty()) {
            // Signal the next kslack to proceed
            int next_id = request_queue.front();
            cout << "signaling kslack " << next_id << endl;
        }
    }
}

void kslack_function(int id) {
    // Each kslack tries to access the synchronizer multiple times
    for (int i = 0; i < 3; i++) {
        // Send a request to access the synchronizer
        request_queue.push(id);

        // Wait for permission to access the synchronizer
        while (next_kslack != id) {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        // Access the synchronizer
        cout << "kslack " << id << " got permission to access the synchronizer" << endl;

        // Release the synchronizer
        next_kslack = (next_kslack + 1) % NUM_KSLACKS;
        cout << "kslack " << id << " released the synchronizer" << endl;

        // Sleep for a random amount of time before trying again
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
    }
}

int main() {
    using clock = std::chrono::steady_clock;
    clock::time_point start = clock::now();
    // A long task...

    // Start the synchronizer thread
    thread sync_thread(synchronizer_function);

    // Start multiple kslack threads
    vector<thread> kslack_threads;
    for (int i = 0; i < NUM_KSLACKS; i++) {
        kslack_threads.emplace_back(kslack_function, i);
    }

    // Join all threads
    for (auto &t: kslack_threads) {
        t.join();
    }
    sync_thread.join();

    // Set the flag to true to signal the synchronizer to stop processing requests
    stop_processing_requests = true;

    clock::time_point end = clock::now();
    clock::duration execution_time = end - start;

    std::cout << execution_time.count() << std::endl;
    return 0;
}