#include "nn.hpp"
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>

class MyDataSet : public DataSet {
    public:
        MyDataSet() : DataSet() {
            return;
        }

        void Copy(Matrix* X, Matrix* Y) override {
            this->X = std::vector<Matrix>{*X};
            this->Y = std::vector<Matrix>{*Y};
        }

        void Copy(std::vector<Matrix>& X, std::vector<Matrix>& Y) override {
            this->X = X;
            this->Y = Y;
        }

        void Copy(DataSet& other) override {
            this->X = other.GetX();
            this->Y = other.GetY();
        }

        std::unique_ptr<DataSet> CloneEmpty() const override {
            return std::make_unique<MyDataSet>();
        }

        std::unique_ptr<DataSet> Clone() const override {
            return std::make_unique<MyDataSet>(*this);
        }

        ~MyDataSet() override {
            return;
        }

        void AddSample(Matrix& X, Matrix& Y) override {
            this->X.push_back(X);
            this->Y.push_back(Y);
        }

        size_t Size() const override {
            return X[0].Rows();
        }

        std::tuple<Matrix, Matrix> Get(size_t index) const override {
            return std::make_tuple(X.at(index), Y.at(index));
        }

        std::vector<Matrix> GetX() const override {
            return X;
        }

        std::vector<Matrix> GetY() const override {
            return Y;
        }

        std::tuple<std::vector<Matrix>, std::vector<Matrix>> ToVector() const override {
            std::vector<Matrix> X_vec;
            std::vector<Matrix> Y_vec;

            // Loop over all matrices in X and Y
            for (size_t i = 0; i < X.size(); i++) {
                const Matrix& x_mat = X[i];
                const Matrix& y_mat = Y[i];

                // Split x_mat into individual rows
                for (size_t row = 0; row < x_mat.Rows(); row++) {
                    // Each row becomes a 1×cols matrix
                    Matrix x_row(1, x_mat.Cols());
                    Matrix y_row(1, y_mat.Cols());

                    for (size_t col = 0; col < x_mat.Cols(); col++) {
                        x_row.Set(0, col, x_mat.Get(row, col));
                    }

                    for (size_t col = 0; col < y_mat.Cols(); col++) {
                        y_row.Set(0, col, y_mat.Get(row, col));
                    }

                    X_vec.push_back(x_row);
                    Y_vec.push_back(y_row);
                }
            }

            return std::make_tuple(X_vec, Y_vec);
        }
};


int main() {
    // XOR dataset: 4 examples, 2 inputs each
    Matrix data(4, 2);
    Matrix targets(4, 2);

    // Inputs
    data.Set(0, 0, 0); data.Set(0, 1, 0); targets.Set(0, 0, 0); targets.Set(0, 1, 1);
    data.Set(1, 0, 0); data.Set(1, 1, 1); targets.Set(1, 0, 1); targets.Set(1, 1, 0);
    data.Set(2, 0, 1); data.Set(2, 1, 0); targets.Set(2, 0, 1); targets.Set(2, 1, 0);
    data.Set(3, 0, 1); data.Set(3, 1, 1); targets.Set(3, 0, 0); targets.Set(3, 1, 1);

    // Copy into dataset
    MyDataSet dataset;
    for (size_t i = 0; i < 1; i++) {
        dataset.Copy(&data, &targets);
    }

    // DataLoader: batch size = 4
    DataLoader loader(&dataset, 4, false);

    // Neural network: 2 inputs, 2 outputs, 1 hidden layer with 5 neurons
    std::vector<size_t> neurons = {5, 2};
    NN nn(2, 2, 2, neurons);  // inputs=2, outputs=1, num_layers=2 (hidden+output)
    // Train
    nn.Train(100000, 0.1, loader); // 5000 epochs, lr=0.01

    // Test all 4 XOR inputs
    std::vector<std::vector<double>> test_inputs = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    };

    for (auto &input : test_inputs) {
        std::vector<double> output = nn.Forward(input);
        auto max = std::max_element(output.begin(), output.end());
        size_t idx = static_cast<size_t>(std::distance(output.begin(), max));
        std::cout << input[0] << " XOR " << input[1] << " = " << 1 - idx << "\n";
    }

    return 0;
}