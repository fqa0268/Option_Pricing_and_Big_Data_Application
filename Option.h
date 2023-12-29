#pragma once
#include "PricingModel.h"
#include "BinLattice.h"

namespace fre 
{
    class Option
    {
    private:
        Option() : N(0) {}
        Option(const Option& option) : N(option.N) {}
    protected:
        int N;
    public:
        Option(int N_) : N(N_) {}
        virtual ~Option() {}
        int GetN() const { return N; }
        virtual double Payoff(double z) const = 0;
        double PriceByCRR(const BinomialTreeModel& Model);
        double PriceBySnell(const BinomialTreeModel& Model);
        double PriceBySnell(const BinomialTreeModel& Model, BinLattice & PriceTree);
    };
    
    class Call : public Option
    {
    private:
        double K; 
    public:
        Call(int N_, double K_) : Option(N_), K(K_) {}
        ~Call() {}
        double Payoff(double z) const;
    };
    
    class Put : public Option
    {
    private:
        double K; 
    public:
        Put(int N_, double K_) : Option(N_) , K(K_) {}
        ~Put() {}
        double Payoff(double z) const;
    };
    
    class BullSpread : public Option
    {
    private:
        double K1, K2; 
    public:
        BullSpread(int N_, double K1_, double K2_) : Option(N_), K1(K1_), K2(K2_) {}
        ~BullSpread() {}
        double Payoff(double z) const;
    };
    
    class BearSpread : public Option
    {
    private:
        double K1, K2; 
    public:
        BearSpread(int N_, double K1_, double K2_) : Option(N_), K1(K1_), K2(K2_) {}
        ~BearSpread() {}
        double Payoff(double z) const;
    };

    class Strangle : public Option
    {
    private:
        double K1;
        double K2;
    public:
        Strangle(int N_, double K1_, double K2_) : Option(N_), K1(K1_), K2(K2_) {};
        ~Strangle() {}
        double Payoff(double z) const;
    };

    class Straddle : public Option
    {
    private:
        double K;
    public:
        Straddle(int N_, double K_) : Option(N_), K(K_) {};
        ~Straddle() {}
        double Payoff(double z) const;
    };

    class Butterfly : public Option
    {
    private:
        double K1;
        double K2;
    public:
        Butterfly(int N_, double K1_, double K2_) : Option(N_), K1(K1_), K2(K2_) {};
        ~Butterfly() {}
        double Payoff(double z) const;
    };

    class DoubDigitOpt : public Option
    {
    private:
        double K1;
        double K2;
    public:
        DoubDigitOpt(int N_, double K1_, double K2_) : Option(N_), K1(K1_), K2(K2_) {};
        ~DoubDigitOpt() {}
        double Payoff(double z) const;
    };
        
    double N(double x);
    class BSEurOption
    {
    private:
        double T, K;
        double d_plus(double S0, double sigma, double r);
        double d_minus(double S0, double sigma, double r);
    public:
        BSEurOption(double T_, double K_) : T(T_), K(K_) {}
        double CallPriceByBSFormula(double S0, double sigma, double r);
        double PutPriceByBSFormula(double S0, double sigma, double r);
    };
    
    class PathDepOption
    {
    protected:
        double Price, PricingError;
        int m;
        double K;
        double T;
    public:
        PathDepOption(double T_, double K_, int m_) :Price(0.0), PricingError(0.0), T(T_), K(K_), m(m_) {}
        virtual ~PathDepOption() {}
        virtual double Payoff(const SamplePath& S) const = 0;
        double PriceByMC(const MCModel& Model, long N);
        double PriceByVarRedMC(const MCModel& Model, long N, PathDepOption& CVOption);
        virtual double PriceByBSFormula(const MCModel& Model) { return 0.0; }
        double GetT() { return T; }
        double GetPrice() { return Price; }
        double GetPricingError() { return PricingError; }
    };
    
    class DifferenceOfOptions : public PathDepOption
    {
    private:
        PathDepOption* Ptr1;
        PathDepOption* Ptr2;
    public:
        DifferenceOfOptions(double T_, double K_, int m_, PathDepOption* Ptr1_, PathDepOption* Ptr2_) : PathDepOption(T_, K_, m_), Ptr1(Ptr1_), Ptr2(Ptr2_) {}
        double Payoff(const SamplePath& S) const { return Ptr1->Payoff(S) - Ptr2->Payoff(S); }
    };

    class ArthmAsianCall : public PathDepOption
    {
    public:
        ArthmAsianCall(double T_, double K_, int m_) :PathDepOption(T_, K_, m_) {}
        double Payoff(const SamplePath& S) const;
    };

    class GmtrAsianCall : public PathDepOption
    {
    public:
        GmtrAsianCall(double T_, double K_, int m_) : PathDepOption(T_, K_, m_) {}
        double Payoff(const SamplePath& S) const;
        double PriceByBSFormula(const MCModel& Model);
    };
}
