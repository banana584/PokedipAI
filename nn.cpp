#include "nn.hpp"

Matrix::Matrix(size_t width, size_t height) {
    // Assign sizes
    this->width = width;
    this->height = height;

    // Allocate data and set to all 0
    this->data = new double[width * height];
    for (size_t i = 0; i < width * height; i++) {
        this->data[i] = 0.0;
    }
}

Matrix::~Matrix() {
    // Deallocate data to prevent memory leaks
    delete this->data;
}

size_t Matrix::GetIndex(size_t x, size_t y) const {
    // Row * cols + col converts 2D indicies to 1D index
    return x * height + y;
}

bool Matrix::CheckSize(const Matrix& other) const {
    // Check if each size is equal
    return (this->width == other.width) && (this->height == other.height);
}

double Matrix::Get(size_t x, size_t y) const {
    size_t index = GetIndex(x, y);

    // Check if index is in range
    if (index > (width * height)) {
        throw std::out_of_range("Index is too large for matrix.");
    }

    return data[index];
}

void Matrix::Set(size_t x, size_t y, double data) {
    size_t index = GetIndex(x, y);

    // Check if index is in range
    if (index > (width * height)) {
        throw std::out_of_range("Index is too large for matrix.");
    }

    this->data[index] = data;
}

Matrix& Matrix::operator+(const Matrix& other) const {
    // Check if sizes are equal
    if (!this->CheckSize(other)) {
        throw std::out_of_range("Sizes of matrices are different.");
    }

    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t y = 0; y < this->height; y++) {
        for (size_t x = 0; x < this->width; x++) {
            // Add elements and store in new matrix
            double i = this->Get(x, y);
            double j = other.Get(x, y);

            double k = i + j;
            new_matrix.Set(x, y, k);
        }
    }
}

Matrix& Matrix::operator-(const Matrix& other) const {
    // Check if sizes are equal
    if (!this->CheckSize(other)) {
        throw std::out_of_range("Sizes of matrices are different.");
    }

    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t y = 0; y < this->height; y++) {
        for (size_t x = 0; x < this->width; x++) {
            // Subtract elements and store in new matrix
            double i = this->Get(x, y);
            double j = other.Get(x, y);

            double k = i - j;
            new_matrix.Set(x, y, k);
        }
    }
}

Matrix& Matrix::operator*(const Matrix& other) const {
    // Check if sizes can be used for new matrix
    if (this->height != other.width) {
        throw std::out_of_range("Sizes of matrices are incompatible.");
    }

    Matrix new_matrix(this->height, other.width);

    // Loop over each element
    for (size_t y = 0; y < other.width; y++) {
        for (size_t x = 0; x < this->height; x++) {
            // Perform multiplication and add to current element
            for (size_t z = 0; z < this->width; z++) {
                double i = this->Get(x, z);
                double j = this->Get(z, y);

                double k = i * j;
                new_matrix.Set(x, y, new_matrix.Get(x, y) + k);
            }
        }
    }

    return new_matrix;
}

Matrix& Matrix::operator*(const double& other) const {
    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t y = 0; y < this->height; y++) {
        for (size_t x = 0; x < this->width; x++) {
            // Multiply by scalar and store in new matrix
            double i = this->Get(x, y);

            double k = i * other;
            new_matrix.Set(x, y, k);
        }
    }

    return new_matrix;
}