#include <iostream>
#include <cmath>
#include <cmath>
#include "Option.h"
#include "PricingModel.h"
using namespace std;
namespace fre 
{
    double Option::PriceByCRR(const BinomialTreeModel& Model)
    {
        double optionPrice = 0.0;
        double q = Model.RiskNeutProb();
        vector<double> PricePath(N + 1);
        for (int i = 0; i <= N; i++)
            {
                PricePath[i] = Payoff(Model.CalculateAssetPrice(N, i));
            }
        for (int n = N - 1; n >= 0; n--)
        {
            for (int i = 0; i <= n; i++)
                PricePath[i] = (q * PricePath[i + 1] + (1 - q) * PricePath[i]) / Model.GetR();
        }
        optionPrice = PricePath[0];
        return optionPrice;
    }

    double Option::PriceBySnell(const BinomialTreeModel& Model, BinLattice & PriceTree)
    {
        double q = Model.RiskNeutProb();
        double ContVal = 0;
        for (int i = 0; i <= N; i++)
        {
            PriceTree.SetNode(N, i, Payoff(Model.CalculateAssetPrice(N, i)));
        }
        for (int n = N - 1; n >= 0; n--) {
            for(int i=0;i<=n;i++)
            {
                ContVal = (q * PriceTree.GetNode(n + 1, i + 1) + (1 - q) * PriceTree.GetNode(n + 1, i)) / Model.GetR();
                PriceTree.SetNode(n, i, Payoff(Model.CalculateAssetPrice(n, i)));
                if (ContVal > PriceTree.GetNode(n, i))
                {
                    PriceTree.SetNode(n, i, ContVal);
                }
            }
        }
        return PriceTree.GetNode(0, 0);
    }

    double Option::PriceBySnell(const BinomialTreeModel& Model)
    {
        BinLattice PriceTree(N);
        return PriceBySnell(Model, PriceTree);
    }

    double Call::Payoff(double z) const
    { 
        if(z>K) return z-K;
        return 0.0; 
    }
    
    double Put::Payoff(double z) const 
    { 
        if(z<K) return K-z;
        return 0.0; 
        
    }
    
    double BullSpread::Payoff(double z) const 
    { 
        if(z<=K1) return 0;
        if(z>=K2) return K2-K1;
        return z-K1;
    }
    
    double BearSpread::Payoff(double z) const 
    { 
        if(z<=K1) return K2-K1;
        if(z>=K2) return 0;
        return K2-z;
    }

    double Strangle::Payoff(double z) const
    {
        if (z<=K1) return K1-z;
        if (z>K2) return z-K2;
        return 0.0;
    }

    double Straddle::Payoff(double z) const
    {
        if (z<=K) return K-z;
        return z-K;
    }

    double Butterfly::Payoff(double z) const
    {
        if (K1<z && z<=(K1+K2)/2) return z-K1;
        if ((K1+K2)/2<z && z<=K2) return K2-z;
        return 0.0;
    }
    
    double DoubDigitOpt::Payoff(double z) const
    {
        if (K1 < z && z < K2) return 1.0;
        return 0.0;
    }
    
    double N(double x)
    {
        double gamma = 0.2316419;
        double a1 = 0.319381530;
        double a2 = -0.356563782;
        double a3 = 1.781477937;
        double a4 = -1.821255978;
        double a5 = 1.330274429;
        double pi = 4.0 * atan(1.0);
        double k = 1.0 / (1.0 + gamma * x);
        if (x >= 0.0)
            return 1.0 - ((((a5 * k + a4) * k + a3) * k + a2) * k + a1) * k * exp(-x * x / 2.0) / sqrt(2.0 * pi);
        return 1.0 - N(-x);
    }

    double BSEurOption::d_plus(double S0, double sigma, double r)
    {
        return (log(S0 / K) + (r + 0.5 * pow(sigma, 2.0)) * T) / (sigma * sqrt(T));
    }

    double BSEurOption::d_minus(double S0, double sigma, double r)
    {
        return d_plus(S0, sigma, r) - sigma * sqrt(T);
    }

    double BSEurOption::CallPriceByBSFormula(double S0, double sigma, double r)
    {
        return S0 * N(d_plus(S0, sigma, r)) - K * exp(-r * T) * N(d_minus(S0, sigma, r));
    }

    double BSEurOption::PutPriceByBSFormula(double S0, double sigma, double r)
    {
        return K * exp(-r * T) * N(-d_minus(S0, sigma, r)) - S0 * N(-d_plus(S0, sigma, r));
    }

    double PathDepOption::PriceByMC(const MCModel& Model, long N)
    {
        double H = 0.0, Hsq = 0.0;
        SamplePath S(m);
        for (long i = 0; i < N; i++)
        {
            Model.GenerateSamplePath(T, m, S);
            H = (i * H + Payoff(S)) / (i + 1.0);
            Hsq = (i * Hsq + pow(Payoff(S), 2.0)) / (i + 1.0);
        }
        Price = exp(-Model.GetR() * T) * H;
        PricingError = exp(-Model.GetR() * T) * sqrt(Hsq - H * H) / sqrt(N - 1.0);
        return Price;
    }
    double PathDepOption::PriceByVarRedMC(const MCModel& Model, long N, PathDepOption& CVOption)
    {
        DifferenceOfOptions VarRedOpt(T, K, m, this, &CVOption);
        Price = VarRedOpt.PriceByMC(Model, N) + CVOption.PriceByBSFormula(Model);
        PricingError = VarRedOpt.PricingError;
        return Price;
    }

    double ArthmAsianCall::Payoff(const SamplePath& S) const
    {
        double Ave = 0.0;
        for (int k = 0; k < m; k++)
            Ave = (k * Ave + S[k]) / (k + 1.0);
        if (Ave < K)
            return 0.0;
        return Ave - K;
    }

    double GmtrAsianCall::Payoff(const SamplePath& S) const
    {
        double Prod = 1.0;
        for (int i = 0; i < m; i++)
        {
            Prod = Prod * S[i];
        }
        if (pow(Prod, 1.0 / m) < K)
            return 0.0;
        return pow(Prod, 1.0 / m) - K;
    }

    double GmtrAsianCall::PriceByBSFormula(const MCModel& Model)
    {
        double a = exp(-Model.GetR() * T) * Model.GetS0() * exp((m + 1.0) * T / (2.0 * m) * (Model.GetR() + Model.GetSigma() * Model.GetSigma() * ((2.0 * m + 1.0) / (3.0 * m) - 1.0) / 2.0));
        double b = Model.GetSigma() * sqrt((m + 1.0) * (2.0 * m + 1.0) / (6.0 * m * m));
        BSEurOption G(T, K);
        Price = G.CallPriceByBSFormula(a, b, Model.GetR());
        return Price;
    }
}
