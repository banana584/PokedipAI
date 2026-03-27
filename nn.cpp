#include "nn.hpp"

Matrix::Matrix(size_t width, size_t height) : width(width), height(height) {
    // Allocate data and set to all 0
    this->data = new double[width * height];
    for (size_t i = 0; i < width * height; i++) {
        this->data[i] = 0.0;
    }
}

Matrix::Matrix(Matrix& other) : width(other.width), height(other.height) {
    // Copy data over
    this->data = other.data;
}

Matrix::~Matrix() {
    // Deallocate data to prevent memory leaks
    delete[] this->data;
}

size_t Matrix::GetIndex(size_t row, size_t col) const {
    // Row * cols + col converts 2D indicies to 1D index
    return row * width + col;
}

bool Matrix::CheckSize(const Matrix& other) const {
    // Check if each size is equal
    return (this->width == other.width) && (this->height == other.height);
}

double Matrix::Get(size_t row, size_t col) const {
    // Check if indices are in range
    if (row >= height || col >= width) {
        throw std::out_of_range("Index out of range");
    }

    size_t index = GetIndex(row, col);

    return data[index];
}

void Matrix::Set(size_t row, size_t col, double data) {
    // Check if indices are in range
    if (row >= height || col >= width) {
        std::cout << height << " " << width << "\n";
        throw std::out_of_range("Index out of range");
    }

    size_t index = GetIndex(row, col);

    this->data[index] = data;
}

void Matrix::Print() const {
    // Loop over each element
    for (size_t row = 0; row < height; row++) {
        for (size_t col = 0; col < width; col++) {
            std::cout << data[GetIndex(row, col)] << " ";
        }
        std::cout << "\n";
    }
}

size_t Matrix::Width() const noexcept {
    return width;
}

size_t Matrix::Height() const noexcept {
    return height;
}

Matrix Matrix::operator+(const Matrix& other) const {
    // Check if sizes are equal
    if (!this->CheckSize(other)) {
        throw std::out_of_range("Sizes of matrices are different.");
    }

    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t row = 0; row < this->width; row++) {
        for (size_t col = 0; col < this->height; col++) {
            // Add elements and store in new matrix
            double i = this->Get(row, col);
            double j = other.Get(row, col);

            double k = i + j;
            new_matrix.Set(row, col, k);
        }
    }

    return new_matrix;
}

Matrix Matrix::operator-(const Matrix& other) const {
    // Check if sizes are equal
    if (!this->CheckSize(other)) {
        throw std::out_of_range("Sizes of matrices are different.");
    }

    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t row = 0; row < this->width; row++) {
        for (size_t col = 0; col < this->height; col++) {
            // Subtract elements and store in new matrix
            double i = this->Get(row, col);
            double j = other.Get(row, col);

            double k = i - j;
            new_matrix.Set(row, col, k);
        }
    }

    return new_matrix;
}

Matrix Matrix::operator*(const Matrix& other) const {
    // Check if sizes are compatible
    if (width != other.height) {
        throw std::out_of_range("Sizes incompatible.");
    }

    Matrix result(height, other.width);

    // Loop over all elements and perform dot product
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < other.width; j++) {

            double sum = 0.0;

            for (size_t k = 0; k < width; k++)
                sum += Get(i, k) * other.Get(k, j);

            result.Set(i, j, sum);
        }
    }

    return result;
}

Matrix Matrix::operator*(const double& other) const {
    Matrix new_matrix(this->width, this->height);

    // Loop over each element
    for (size_t row = 0; row < this->width; row++) {
        for (size_t col = 0; col < this->height; col++) {
            // Multiply by scalar and store in new matrix
            double i = this->Get(row, col);

            double k = i * other;
            new_matrix.Set(row, col, k);
        }
    }

    return new_matrix;
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) return *this;

    // Copy all data
    this->width = other.width;
    this->height = other.height;

    delete[] this->data;
    this->data = other.data;

    return *this;
}