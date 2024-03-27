// Main file of cache emulator
#include "iostream"
#include "Cache/cache.cpp"
#include<random>

using namespace std;

vector<vector<long>> matrix_multiply_cache(const vector<vector<long>>& matA, const vector<vector<long>>& matB, cache my_cache)
{
    /* Obtains cache caluclation for matrix multiplication*/
    long hit = 0, miss = 0, total_count = 0;
    if (matA[0].size() != matB.size())
    {
        cerr << "Error: Cannot multiply matrices. \n";
        return {};
    }
    vector<vector<long>> result(matA.size(), vector<long>(matB[0].size(), 0));
    for (long i = 0; i < matA.size(); ++i) {
        for (long j = 0; j < matB[0].size(); ++j)
        {
            for (long k = 0; k < matA[0].size(); ++k)
            {
                result[i][j] += matA[i][k] * matB[k][j];
                long result_addr = reinterpret_cast<long>(&result[i][j]);
                if(my_cache.find(result_addr))hit++;
                else miss++;
                long a_addr = reinterpret_cast<long>(&matA[i][k]);
                if(my_cache.find(a_addr))hit++;
                else miss++;
                long b_addr = reinterpret_cast<long>(&matB[k][j]);
                if(my_cache.find(b_addr))hit++;
                else miss++;
                total_count+=3;
            }
        }
    }
    cout<<"HIT "<<hit<<endl;
    cout<<"MISS "<<miss<<endl;
    cout<<"TOTAL "<<total_count<<endl;
    cout<<"Hit Ratio: "<<static_cast<double>(hit)/total_count<<endl;
    cout<<"Miss Ratio: "<<static_cast<double>(miss)/total_count<<endl;
    return result;
}

vector<vector<long>> initalize_random_matrix(int rows, int cols)
{
    /* Initalises a random matrix based on input*/
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long> dis(1, 50); 

    vector<vector<long>> matrix(rows, vector<long>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " rows_A cols_A rows_B cols_B" << endl;
        return 1;
    }

    int rowsA = atoi(argv[1]);
    int colsA = atoi(argv[2]);
    int rowsB = atoi(argv[3]);
    int colsB = atoi(argv[4]);
    cache my_cache(1024,64,4,2);
    vector<vector<long>> matA = initalize_random_matrix(rowsA,colsA);
    vector<vector<long>> matB = initalize_random_matrix(rowsB,colsB);
    vector<vector<long>> result = matrix_multiply_cache(matA,matB,my_cache);

}