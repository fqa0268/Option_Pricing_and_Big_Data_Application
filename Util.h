#pragma once
#include <string>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <map>

using namespace std;
const int BUFFUR_SIZE = 2048;
const int DATE_SIZE = 15;
const int TIME_SIZE = 13;
const string TRADING_DATE = "21-MAY-2012";

typedef map<string, pair<long, float>> Data;

void GetTimeIntervals(vector<pair<string, string>> &time_pairs)
{
    string hour_local[8] = {"09", "10", "11", "12", "13", "14", "15", "16"};
    string hour_gmt[8] = {"13", "14", "15", "16", "17", "18", "19", "20"};  // GMT-4
    string min[4] = {"00", "15", "30", "45"};
    string time_local, time_gmt;
    for (int h = 0; h < sizeof(hour_local) / sizeof(hour_local[0]); h++)
    {
        for (int m = 0; m < sizeof(min) / sizeof(min[0]); m++)
        {
            time_local = hour_local[h] + ":" + min[m] + ":00";
            time_gmt = hour_gmt[h] + ":" + min[m] + ":00";
            time_pairs.push_back(pair(time_local, time_gmt));
        }
    }
    time_pairs.erase(time_pairs.begin(),time_pairs.begin()+2);
    time_pairs.erase(time_pairs.end() - 2, time_pairs.end());
}

string GetTimeSlot(char *time_gmt, vector<pair<string, string>> &time_pairs)
{
    string time_str = string(time_gmt);
    for(int i = 0; i < time_pairs.size(); i++)
    {
        if (time_str.compare(time_pairs[i].second) == 0)
            return time_pairs[i].first;
        else if (time_str.compare(time_pairs[i].second) < 0)
            return time_pairs[i-1].first;
    }
    cout << "TimeSlot Error " << time_gmt << endl;
    return "-1";
}

int ExtractData(string file_name, vector<pair<string, string>> &time_pairs, Data &table)
{
    FILE * fin = fopen(file_name.c_str(), "r");

    if (fin == NULL) {
        cerr << "Error: Could not open the file." << endl;
        return -1;
    }

    string line;
    char buffer[BUFFUR_SIZE], date1[DATE_SIZE], date2[DATE_SIZE], time_gmt[TIME_SIZE];
    char *found, *p1, *p2;
    int trading_days = 0;
    long volumn;
    float price;
    string time_slot;
    
    while (fgets(buffer, BUFFUR_SIZE, fin))
    {
//        getline(fin, line);
//        strcpy(buffer, line.c_str());
        found = strstr(buffer, "Trade");
        if (found != NULL)
        {
            p1 = strchr(buffer, ',') + 1;
            p2 = strchr(p1, ',');
            *p2 = '\0';
            strncpy(date1, p1, DATE_SIZE);
            
            p1 = p2 + 1;
            p2 = strchr(p1, ',');
            *p2 = '\0';
            strncpy(time_gmt, p1, TIME_SIZE);

            if (string(time_gmt).compare(time_pairs.front().second) < 0 || string(time_gmt).compare(time_pairs.back().second) >= 0 )
                continue;
            
            p1 = p2 + 1;
            p1 = strchr(p1, ',') + 1;
            p1 = strchr(p1, ',') + 1;
            p2 = strchr(p1, ',');
            *p2 = '\0';
            price = atof(p1);
            
            p1 = p2 + 1;
            p2 = strchr(p1, ',');
            *p2 = '\0';
            volumn = atoi(p1);
            
            time_slot = GetTimeSlot(time_gmt, time_pairs);
            
            if (string(date1).compare(TRADING_DATE) < 0){
                table[time_slot].first += volumn;
            }
            else if (string(date1).compare(TRADING_DATE) == 0)
            {
                if ((int) table[time_slot].second == 0 )
                    table[time_slot].second = price;
            }
            else
                break;
            
            if (strncmp(date1, date2, DATE_SIZE) != 0)
            {
                trading_days++;
                strncpy(date2, date1, DATE_SIZE);
            }
        }
    }
    trading_days--;
    
    for(Data::iterator it = table.begin(); it != table.end(); it++)
        (*it).second.first /= trading_days;
    
    return 0;
}


void OutputToFile(vector<pair<string, string>> &time_pairs, Data &table)
{
    ofstream output_file;
    output_file.open ("outputs.csv");
    output_file << "Time Interval (NYC Local Time), Quatity (Avg Qty from 5/1-5/20), Price (Trading Price from 5/21)" << endl;

    for(vector<pair<string, string>>::iterator it = time_pairs.begin(); it != time_pairs.end()-1; it++)
    {
        output_file << (*it).first << ',' << table[(*it).first].first << ',' << fixed << setprecision(2) << table[(*it).first].second << endl;
    }
    output_file.close();
}
