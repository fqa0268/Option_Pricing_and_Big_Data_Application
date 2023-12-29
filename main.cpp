#include <iostream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>

#include "Util.h"
#include "PricingModel.h"
#include "Option.h"
using namespace std;
using namespace fre;

int main() {
    
    auto start = time(NULL);

    vector<pair<string, string>> time_pairs;
    GetTimeIntervals(time_pairs);

    string file_name = "SPY.csv";
    Data table;
    for(vector<pair<string, string>>::iterator it = time_pairs.begin(); it != time_pairs.end(); it++)
        table[(*it).first] = pair(0, 0.0);

    ExtractData(file_name, time_pairs, table);
    OutputToFile(time_pairs, table);
    
    
    int N = 8;
    double U = 1.15125, D = 0.86862, R = 1.00545;
    double S0, K = 130;
    double K1 = 120, K2 = 140;
    
    double r = R - 1, sigma=0.2;
    
    ofstream output_file;
    output_file.open ("outputs.csv");
    output_file << "Time Interval (NYC Local Time), Quatity (Avg Qty from 5/1-5/20), Price (Trading Price from 5/21), ";
    output_file << "European Call, European Put, Bull Spread, Bear Spread, Strangle, Straddle, Butterfly, American Call, American Put, Asian Call, Error, Asian Call with Control Variates, Error" << endl;

    for(vector<pair<string, string>>::iterator it = time_pairs.begin(); it != time_pairs.end()-1; it++)
    {
        output_file << (*it).first << ',' << table[(*it).first].first << ',' << fixed << setprecision(2) << table[(*it).first].second << ',';
        
        S0 = table[(*it).first].second;
        BinomialTreeModel Model(S0, U, D, R);
        
        Call call(N, K);
        output_file << fixed << setprecision(2) << call.PriceByCRR(Model) << ',';
        
        Put put(N, K);
        output_file << fixed << setprecision(2) << put.PriceByCRR(Model) << ',';
        
        BullSpread bullSpread(N, K1, K2);
        output_file << fixed << setprecision(2) << bullSpread.PriceByCRR(Model) << ',';
        
        BearSpread bearSpread(N, K1, K2);
        output_file << fixed << setprecision(2) << bearSpread.PriceByCRR(Model) << ',';
        
        Strangle strangle(N, K1, K2);
        output_file << fixed << setprecision(2) << strangle.PriceByCRR(Model) << ',';
        
        Straddle straddle(N, K);
        output_file << fixed << setprecision(2) << straddle.PriceByCRR(Model) << ',';
        
        Butterfly butterfly(N, K1, K2);
        output_file << fixed << setprecision(2) << butterfly.PriceByCRR(Model) << ',';
        
        output_file << fixed << setprecision(2) << call.PriceBySnell(Model) << ',';
        
        output_file << fixed << setprecision(2) << put.PriceBySnell(Model) << ',';
        
        MCModel mcModel(S0,r,sigma);
        double T =1.0/12.0;
        int m=22;
        long N_simul=30000;
        
        ArthmAsianCall ArthOption(T,K,m);
        output_file << fixed << setprecision(2) << ArthOption.PriceByMC(mcModel, N_simul) << ',';
        output_file << fixed << setprecision(4) << ArthOption.GetPricingError() << ',';
        
        GmtrAsianCall CVOption(T,K,m);
        output_file << fixed << setprecision(2) << ArthOption.PriceByVarRedMC(mcModel,N_simul,CVOption) << ',';
        output_file << fixed << setprecision(4) << ArthOption.GetPricingError() << endl;

    }
    
    output_file.close();
    
    auto end = time(NULL);
    cout << "Time taken to fetch and calculate all the data: " << (end - start) << " seconds" << endl;
    
    return 0;
}
