#pragma once 
#include <vector>
using namespace std;
namespace fre 
{

	class BinomialTreeModel 
	{
	private:
		double S0; 
		double U; 
		double D; 
		double R;
	public:
		BinomialTreeModel() :S0(0), U(0), D(0), R(0) {} 
		BinomialTreeModel(double S0_, double U_, double D_, double R_) :S0(S0_), U(U_), D(D_), R(R_) {} 
		BinomialTreeModel(const BinomialTreeModel& B):S0(B.S0), U(B.U), D(B.D), R(B.R) {} 
		~BinomialTreeModel() {}
		double RiskNeutProb() const;
		double CalculateAssetPrice(int n, int i) const;
		void UpdateBinomialTreeModel(double S0_, double U_, double D_, double R_);
		int ValidateInputData() const;
		int GetInputData();
		double GetS0() const { return S0; } 
		double GetU() const { return U; } 
		double GetD() const { return D; } 
		double GetR() const { return R; }
	};

    typedef vector<double> SamplePath;
    class MCModel
    {
    private:
        double S0, r, sigma;
    public:
        MCModel() :S0(0.0), r(0.0), sigma(0.0) {}
        MCModel(double S0_, double r_, double sigma_) :S0(S0_), r(r_), sigma(sigma_) {srand((unsigned)time(NULL));}
        void GenerateSamplePath(double T, int m, SamplePath& S) const;
        double GetS0() const { return S0; } double GetR() const { return r; }
        double GetSigma() const { return sigma; } void SetS0(double S0_) { S0 = S0_; }
        void SetR(double r_) { r = r_; }
        void SetSigma(double sigma_) { sigma = sigma_; }
    };
}
