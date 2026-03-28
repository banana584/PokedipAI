#include "nn.hpp"

std::mt19937 NN::gen{std::random_device{}()};

Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    data = new double[rows * cols]{};
}

Matrix::Matrix(Matrix& other) : rows(other.rows), cols(other.cols) {
    data = new double[rows * cols];
    std::copy(other.data, other.data + rows * cols, data);
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    data = new double[rows * cols];
    std::copy(other.data, other.data + rows * cols, data);
}

Matrix::Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) {
    other.data = nullptr;
    other.rows = 0;
    other.cols = 0;
}

Matrix::Matrix() : rows(0), cols(0), data(nullptr) {}

Matrix::~Matrix() {
    delete[] data;
}

size_t Matrix::GetIndex(size_t row, size_t col) const {
    return row * cols + col;
}

double Matrix::Get(size_t row, size_t col) const {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Index out of range");
    }

    return data[GetIndex(row, col)];
}

void Matrix::Set(size_t row, size_t col, double value) {
    if (row >= rows || col >= cols)
        throw std::out_of_range("Index out of range");

    data[GetIndex(row, col)] = value;
}

size_t Matrix::Rows() const noexcept { return rows; }
size_t Matrix::Cols() const noexcept { return cols; }

bool Matrix::CheckSize(const Matrix& other) const {
    return rows == other.rows && cols == other.cols;
}

void Matrix::Print() const {
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++)
            std::cout << Get(r,c) << " ";
        std::cout << "\n";
    }
}

Matrix Matrix::Transpose() const {
    Matrix result(cols, rows);

    for (size_t r = 0; r < rows; r++)
        for (size_t c = 0; c < cols; c++)
            result.Set(c, r, Get(r,c));

    return result;
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (!CheckSize(other)) {
        throw std::runtime_error("Matrix size mismatch");
    }

    Matrix result(rows, cols);

    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            result.Set(r,c, Get(r,c) + other.Get(r,c));
        }
    }

    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (!CheckSize(other)) {
        throw std::runtime_error("Matrix size mismatch");
    }

    Matrix result(rows, cols);

    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            result.Set(r,c, Get(r,c) - other.Get(r,c));
        }
    }

    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw std::runtime_error("Matrix multiply dimension mismatch");
    }

    Matrix result(rows, other.cols);

    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < other.cols; c++) {

            double sum = 0.0;

            for (size_t k = 0; k < cols; k++) {
                sum += Get(r,k) * other.Get(k,c);
            }

            result.Set(r,c,sum);
        }
    }

    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols);

    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            result.Set(r,c, Get(r,c) * scalar);
        }
    }

    return result;
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) return *this;

    delete[] data;

    rows = other.rows;
    cols = other.cols;

    data = new double[rows * cols];
    std::copy(other.data, other.data + rows * cols, data);

    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this == &other) return *this;

    delete[] data;

    data = other.data;
    rows = other.rows;
    cols = other.cols;

    other.data = nullptr;
    other.rows = 0;
    other.cols = 0;

    return *this;
}

bool Matrix::operator==(const Matrix& other) const {
    if (!CheckSize(other)) return false;

    for (size_t i = 0; i < rows*cols; i++) {
        if (data[i] != other.data[i]) {
            return false;
        }
    }

    return true;
}

DataSet::~DataSet() {}

DataLoader::DataLoader(DataSet* data, size_t num_batches, bool shuffle) : num_batches(num_batches), shuffle(shuffle) {
    batch_size = data->Size() / num_batches;

    auto vec = data->ToVector();

    for (size_t i = 0; i < num_batches; ++i) {
        size_t start = i * batch_size;

        if (start >= data->Size()) {
            break;
        }

        auto& Xvec = std::get<0>(vec);
        auto& Yvec = std::get<1>(vec);

        size_t total = Xvec.size();

        size_t end = std::min(start + batch_size, total);

        assert(start <= end);
        assert(end <= total);

        std::vector<Matrix> subX(Xvec.begin() + start, Xvec.begin() + end);

        std::vector<Matrix> subY(Yvec.begin() + start, Yvec.begin() + end);

        auto batchData = data->CloneEmpty();
        batchData->Copy(subX, subY);

        // Add the batch to the vector
        batches.push_back(std::move(batchData));
    }
}

DataLoader::Iterator DataLoader::Begin() {
    return Iterator(this, 0);
}

DataLoader::Iterator DataLoader::End() {
    return Iterator(this, batches.size());
}

DataLoader::Iterator::Iterator(DataLoader* loader, size_t index) {
    this->loader = loader;
    this->idx = index;
}

const DataSet& DataLoader::Iterator::operator*() const {
    return *(loader->batches[idx]);
}

DataSet* DataLoader::Iterator::operator->() const {
    return loader->batches[idx].get();
}

DataLoader::Iterator& DataLoader::Iterator::operator++() {
    idx = (idx + 1) % loader->batches.size();
    return *this;
}

DataLoader::Iterator DataLoader::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool DataLoader::Iterator::operator==(const Iterator& other) const {
    return loader == other.loader && idx == other.idx;
}

bool DataLoader::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

/*
std::tuple<Matrix, Matrix> DataLoader::operator*() const {
    std::tuple<std::vector<Matrix>, std::vector<Matrix>> vec = batches.at(idx)->ToVector();

    Matrix data_X = std::get<0>(vec).at(0);
    Matrix data_Y = std::get<1>(vec).at(0);

    return std::make_tuple(data_X, data_Y);
}

DataLoader& DataLoader::operator++() {
    idx = (idx + 1) % num_batches;
    return *this;
}

bool DataLoader::operator==(const DataLoader& other) const {
    return idx == other.idx;
}

bool DataLoader::operator!=(const DataLoader& other) const {
    return !(*this == other);
}

DataSet* DataLoader::operator->() const {
    return batches[idx].get();
}
*/

NN::NN(size_t inputs, size_t outputs, size_t num_layers, std::vector<size_t> num_neurons) {
    this->inputs = inputs;
    this->outputs = outputs;
    this->num_layers = num_layers;

    std::uniform_real_distribution<double> dist(-1,1);

    size_t prev_size = inputs;

    head = nullptr;
    Layer* prev = nullptr;

    for (size_t i = 0; i < num_neurons.size(); i++) {
        size_t curr_size = num_neurons[i];

        // Initialize weights to random values
        Matrix weights(prev_size, curr_size);

        for (size_t r = 0; r < weights.Rows(); r++) {
            for (size_t c = 0; c < weights.Cols(); c++) {
                weights.Set(r, c, dist(gen));
            }
        }

        // Initialize biases to random values
        Matrix biases(1, curr_size);
        for (size_t c = 0; c < curr_size; c++) {
            biases.Set(0, c, dist(gen));
        }

        // Store new information in linked list
        Layer* layer = new Layer(weights, curr_size, biases, nullptr, prev);

        if (!head) {
            head = layer;
        } else {
            prev->next = layer;
        }

        // Update pointers
        prev = layer;
        prev_size = curr_size;
    }
}

NN::~NN() {
    // Destroy linked list
    Layer* current = this->head;

    while (current) {
        Layer* tmp = current->next;

        delete current;

        current = tmp;
    }
}

inline double NN::Sigmoid(double x) const {
    return 1 / (1 + exp(-x));
}

inline double NN::SigmoidDeriv(double x) const {
    return Sigmoid(x) * (1 - Sigmoid(x));
}

inline double NN::Error(Matrix output, Matrix target) const {
    Matrix diff = output - target;

    double sum = 0.0;
    for (size_t i = 0; i < diff.Cols(); i++) {
        for (size_t j = 0; j < diff.Rows(); j++) {
            sum += diff.Get(i, j);
        }
    }

    return pow(sum, 2);
}

std::vector<double> NN::Forward(const std::vector<double>& inputs) const {
    // Convert inputs to 1×n matrix
    Matrix current_input(1, inputs.size());
    for (size_t col = 0; col < inputs.size(); col++)
        current_input.Set(0, col, inputs[col]);

    // Pass through each layer
    Layer* current = head;
    while (current) {
        // z = xW + b
        Matrix z = current_input * current->weights;
        z = z + current->biases;

        // Apply sigmoid activation
        for (size_t row = 0; row < z.Rows(); row++) {
            for (size_t col = 0; col < z.Cols(); col++) {
                z.Set(row, col, Sigmoid(z.Get(row, col)));
            }
        }

        current_input = z;
        current = current->next;
    }

    // Convert final matrix output to vector
    std::vector<double> res(current_input.Cols());
    for (size_t col = 0; col < current_input.Cols(); col++) {
        res[col] = current_input.Get(0, col);
    }

    return res;
}

double NN::Backward(const std::vector<double>& inputs, const std::vector<double>& target, double lr) {
    // Perform forward pass to get each layer value
    Matrix input_matrix(1, inputs.size());
    for (size_t col = 0; col < inputs.size(); col++) {
        input_matrix.Set(0, col, inputs[col]);
    }

    std::vector<Matrix> layer_outputs;
    Matrix current_input = input_matrix;

    Layer* current = head;
    while (current) {
        // z = xW + b
        Matrix z = current_input * current->weights;
        z = z + current->biases;

        // Apply sigmoid activation
        for (size_t row = 0; row < z.Rows(); row++) {
            for (size_t col = 0; col < z.Cols(); col++) {
                z.Set(row, col, Sigmoid(z.Get(row, col)));
            }
        }

        layer_outputs.push_back(z);
        current_input = z;
        current = current->next;
    }

    Matrix output = current_input; // final layer output

    // Get loss
    Matrix target_matrix(1, target.size());
    for (size_t col = 0; col < target.size(); col++) {
        target_matrix.Set(0, col, target[col]);
    }

    Matrix diff = output - target_matrix;
    double loss = 0.0;
    for (size_t row = 0; row < diff.Rows(); row++)
        for (size_t col = 0; col < diff.Cols(); col++)
            loss += diff.Get(row, col) * diff.Get(row, col);

    // Backwards pass 
    Matrix dprev = diff; // gradient of loss w.r.t output
    for (int layer_idx = layer_outputs.size() - 1; layer_idx >= 0; layer_idx--) {
        Matrix activation = layer_outputs[layer_idx];

        // derivative of sigmoid
        Matrix dactivation(activation.Rows(), activation.Cols());
        for (size_t row = 0; row < activation.Rows(); row++) {
            for (size_t col = 0; col < activation.Cols(); col++) {
                double a = activation.Get(row, col);
                dactivation.Set(row, col, a * (1 - a));
            }
        }

        // dz = dprev ⊙ sigmoid'(z)
        Matrix dz(activation.Rows(), activation.Cols());
        for (size_t row = 0; row < dz.Rows(); row++) {
            for (size_t col = 0; col < dz.Cols(); col++) {
                dz.Set(row, col, dprev.Get(row, col) * dactivation.Get(row, col));
            }
        }

        // input to this layer
        Matrix input_to_layer = (layer_idx == 0) ? input_matrix : layer_outputs[layer_idx - 1];

        // gradient w.r.t weights and biases
        Matrix dweights = input_to_layer.Transpose() * dz;

        Layer* layer_ptr = head;
        for (int i = 0; i < layer_idx; i++) layer_ptr = layer_ptr->next;

        // update weights
        layer_ptr->weights = layer_ptr->weights - (dweights * lr);

        // update biases
        Matrix dbias(1, dz.Cols());
        for (size_t col = 0; col < dz.Cols(); col++) {
            double sum = 0.0;
            for (size_t row = 0; row < dz.Rows(); row++) {
                sum += dz.Get(row, col);
            }
            dbias.Set(0, col, sum);
        }
        layer_ptr->biases = layer_ptr->biases - (dbias * lr);

        // propagate gradient to previous layer
        dprev = dz * layer_ptr->weights.Transpose();
    }

    return loss;
}

void NN::Train(size_t epochs, double lr, DataLoader& loader) {
    auto it = loader.Begin();
    for (size_t epoch = 0; epoch < epochs; epoch++) {
        double loss = 0.0;

        auto ptr = it.operator->();
        assert(ptr != nullptr);
        auto data = ptr->Clone();
        std::tuple<std::vector<Matrix>, std::vector<Matrix>> tuple = data->ToVector();
        Matrix X = std::get<0>(tuple)[0];
        Matrix Y = std::get<1>(tuple)[0];

        // iterate over samples
        for (size_t sample = 0; sample < X.Rows(); sample++) {
            std::vector<double> X_vec(inputs);
            std::vector<double> Y_vec(outputs);

            // extract sample
            for (size_t feature = 0; feature < X.Cols(); feature++) {
                X_vec.at(feature) = X.Get(sample, feature);
            }

            for (size_t out = 0; out < Y.Cols(); out++) {
                Y_vec.at(out) = Y.Get(sample, out);
            }

            loss += Backward(X_vec, Y_vec, lr);
        }

        if (epoch % 10 == 0) {
            std::cout << "Epoch " << epoch << " loss: " << loss << "\n";
        }

        it++;
    }
}