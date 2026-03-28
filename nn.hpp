#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <tuple>
#include <memory>
#include <cassert>

/**
 * @class Matrix
 * @brief A class to represent 2D matrices.
 * 
 * A class that is a 2D matrix, but a 1D matrix is used and indices are converted. Supports addition, subtraction, multiplication and scalar multiplication currently.
 */
class Matrix {
    protected:
        // Size of 2D array.
        size_t rows;
        size_t cols;
        // 1D matrix allocated using new/delete.
        double* data;

    protected:
        /**
         * @brief Converts 2D indices into 1D index.
         * 
         * This uses the formula row * cols + col to convert 2D indices into a 1D index that can be used in the data array.
         * 
         * @param row The row index.
         * @param col The col index.
         * @return The 1D index - from the 2D indices - to be used in the data array.
         */
        size_t GetIndex(size_t row, size_t col) const;

        /**
         * @brief Checks if 2 matrices have the same dimensions.
         * 
         * This checks if cols and rows are exactly the same between 2 matrices.
         * 
         * @param other The other matrix to compare sizes with this.
         * @return True if the sizes are equal, False if they are different.
         */
        bool CheckSize(const Matrix& other) const;
    
    public:
        /**
         * @brief Constructor that creates data.
         * 
         * This dynamically allocates the data array, of size cols * rows, and initialises it to be all 0.
         * 
         * @param cols The cols of the 2D array.
         * @param rows The rows of the 2D array.
         */
        Matrix(size_t rows, size_t cols);

        /**
         * @brief Copy constructor
         * 
         * This copies data from another matrix and creates a new matrix with the same data.
         * 
         * @param other A different matrix to copy into this.
         */
        Matrix(Matrix& other);

        Matrix(const Matrix& other);

        Matrix();

        /**
         * @brief Destructor that destroyes class.
         * 
         * This deallocates the data array with delete.
         */
        ~Matrix();

        /**
         * @brief Gets data from an index.
         * 
         * This maps the 2D indices to a 1D index and returns the data at the requested index.
         * 
         * @param row The row to get data from.
         * @param col The col to get data from.
         * 
         * @return That data at the requested index.
         */
        double Get(size_t row, size_t col) const;

        /**
         * @brief Sets data in array at an index.
         * 
         * This maps the 2D indices to a 1D index and sets the data at the requested index.
         * 
         * @param row The row to set the data at.
         * @param col The col to set the data at.
         * @param data The data to set at the requested index.
         */
        void Set(size_t row, size_t col, double data);

        Matrix Transpose() const;

        /**
         * @brief Helper function to print the data in the matrix.
         * 
         * Prints all data in matrix, with 1 space inbetween each item a row and a newline between each row.
         */
        void Print() const;

        /**
         * @brief Helper function to get cols.
         * 
         * Returns the value of cols.
         * 
         * @return The cols of the matrix.
         */
        size_t Cols() const noexcept;

        /**
         * @brief Helper function to get rows.
         * 
         * Returns the value of rows.
         * 
         * @return The rows of the matrix.
         */
        size_t Rows() const noexcept;

        /**
         * @brief Adds 2 matrices.
         *
         * This adds each number in a matrix and returns the result (Aₓᵧ + Bₓᵧ where col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this add other.
         */
        Matrix operator+(const Matrix& other) const;

        /**
         * @brief Subtracts 2 matricies.
         * 
         * This subtracts each number in a matrix and returns the result (Aₓᵧ - Bₓᵧ where col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this subtract other.
         */
        Matrix operator-(const Matrix& other) const;

        /**
         * @brief Multiplies 2 matricies.
         * 
         * This multiplies each number in a matrix and returns the result (Σₖ₌₁b ​Aₓₖ​Bₖᵧ where k is the current iteration, b is the last and col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this multiplied other.
         */
        Matrix operator*(const Matrix& other) const;

        /**
         * @brief Multiplies a matrix and a scalar.
         * 
         * This multiplies each number in a matrix by a scalar and returns the result (Aₓᵧ * n where n is a constant scalar, col and row are indicies).
         * 
         * @param other A const scalar.
         * @return A new matrix which is the result of this multiplied by a scalar - other.
         */
        Matrix operator*(const double other) const;

        /**
         * @brief Copy operator for matrices.
         * 
         * Sets this to other, including size and data. Old data is lost and deallocated.
         * 
         * @param other A const reference to another matrix to copy.
         * @return The new data copied, a reference to this.
         */
        Matrix& operator=(const Matrix& other);

        Matrix(Matrix&& other) noexcept;

        Matrix& operator=(Matrix&& other) noexcept;

        bool operator==(const Matrix& other) const;
};

class DataSet {
    public:
        DataSet() {X = std::vector<Matrix>(); Y = std::vector<Matrix>();};

        DataSet(const Matrix& X, const Matrix& Y) : X({X}), Y({Y}) {};

        virtual void Copy(Matrix* X, Matrix* Y) = 0;

        virtual void Copy(std::vector<Matrix>& X, std::vector<Matrix>& Y) = 0;

        virtual void Copy(DataSet& other) = 0;

        virtual std::unique_ptr<DataSet> CloneEmpty() const = 0;

        virtual ~DataSet() = 0;

        virtual void AddSample(Matrix& X, Matrix& Y) = 0;

        virtual size_t Size() const = 0;

        virtual std::tuple<Matrix, Matrix> Get(size_t index) const = 0;

        virtual std::vector<Matrix> GetX() const = 0;

        virtual std::vector<Matrix> GetY() const = 0;

        virtual std::tuple<std::vector<Matrix>, std::vector<Matrix>> ToVector() const = 0;
    
    protected:
        std::vector<Matrix> X;
        std::vector<Matrix> Y;
};

class DataLoader {
    protected:
        DataSet* data;
        std::vector<std::unique_ptr<DataSet>> batches;
        size_t num_batches;
        size_t batch_size;
        size_t idx;
        bool shuffle;
    
    public:
        DataLoader(DataSet* data, size_t num_batches, bool shuffle);

        std::tuple<Matrix, Matrix> operator*() const;

        DataLoader& operator++();

        DataLoader operator++(int) = delete;

        bool operator==(const DataLoader& other) const;

        bool operator!=(const DataLoader& other) const;

        ////Matrix* operator->() const;
        DataSet* operator->() const;

        DataLoader(const DataLoader&) = delete;
        DataLoader& operator=(const DataLoader&) = delete;

        DataLoader(DataLoader&&) = default;
        DataLoader& operator=(DataLoader&&) = default;
};

struct Layer {
    Matrix weights;
    size_t num_weights;
    Matrix biases;

    Layer* next;
    Layer* prev;

    Layer(Matrix& weights, size_t num_weights, Matrix& biases, Layer* next, Layer* prev) {
        this->weights = weights;
        this->num_weights = num_weights;
        this->biases = biases;
        this->next = next;
        this->prev = prev;
    }
};

class NN {
    protected:
        Layer* head;
        size_t num_layers;
        size_t inputs;
        size_t outputs;
        static std::mt19937 gen;

    protected:
        inline double Sigmoid(double x) const;

        inline double SigmoidDeriv(double x) const;

        inline double Error(Matrix output, Matrix target) const;
    
    public:
        NN(size_t inputs, size_t outputs, size_t num_layers, std::vector<size_t> num_neurons);

        ~NN();

        std::vector<double> Forward(const std::vector<double>& inputs) const;

        double Backward(const std::vector<double>& inputs, const std::vector<double>& targets, double lr);

        void Train(size_t epochs, double lr, DataLoader& loader);
};