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
         * This copies data - deep copy - from another matrix and creates a new matrix with the same data.
         * 
         * @param other A reference to a different matrix to copy into this.
         */
        Matrix(Matrix& other);

        /**
         * @brief Copy constructor
         * 
         * This copies data - deep copy - from another matrix and creates a new matrix with the same data.
         * 
         * @param other A const reference to a different matrix to copy into this.
         */
        Matrix(const Matrix& other);

        /**
         * @brief Default constructor
         * 
         * This sets rows and cols to 0, and data to nullptr. Default values for all members.
         */
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

        /**
         * @brief Performs the transposition of the current matrix. Does not modify data.
         * 
         * This swaps the rows and cols of the matrix and returns the result. Data is not modified as a new matrix is returned.
         * 
         * @return The newly created matrix with the transposed data.
         */
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

        /**
         * @brief Move constructor for matrices.
         * 
         * Sets data in new matrix to data in other matrix with a deep copy, and destroys the old matrix.
         * 
         * @param other A different matrix to be destroyed and have its data moved into this.
         */
        Matrix(Matrix&& other) noexcept;

        /**
         * @brief Move operator for matrices.
         * 
         * Sets data in this to data in other matrix with a deep copy, and destroys the old matrix.
         * 
         * @param other A different matrix to be destroyed and have its data moved into this.
         * @return The matrix with the moved data stored.
         */
        Matrix& operator=(Matrix&& other) noexcept;

        /**
         * @brief Equality operator for matrices.
         * 
         * Checks if sizes and data are equal. Returns true if they are and false otherwise.
         * 
         * @param other A matrix to compare against this.
         * @return True if they are equal, false otherwise.
         */
        bool operator==(const Matrix& other) const;
};

/**
 * @class DataSet
 * @brief An abstract class representing data
 * 
 * An abstract class used in the DataLoader class for batching and data storage. Inherited classes must implement all methods.
 * 
 * @see DataLoader
 */
class DataSet {
    protected:
        // Data and target matrices, stored in vectors for if there are multiple.
        std::vector<Matrix> X;
        std::vector<Matrix> Y;

    public:
        /**
         * @brief Default constructor
         * 
         * Sets X and Y to empty vectors. Data should be put in with AddSample or Copy after.
         */
        DataSet() {X = std::vector<Matrix>(); Y = std::vector<Matrix>();};

        /**
         * @brief Constructor
         * 
         * Sets X and Y to vectors with 1 element each, which is the data passed in.
         * 
         * @param X A const reference to a matrix of the train/test data.
         * @param Y A const reference to a matrix of the target data.
         */
        DataSet(const Matrix& X, const Matrix& Y) : X({X}), Y({Y}) {};

        /**
         * @brief Copies data into vectors.
         * 
         * Pure virtual function whose implementations should copy data into vectors by overwriting old data.
         * 
         * @param X A pointer to a matrix that will overwrite X data in the class.
         * @param Y A pointer to a matrix that will overwrite Y data in the class.
         */
        virtual void Copy(Matrix* X, Matrix* Y) = 0;

        /**
         * @brief Copies vectors into class.
         * 
         * Pure virtual function whose implementation should copy data into vectors by overwriting old data with full new data.
         * 
         * @param X A reference to a vector of new X data that will overwrite old X data.
         * @param Y A reference to a vector of new Y data that will overwrite old Y data.
         */
        virtual void Copy(std::vector<Matrix>& X, std::vector<Matrix>& Y) = 0;

        /**
         * @brief Copies a different dataset into this.
         * 
         * Pure virtual function whose implementation should copy data into this by overwriting with the data of other.
         * 
         * @param other A reference to another dataset that will overwrite data in this.
         */
        virtual void Copy(DataSet& other) = 0;

        /**
         * @brief Creates a unique pointer to an empty instance of itself.
         * 
         * Pure virtual function whose implementation should create a unique pointer of an empty version of itself with a covariant return type.
         * 
         * @return A unique pointer to an empty instance of the inherited class.
         */
        virtual std::unique_ptr<DataSet> CloneEmpty() const = 0;

        virtual std::unique_ptr<DataSet> Clone() const = 0;

        /**
         * @brief Destructor
         * 
         * Pure virtual function whose implementation should clean up data loaded/created.
         */
        virtual ~DataSet() = 0;

        /**
         * @brief Adds data into existing vectors.
         * 
         * A pure virtual function whose implementation should append new data onto old data vectors.
         * 
         * @param X A reference to a matrix that will be appended onto the X vector.
         * @param Y A reference to a matrix that will be appended onto the Y vector.
         */
        virtual void AddSample(Matrix& X, Matrix& Y) = 0;

        /**
         * @brief Gets the size of the data.
         * 
         * A pure virtual function whose implementation should return the amount of rows in data.
         * 
         * @return The amount of rows in any one of the data or target matrices.
         */
        virtual size_t Size() const = 0;

        /**
         * @brief Gets data and target matrices at any index.
         * 
         * A pure virtual function whose implementation should get the data and target matrices at any index and return them as a tuple.
         * 
         * @param index The index in the vectors of which data to retrieve.
         * @return A tuple of the order X, Y of the data found.
         */
        virtual std::tuple<Matrix, Matrix> Get(size_t index) const = 0;

        /**
         * @brief Gets vector of data matrices.
         * 
         * A pure virtual function whose implementation should return the X vector.
         * 
         * @return The vector of X data.
         */
        virtual std::vector<Matrix> GetX() const = 0;

        /**
         * @brief Gets vector of target matrices.
         * 
         * A pure virtual function whose implementation should return the Y vector.
         * 
         * @return The vector of Y data.
         */
        virtual std::vector<Matrix> GetY() const = 0;

        /**
         * @brief Converts all data in vectors to vectors of row matrices.
         * 
         * A pure virtual function whose implementation should convert each matrix into different row matrices and append them into vectors and return them as tuples.
         * 
         * @return A tuple of the vectors of row matrices.
         */
        virtual std::tuple<std::vector<Matrix>, std::vector<Matrix>> ToVector() const = 0;
};

/**
 * @class DataLoader
 * @brief Handles batching and shuffling of DataSets.
 * 
 * Contains a vector of unique pointers to DataSets that can be shuffled and is batched. Can give an iterator to said batches.
 */
class DataLoader {
    protected:
        // Batch data
        std::vector<std::unique_ptr<DataSet>> batches;
        // Amount of batches and expected size of each batch
        size_t num_batches;
        size_t batch_size;
        // If shuffled or not
        // TODO: Make shuffle do something in constructor
        bool shuffle;
    
    public:
        /**
         * @brief Constructor
         * 
         * Takes in 1 large DataSet, splits into an amount of batches and can shuffle.
         * 
         * @param data A pointer to a DataSet that will be batched and potentiall shuffled.
         * @param num_batches The amount of batches to split the data into.
         * @param shuffle A bool on wether data should be shuffled around.
         */
        DataLoader(DataSet* data, size_t num_batches, bool shuffle);

        /**
         * @class Iterator
         * @brief An iterator that goes over each batch in the loader.
         * 
         * Contains a pointer to the DataLoader and an index of which batch is on. Supports dereferencing, increment, post-increment and equality checks.
         */
        class Iterator {
            protected:
                // A pointer to the DataLoader which this was created from
                DataLoader* loader;
                // The index of the batch we are currently on - does wrap around
                size_t idx;

            public:
                /**
                 * @brief Constructor.
                 * 
                 * Takes in a pointer to the original loader and starting index to store.
                 * 
                 * @param loader The original loader to get data from.
                 * @param index The starting index the iterator is at.
                 */
                Iterator(DataLoader* loader, size_t index);

                /**
                 * @brief Dereferences to get current batch
                 * 
                 * Gets batch from loader at current index.
                 * 
                 * @return A const reference to the current batch of the iterator.
                 */
                const DataSet& operator*() const;

                /**
                 * @brief Gets current pointer.
                 * 
                 * Gets a pointer to the current batch from loader.
                 * 
                 * @return A pointer to the current batch.
                 */
                DataSet* operator->() const;

                /**
                 * @brief Increases index by 1.
                 * 
                 * Increases index by 1 (wraps around if needed) and returns the value of *this after incrementing.
                 * 
                 * @return The state of the iterator after incrementing.
                 */
                Iterator& operator++();

                /**
                 * @brief Increases index by 1.
                 * 
                 * Increases index by 1 (wraps around if needed) and returns the value of *this before incrementing.
                 * 
                 * @return The state of the iterator before incrementing.
                 */
                Iterator operator++(int);

                /**
                 * @brief Checks if iterator is equal to another.
                 * 
                 * Checks if loader pointers and index are equal.
                 * 
                 * @param other A const reference to a different iterator.
                 * @return True if they are equal, false otherwise.
                 */
                bool operator==(const Iterator& other) const;

                /**
                 * @brief Checks if iterator is not equal to another.
                 * 
                 * Checks if they are equal and inverts result.
                 * 
                 * @param other A const reference to a different iterator.
                 * @return True if they are not equal, false otherwise.
                 */
                bool operator!=(const Iterator& other) const;
        };

        /**
         * @brief Gets an iterator to the start of the data.
         * 
         * Creates a new iterator with the index of 0.
         * 
         * @return An iterator pointing to the beginning of batch data.
         */
        Iterator Begin();

        /**
         * @brief Gets an interator the end of the data.
         * 
         * Created a new iterator with the index of size of batches.
         * 
         * @return An interator pointing to the end of batch data.
         */
        Iterator End();
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