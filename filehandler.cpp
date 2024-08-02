#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

void fill_buffer_a(string s, char *curr_buff)
{
    int n = s.length();

    for (int i = 0; i < n; i++)
    {
        curr_buff[i] = s[i];
    }
    return;
}

char *get_from_txt(string a, int verbose = 0)
{
    string file_contents = "";

    ifstream my_file(a);

    string curr_line;
    while (getline(my_file, curr_line))
    {
        file_contents += curr_line;
        file_contents += '\n';
    }

    if (verbose)
    {
        cout << file_contents << endl;
    }

    int file_size = file_contents.length();

    char *arr = new char[file_size];

    fill_buffer_a(file_contents, arr);

    return arr;
}

string write_to_txt(string contents, string identifier)
{
    string final_name = "/resources/" + identifier + ".txt";
    ofstream new_file(final_name);

    new_file << contents;
    new_file.close();

    return final_name;
}