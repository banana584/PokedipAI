#pragma once

#include <iostream>
#include <cmath>

/**
 * @class Matrix
 * @brief A class to represent 2D matrices.
 * 
 * A class that is a 2D matrix, but a 1D matrix is used and indices are converted. Supports addition, subtraction, multiplication and scalar multiplication currently.
 */
class Matrix {
    protected:
        // Size of 2D array.
        size_t width;
        size_t height;
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
         * This checks if width and height are exactly the same between 2 matrices.
         * 
         * @param other The other matrix to compare sizes with this.
         * @return True if the sizes are equal, False if they are different.
         */
        bool CheckSize(const Matrix& other) const;
    
    public:
        /**
         * @brief Constructor that creates data.
         * 
         * This dynamically allocates the data array, of size width * height, and initialises it to be all 0.
         * 
         * @param width The width of the 2D array.
         * @param height The height of the 2D array.
         */
        Matrix(size_t width, size_t height);

        /**
         * @brief Copy constructor
         * 
         * This copies data from another matrix and creates a new matrix with the same data.
         * 
         * @param other A different matrix to copy into this.
         */
        Matrix(Matrix& other);

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
         * @brief Helper function to print the data in the matrix.
         * 
         * Prints all data in matrix, with 1 space inbetween each item a row and a newline between each row.
         */
        void Print() const;

        /**
         * @brief Helper function to get width.
         * 
         * Returns the value of width.
         * 
         * @return The width of the matrix.
         */
        size_t Width() const noexcept;

        /**
         * @brief Helper function to get height.
         * 
         * Returns the value of height.
         * 
         * @return The height of the matrix.
         */
        size_t Height() const noexcept;

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
         * @param other A const reference to a scalar.
         * @return A new matrix which is the result of this multiplied by a scalar - other.
         */
        Matrix operator*(const double& other) const;

        /**
         * @brief Copy operator for matrices.
         * 
         * Sets this to other, including size and data. Old data is lost and deallocated.
         * 
         * @param other A const reference to another matrix to copy.
         * @return The new data copied, a reference to this.
         */
        Matrix& operator=(const Matrix& other);
};