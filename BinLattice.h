#pragma once
#include <vector>
#include <iomanip>
using namespace std;
namespace fre {
    class BinLattice
    {
    private:
        int N;
        vector<vector<double>> Lattice;
    public:
        BinLattice():N(0) {}
        BinLattice(int N_) :N(N_)
        {
            Lattice.resize(N + 1);
            for (int n = 0; n <= N; n++) Lattice[n].resize(n + 1);
        }
        ~BinLattice() {}
        void SetN(int N_)
        {
            N=N_;
            Lattice.resize(N + 1);
            for (int n = 0; n <= N; n++) Lattice[n].resize(n + 1);
            
        }
        void SetNode(int n, int i, double x) { Lattice[n][i] = x; } double GetNode(int n, int i) { return Lattice[n][i]; }
        void Display()
        {
            cout << setiosflags(ios::fixed) << setprecision(3);
            for (int n = 0; n <= N; n++)
            {
                for (int i = 0; i <= n; i++)
                    cout << setw(15) << GetNode(n, i);
                cout << endl;
            }
        cout << endl;
        }
    };
}
