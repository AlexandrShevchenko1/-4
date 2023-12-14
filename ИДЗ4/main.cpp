#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

// global variables
int pot = 0;
string outputFile;
int N, H;
ofstream file;

pthread_mutex_t mutexPot;
pthread_mutex_t mutexFile;
pthread_cond_t condPot;

string getInput(int& N, int& H, int& argc, char* arg[]) {
    if (argc == 1) {
        // won't write to the file in this case
        std::cout << "Enter 2 numbers. First for N, Second for H." << std::endl;
        printf("Input from the console\n");
        std::cin >> N;
        std::cin >> H;
        return "";
    } else if (argc == 2) { // the number of parameters for main > 1 => use command line
        // will write to the file
        // read configuration file
        ifstream file(arg[1]);
        vector<string> data;
        string str;
        while (file >> str) {
            data.push_back(str);
        }
        N = stoi(data[0]);
        H = stoi(data[1]);
        return data[2];
    } else if (argc == 4){
        // will write to the file
        printf("Input from the command line\n");
        printf("%s\n", arg[1]);
        printf("%s\n", arg[2]);
        N = stoi(arg[1]);
        H = stoi(arg[2]);
        return arg[3];
    }
    return "";
}


void* bee(void* arg) {
    while(true) {
        pthread_mutex_lock(&mutexPot);
        ++pot;
        pthread_mutex_lock(&mutexFile);
        // write to file
        file << "A bee brings a sip of honey. Sips in pot: " << to_string(pot) << endl;
        pthread_mutex_unlock(&mutexFile);
        printf("A bee brings a sip of honey. Sips in pot: %d\n", pot);
        pthread_mutex_unlock(&mutexPot);
        // passing signal to bear
        pthread_cond_signal(&condPot);
        sleep(1);
    }
}


void* bear(void* arg) {
    while(true) {
        pthread_mutex_lock(&mutexPot);
        while (pot < H) {
            pthread_mutex_lock(&mutexFile);
            // write to file
            file << "Bear sleeps. Waiting for pot full of honey ...\n";
            pthread_mutex_unlock(&mutexFile);
            printf("Bear sleeps. Waiting for pot full of honey ...\n");
            pthread_cond_wait(&condPot, &mutexPot);
        }
        pot -= H; // bear drinks whole pot;
        pthread_mutex_lock(&mutexFile);
        // write to file
        file << "Bear ate the pot and went to sleep. Sips in pot: " << to_string(pot) << endl;
        pthread_mutex_unlock(&mutexFile);
        printf("Bear ate the pot and went to sleep. Sips in pot: %d\n", pot);
        pthread_mutex_unlock(&mutexPot);
    }
}


int main(int argc, char* arg[])
{
    outputFile = getInput(N, H, argc, arg); // can be nullptr if input was from console
    file.open(outputFile);
    pthread_t th[N+1]; // number of bees and bear
    pthread_mutex_init(&mutexPot, nullptr);
    pthread_mutex_init(&mutexFile, nullptr);
    pthread_cond_init(&condPot, nullptr);
    for (int i = 0; i < N + 1; ++i) {
        if (i) {
            if (pthread_create(th + i, nullptr, &bee, nullptr) != 0) {
                perror("Failed to create thread\n");
            }
        } else {
            if (pthread_create(th, nullptr, &bear, nullptr) != 0) {
                perror("Failed to create thread\n");
            }
        }
    }

    for (int i = 0; i < N + 1; ++i) {
        if (pthread_join(*(th + i), nullptr) != 0) {
            perror("Failed to join thread ...\n");
        }
    }
    pthread_mutex_destroy(&mutexPot);
    pthread_mutex_destroy(&mutexFile);
    pthread_cond_destroy(&condPot);
    file.close();
    return 0;
}
