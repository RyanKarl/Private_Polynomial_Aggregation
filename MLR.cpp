#include <iostream>
#include <vector>
#include <math.h>
#include <iomanip>
#include <stdexcept>

using namespace std;
 
class MultipleLinearRegression{
   public:
      MultipleLinearRegression();  // This is the constructor
      std::vector<double> coefficients_c;
      double intercept;
      void transform_x(vector<vector<double> > &x);

};
 
// Member functions definitions including constructor
MultipleLinearRegression::MultipleLinearRegression(void) {
}


void print_matrix(std::vector<std::vector<double> > mat){
    for(std::vector<double> &v : mat ){
        for(double x : v) cout << x << ' ';
            cout << endl;
    }
}

void MultipleLinearRegression::transform_x(vector<vector<double> > &x) {

    for(int i = 0; i < x.size(); i++){
       x[i].insert(x[i].begin(), 1.0);
    }

}

std::vector<std::vector<double> > transpose(std::vector<std::vector<double> > data) {
    
    std::vector<std::vector<double> > result(data[0].size(), std::vector<double>(data.size()));
    
    for(int i = 0; i < data[0].size(); i++) 
        for (int j = 0; j < data.size(); j++) {
            result[i][j] = data[j][i];
        }

    return result;
}

std::vector<std::vector<double> > dot(std::vector<std::vector<double> > data1, std::vector<std::vector<double> > data2){
    
    std::vector<std::vector<double> > result(data1.size(), std::vector<double>(data2[0].size()));

    for(int i = 0; i < data1.size(); i++){ 
        for(int j = 0; j < data2[0].size(); j++) {
            for(int k = 0; k < data1[0].size(); k++){
                result[i][j] += data1[i][k] * data2[k][j];
            }
        }
    }

    return result;
}

double getDeterminant(const std::vector<std::vector<double>> vect) {
    if(vect.size() != vect[0].size()) {
        throw std::runtime_error("Matrix is not quadratic");
    } 
    int dimension = vect.size();

    if(dimension == 0) {
        return 1;
    }

    if(dimension == 1) {
        return vect[0][0];
    }

    //Formula for 2x2-matrix
    if(dimension == 2) {
        return vect[0][0] * vect[1][1] - vect[0][1] * vect[1][0];
    }

    double result = 0;
    int sign = 1;
    for(int i = 0; i < dimension; i++) {

        //Submatrix
        std::vector<std::vector<double>> subVect(dimension - 1, std::vector<double> (dimension - 1));
        for(int m = 1; m < dimension; m++) {
            int z = 0;
            for(int n = 0; n < dimension; n++) {
                if(n != i) {
                    subVect[m-1][z] = vect[m][n];
                    z++;
                }
            }
        }

        //recursive call
        result = result + sign * vect[0][i] * getDeterminant(subVect);
        sign = -sign;
    }

    return result;
}

std::vector<std::vector<double>> getCofactor(const std::vector<std::vector<double>> vect) {
    if(vect.size() != vect[0].size()) {
        throw std::runtime_error("Matrix is not quadratic");
    }

    std::vector<std::vector<double>> solution(vect.size(), std::vector<double> (vect.size()));
    std::vector<std::vector<double>> subVect(vect.size() - 1, std::vector<double> (vect.size() - 1));

    for(std::size_t i = 0; i < vect.size(); i++) {
        for(std::size_t j = 0; j < vect[0].size(); j++) {

            int p = 0;
            for(size_t x = 0; x < vect.size(); x++) {
                if(x == i) {
                    continue;
                }
                int q = 0;

                for(size_t y = 0; y < vect.size(); y++) {
                    if(y == j) {
                        continue;
                    }

                    subVect[p][q] = vect[x][y];
                    q++;
                }
                p++;
            }
            solution[i][j] = (double) pow(-1, i + j) * getDeterminant(subVect);
        }
    }
    return solution;
}

std::vector<std::vector<double>> getInverse(const std::vector<std::vector<double>> vect) {
    if(getDeterminant(vect) == 0) {
        throw std::runtime_error("Determinant is 0");
    }

    double d = 1.0/getDeterminant(vect);
    std::vector<std::vector<double>> solution(vect.size(), std::vector<double> (vect.size()));

    for(size_t i = 0; i < vect.size(); i++) {
        for(size_t j = 0; j < vect.size(); j++) {
            solution[i][j] = vect[i][j];
        }
    }

    solution = transpose(getCofactor(solution));

    for(size_t i = 0; i < vect.size(); i++) {
        for(size_t j = 0; j < vect.size(); j++) {
            solution[i][j] *= d;
        }
    }

    return solution;
}

void calculateInverse(vector< vector<double> >& A) {
    int n = A.size();

    for (int i=0; i<n; i++) {
        // Search for maximum in this column
        double maxEl = abs(A[i][i]);
        int maxRow = i;
        for (int k=i+1; k<n; k++) {
            if (abs(A[k][i]) > maxEl) {
                maxEl = A[k][i];
                maxRow = k;
            }
        }

        // Swap maximum row with current row (column by column)
        for (int k=i; k<2*n;k++) {
            double tmp = A[maxRow][k];
            A[maxRow][k] = A[i][k];
            A[i][k] = tmp;
        }

        // Make all rows below this one 0 in current column
        for (int k=i+1; k<n; k++) {
            double c = -A[k][i]/A[i][i];
            for (int j=i; j<2*n; j++) {
                if (i==j) {
                    A[k][j] = 0;
                } else {
                    A[k][j] += c * A[i][j];
                }
            }
        }
    }

    // Solve equation Ax=b for an upper triangular matrix A
    for (int i=n-1; i>=0; i--) {
        for (int k=n; k<2*n;k++) {
            A[i][k] /= A[i][i];
        }
        // this is not necessary, but the output looks nicer:
        A[i][i] = 1;

        for (int rowModify=i-1;rowModify>=0; rowModify--) {
            for (int columModify=n;columModify<2*n;columModify++) {
                A[rowModify][columModify] -= A[i][columModify]
                                             * A[rowModify][i];
            }
            // this is not necessary, but the output looks nicer:
            A[rowModify][i] = 0;
        }
    }
}

/*
def _estimate_coefficients(self, x, y):
        xT = x.transpose()
        inversed = np.linalg.inv( xT.dot(x) )
        coefficients = inversed.dot( xT ).dot(y)
        return coefficients
*/

int main() {
   
   srand (time(NULL));
   MultipleLinearRegression mlr;
 
   std::vector<std::vector<double> > x {
                { 0.09823475, 0.590723},
                { 0.486334, 0.8762134}
            };

   std::vector<std::vector<double> > y {
                { 0.3475},
                { 0.1244}
            };

   /*for(int i = 0; i < 3; i++){
       vector<double> v;
       for(int j = 0; j < 3; j++){    
          v.push_back((double)(j + 3*i));
       }
       x.push_back(v);
    }
    */

    std::vector<std::vector<double> > xT(x[0].size(), std::vector<double>(x.size()));
    std::vector<std::vector<double> > xT_dot_x(x.size(), std::vector<double>(x[0].size()));
    std::vector<std::vector<double> > inversed(x.size(), std::vector<double>(x[0].size()));
    std::vector<std::vector<double> > temp(x[0].size(), std::vector<double>(x.size()));
    std::vector<std::vector<double> > coefficients(x.size(), std::vector<double>(x[0].size()));

    mlr.transform_x(x);
    xT = transpose(x);
    xT_dot_x = dot(xT, x);
    inversed = getInverse(xT_dot_x); 
    temp = dot(inversed, xT);
    coefficients = dot(temp, y);
    

    return 0;
}
