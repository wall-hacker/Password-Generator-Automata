#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

int t, n, E, m, k, qx;
char charAdjMat[5001][5001];
int intAdjMat[201][201];
int intAdjMatCopy[201][201];
int resultAdjMat[201][201];
int auxMat[201][201];
vector<vector<vector<int>>> intermediateOddMat = vector<vector<vector<int>>>(0, vector<vector<int>>(201, vector<int>(201, 0)));
vector<vector<vector<int>>> intermediateEvenMat = vector<vector<vector<int>>>(0, vector<vector<int>>(201, vector<int>(201, 0)));
pair<int, char> wordMat[5001][25005];
pair<int, char> adjList[5001][27];
unordered_map<int, bool> q;
string solution = "";

string buildWord(int node, int length) {
    string result = "";
    while (length > 0) {
        result += wordMat[node][length].second;
        node = wordMat[node][length].first;
        length--;
    }
    reverse(result.begin(), result.end()); 
    return result;
}

void generateWordsDP() {
    long maxLength = n + k;

    // Initialize the first column of the matrix
    int previousNode = qx;
    for (int branch = 1; branch <= E; ++branch) {
        pair<int, char> currentNode = adjList[previousNode][branch];
        wordMat[currentNode.first][1] = make_pair(previousNode, currentNode.second);
    }

    for (long length = 2; length <= maxLength; ++length) {
        for (int previousNode = 1; previousNode <= n; ++previousNode) {
            for (int branch = 1; branch <= E; ++branch) {
                pair<int, char> currentNode = adjList[previousNode][branch];
                if (wordMat[previousNode][length - 1].second != '_' && wordMat[currentNode.first][length].second == '_') {
                    // Update the tuple information
                    wordMat[currentNode.first][length] = make_pair(previousNode, currentNode.second);
                    if (q[currentNode.first] && length >= k) {
                        solution = buildWord(currentNode.first, length);
                        return;
                    }
                }
            }
        }
    }
}

void multiplyMatrices(int mat1[][201], int mat2[][201], int result[][201], int odd) {
    vector<vector<int>> aux = vector<vector<int>>(201, vector<int>(201, 0));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            result[i][j] = 0;
            for (int x = 1; x <= n; x++) {
                result[i][j] += mat1[i][x] * mat2[x][j];
                if (result[i][j] > 0) {
                    result[i][j] = 1;
                    aux[i][j] = x;               
                    break;
                }
            }
        }
    }

    if (odd)
        intermediateOddMat.push_back(aux);
    else
        intermediateEvenMat.push_back(aux);
}

void power(int a[][201], int mat[][201], int result[][201], int power) {
    vector<vector<int>> aux = vector<vector<int>>(201, vector<int>(201, 0));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            aux[i][j] = mat[i][j];
        }
    }
    intermediateEvenMat.push_back(aux);

    int first = 1;
    if (power % 2 != 0) {
        first = 2;
        intermediateOddMat.push_back(aux);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                result[i][j] = aux[i][j];
            }
        }
    }
    
    while (power != 0) {
        if (power % 2 != 0) {
            if (first == 0) {
                multiplyMatrices(result, mat, a, 1);
                for (int i = 1; i <= n; i++) {
                    for (int j = 1; j <= n; j++) {
                        result[i][j] = a[i][j];
                    }
                }
            }
            if (first == 2)
                first = 0;
            if (first == 1) {
                for (int i = 1; i <= n; i++) {
                    for (int j = 1; j <= n; j++) {
                        result[i][j] = mat[i][j];
                    }
                }
                first = 0;
                intermediateOddMat.push_back(intermediateEvenMat[intermediateEvenMat.size() - 1]);
            }
        }
        if (power == 1)
            return;
        
        multiplyMatrices(mat, mat, a, 0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                mat[i][j] = a[i][j];
            }
        }
        power /= 2;
    }
}

int closestPowerOfTwo(int number) {
    if (number <= 0 || (number & (number - 1)) == 0) {
        return number;
    }
    // Find the closest smaller power of two
    return pow(2, static_cast<int>(log2(number)));
}

bool isPowerOfTwo(int number) {
    return (number > 0) && ((number & (number - 1)) == 0);
}

string divideTheCharsPowOfTwo(int firstNode, int secondNode, int power) {
    if (power == 1) {
        std::string s(1, charAdjMat[firstNode][secondNode]);
        return s;
    }
    int intNode = intermediateEvenMat[log2(power)][firstNode][secondNode];
    power /= 2;
    return divideTheCharsPowOfTwo(firstNode, intNode, power) + divideTheCharsPowOfTwo(intNode, secondNode, power);
}

string divideTheChars(int firstNode, int secondNode, vector<vector<vector<int>>> mat, int power) {
    if (power == 1) {
        std::string s(1, charAdjMat[firstNode][secondNode]);
        return s;
    }
    int intNode = mat[mat.size() - 1][firstNode][secondNode];
    intermediateOddMat.pop_back();
    if (isPowerOfTwo(power - closestPowerOfTwo(power)))
        return divideTheCharsPowOfTwo(firstNode, intNode, power - closestPowerOfTwo(power)) + divideTheCharsPowOfTwo(intNode, secondNode, closestPowerOfTwo(power));
    else
        return divideTheChars(firstNode, intNode, intermediateOddMat, power - closestPowerOfTwo(power)) + divideTheCharsPowOfTwo(intNode, secondNode, closestPowerOfTwo(power));
}

int main() {
    // Open input file
    ifstream fin("input.txt");
    if (!fin.is_open()) {
        cerr << "Error opening input.txt\n";
        return 1;
    }

    // Open output file
    ofstream fout("output.txt");
    if (!fout.is_open()) {
        cerr << "Error opening output.txt\n";
        return 1;
    }

    // initialize adjacency matrix
    for (int i = 0; i < 5001; i++) {
        for (int j = 0; j < 5001; j++) {
            charAdjMat[i][j] = '_';
        }
    }

    fin >> t;
    if (t == 3) {
        return 0;
    }
    fin >> n >> E >> m >> k;
    fin >> qx;
    for (int i = 0; i < m; i++) {
        int endNode;
        fin >> endNode;
        q[endNode] = true;
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= E; j++) {
            int a;
            fin >> a;
            charAdjMat[i][a] = char(96 + j);
            intAdjMat[i][a] = 1;
            adjList[i][j].first = a;
            adjList[i][j].second = char(96 + j);
        } 
    }

    // initialize word matrix
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n + k; j++) {
            wordMat[i][j].first = 0;
            wordMat[i][j].second = '_';
        }
    }

    // Generate words using dynamic programming
    if (t == 1) {
        generateWordsDP();
        if (solution == "") {
            fout << -1 << "\n";
        } else {
            fout << solution.length() << "\n" << solution << "\n";
        }
        return 0;
    }

    // Generate words using matrix exponentiation
    if (t == 2) {
        int addLength = 0;
        int finalNode = 0;
        while(true) {
            
            // String not found
            if (addLength > n) {
                fout << -1 << "\n";
                return 0;
            }

            // Initialize matrixes
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    if (i == j) {
                        auxMat[i][j] = 1;
                        resultAdjMat[i][j] = 1;
                    } else {
                        auxMat[i][j] = 0;
                        resultAdjMat[i][j] = 0;
                    }
                    intAdjMatCopy[i][j] = intAdjMat[i][j];
                }
            }
            intermediateEvenMat.clear();
            intermediateOddMat.clear();
            power(auxMat, intAdjMatCopy, resultAdjMat, k + addLength);
            
            for (auto& elem : q) {
                if (elem.second == true) {
                    if (resultAdjMat[qx][elem.first] == 1) {

                        fout << k + addLength << "\n";

                        finalNode = elem.first;
                        int intNode = intermediateOddMat[intermediateOddMat.size() - 1][qx][finalNode];

                        if (isPowerOfTwo(k + addLength)) {
                            solution = divideTheCharsPowOfTwo(qx, intNode, closestPowerOfTwo(k + addLength) / 2) + divideTheCharsPowOfTwo(intNode, finalNode, closestPowerOfTwo(k + addLength) / 2);
                            fout << solution << '\n';
                            fin.close();
                            fout.close();
                            return 0;
                        } else {
                            intermediateOddMat.pop_back();
                            if (isPowerOfTwo(k + addLength - closestPowerOfTwo(k + addLength))) {
                                solution = divideTheCharsPowOfTwo(qx, intNode, k + addLength - closestPowerOfTwo(k + addLength)) + divideTheCharsPowOfTwo(intNode, finalNode, closestPowerOfTwo(k + addLength));
                                fout << solution << '\n';
                                fin.close();
                                fout.close();
                                return 0;
                            } else {
                                solution = divideTheChars(qx, intNode, intermediateOddMat, k + addLength - closestPowerOfTwo(k + addLength)) + divideTheCharsPowOfTwo(intNode, finalNode, closestPowerOfTwo(k + addLength));
                                fout << solution << '\n';
                                fin.close();
                                fout.close();   
                                return 0;
                            }
                        }
                    }
                }
            }
            addLength++;
        }
    }
}
