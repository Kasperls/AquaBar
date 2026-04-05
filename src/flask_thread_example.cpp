// Alternative simpler approach using std::thread
// Add this instead of the fork() approach above:

#include <thread>
#include <future>

// In main(), replace the fork section with:
std::atomic<bool> flask_running = true;

// Function to run Flask server
void run_flask_server() {
    while (flask_running) {
        std::string cmd = "cd /home/piaqua/Desktop/AquaBar/python && python3 flask_server.py";
        int result = system(cmd.c_str());

        if (result != 0 && flask_running) {
            std::cerr << "Flask server exited with error, restarting in 5 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        } else if (!flask_running) {
            break;
        }
    }
}

// Start Flask server thread
std::thread flask_thread(run_flask_server);

// Before shutdown, stop Flask
flask_running = false;
if (flask_thread.joinable()) {
    flask_thread.join();
}