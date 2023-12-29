#include "PricingModel.h" 
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;
namespace fre 
{
    double BinomialTreeModel::RiskNeutProb() const
    { 
        return (R - D) / (U - D);
    }
    
    double BinomialTreeModel::CalculateAssetPrice(int n, int i) const
    {
        return S0 * pow(U, i) * pow(D, n - i);
    }
    
    void BinomialTreeModel::UpdateBinomialTreeModel(double S0_, double U_, double D_, double R_) { S0 = S0_; U = U_; D = D_; R = R_; }
    int BinomialTreeModel::GetInputData() 
    {
        //entering data
        cout << "Enter S0: "; 
        cin >> S0; 
        cout << "Enter U: "; 
        cin >> U; 
        cout << "Enter D: "; 
        cin >> D; 
        cout << "Enter R: "; 
        cin >> R; 
        cout << endl;
        //making sure that 0<S0, -1<D<U, -1<R
        if (S0<=0.0 || U<=-1.0 || D<=-1.0 || U<=D || R<=-1.0)
        {
            cout << "Illegal data ranges" << endl; 
            cout << "Terminating program" << endl; 
            return -1;
        }
        
        //checking for arbitrage 
        if (R>=U || R<=D)
        {
            cout << "Arbitrage exists" << endl;
            cout << "Terminating program" << endl; 
            return -1;
        }
        cout << "Input data checked" << endl;
        cout << "There is no arbitrage" << endl << endl;
        return 0; 
    }
    
    int BinomialTreeModel::ValidateInputData() const 
    {
        //making sure that S0>0, U>D>0, R>0 
        if (S0 <= 0.0 || U <= 0.0 || D <= 0.0 || U <= D || R <= 0.0)
        {
            cout << "Illegal data ranges" << endl; 
            cout << "Terminating program" << endl; 
            return -1;
        }
        //checking for arbitrage 
        if (R >= U || U <= D)
        {
            cout << "Arbitrage exists" << endl;
            cout << "Terminating program" << endl; 
            return -1;
        }
        cout << "Input data checked" << endl;
        cout << "There is no arbitrage" << endl << endl;
        return 0; 
    }

    const double pi=4.0*atan(1.0);
    double Gauss()
    {
        double U1 = (rand() + 1.0) / (RAND_MAX + 1.0);
        double U2 = (rand() + 1.0) / (RAND_MAX + 1.0);
        return sqrt(-2.0 * log(U1)) * cos(2.0 * pi * U2);
    }

    void MCModel::GenerateSamplePath(double T, int m, SamplePath& S) const
    {
        double St = S0;
        for (int k = 0; k < m; k++)
        {
            S[k] = St * exp((r - sigma * sigma * 0.5) * (T / m) + sigma * sqrt(T / m) * Gauss());
            St = S[k];
            
        }
    }
}
