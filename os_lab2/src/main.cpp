#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sys/wait.h>

using namespace std;

int main() {

	int fds[2];
    if(pipe(fds) != 0) {
        perror("pipe");
    }

    int id = fork();
    if (id == -1) {
        return -1;
    }



    if (id == 0) {
		string filename;
        int len;
        read(fds[0], &len, sizeof(int));

        for (int i = 0; i < len; i++) {
            char c;
            read(fds[0], &c, sizeof(char));
            filename.push_back(c); 
        }

        ofstream outfile(filename);

        int t;
        read(fds[0], &t, sizeof(int));
        int amount;
        read(fds[0], &amount, sizeof(int));

        int sum = 0;
        for (int i = 0; i < amount; i++) {
            int n;
            read(fds[0], &n, sizeof(int));
            sum += n;
        }

        for (int i = 0; i < t; i++) {
            int amount;
            read(fds[0], &amount, sizeof(int));
            int sum = 0;
            for (int i = 0; i < amount; i++) {
                int n;
                read(fds[0], &n, sizeof(int));
                sum += n;
            }
            outfile << sum << endl;
        }

        outfile.close();
        close(fds[0]);
        close(fds[1]);
    }
	


    else {   
		string filename;

        cout << "Parent's PID: " << getpid() << endl;
        cout << "Child's PID: " << id << endl;
        vector<int> numbers;
        cout << "Enter the file name:\n";
        cin >> filename;

        int length = filename.length();
        write(fds[1], &length, sizeof(int));
        for (int i = 0; i < length; i++) {
            write(fds[1], &filename[i], sizeof(char));
        }

        cout << "Enter amount of commands:\n";
        int comm;
        cin >> comm;
        comm++;
        write(fds[1], &comm, sizeof(int));


        for (int i = 0; i < comm; i++)
        {
            string s;
            getline(cin, s);
            vector<int> numbers;
            string n;

            for (int i = 0; i < s.length(); i++) {
                if ((s[i] != ' ') || (s[i] != '\0')) {
                    n.push_back(s[i]);
                }
                if ((s[i] == ' ') || (s[i] == '\0') || (s[i] == '\n') || (i == s.length() - 1)) {
                    int num = stoi(n);
                    n = "";
                    numbers.push_back(num);
                }
            }
			
            int amount = numbers.size();
            write(fds[1], &amount, sizeof(int));
            for (int i = 0; i < amount; i++) {
                write(fds[1], &numbers[i], sizeof(int));
            }
        }


        close(fds[1]);
        close(fds[0]);
    }
    return 0;
}
