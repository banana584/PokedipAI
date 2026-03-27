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
        // 1D matrix allocated using new/delete.
        double* data;
        // Size of 2D matrix.
        size_t width;
        size_t height;

    protected:
        /**
         * @brief Converts 2D indices into 1D index.
         * 
         * This uses the formula row * cols + col to convert 2D indices into a 1D index that can be used in the data array.
         * 
         * @param x The row index.
         * @param y The col index.
         * @return The 1D index - from the 2D indices - to be used in the data array.
         */
        size_t GetIndex(size_t x, size_t y) const;

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
         * @param x The row to get data from.
         * @param y The col to get data from.
         * 
         * @return That data at the requested index.
         */
        double Get(size_t x, size_t y) const;

        /**
         * @brief Sets data in array at an index.
         * 
         * This maps the 2D indices to a 1D index and sets the data at the requested index.
         * 
         * @param x The row to set the data at.
         * @param y The col to set the data at.
         * @param data The data to set at the requested index.
         */
        void Set(size_t x, size_t y, double data);

        /**
         * @brief Adds 2 matrices.
         *
         * This adds each number in a matrix and returns the result (Aₓᵧ + Bₓᵧ where x and y are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this add other.
         */
        Matrix& operator+(const Matrix& other) const;

        /**
         * @brief Subtracts 2 matricies.
         * 
         * This subtracts each number in a matrix and returns the result (Aₓᵧ - Bₓᵧ where x and y are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this subtract other.
         */
        Matrix& operator-(const Matrix& other) const;

        /**
         * @brief Multiplies 2 matricies.
         * 
         * This multiplies each number in a matrix and returns the result (Σₖ₌₁b ​Aₓₖ​Bₖᵧ where k is the current iteration, b is the last and x and y are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this multiplied other.
         */
        Matrix& operator*(const Matrix& other) const;

        /**
         * @brief Multiplies a matrix and a scalar.
         * 
         * This multiplies each number in a matrix by a scalar and returns the result (Aₓᵧ * n where n is a constant scalar, x and y are indicies).
         * 
         * @param other A const reference to a scalar.
         * @return A new matrix which is the result of this multiplied by a scalar - other.
         */
        Matrix& operator*(const double& other) const;
};