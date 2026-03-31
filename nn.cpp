#include "nn.hpp"

std::mt19937 NN::gen{std::random_device{}()};
std::mt19937 DataLoader::gen{std::random_device{}()};

std::string ReadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

DataSet::~DataSet() {}

DataLoader::DataLoader(DataSet* data, size_t num_batches, bool shuffle) : num_batches(num_batches), shuffle(shuffle) {
    batch_size = data->Size() / num_batches;

    auto vec = data->ToVector();

    if (shuffle) {
        std::vector<size_t> indices(data->Size());
        std::iota(indices.begin(), indices.end(), 0);

        std::shuffle(indices.begin(), indices.end(), gen);

        auto& Xvec = std::get<0>(vec);
        auto& Yvec = std::get<1>(vec);

        Xvec = std::vector<Matrix<double>>(indices.size());
        Yvec = std::vector<Matrix<double>>(indices.size());

        for (size_t i = 0; i < indices.size(); ++i) {
            Xvec[i] = Xvec[indices[i]];
            Yvec[i] = Yvec[indices[i]];
        }
    }

    for (size_t i = 0; i < num_batches; ++i) {
        size_t start = i * batch_size;

        if (start >= data->Size()) {
            break;
        }

        auto& Xvec = std::get<0>(vec);
        auto& Yvec = std::get<1>(vec);

        size_t total = Xvec.size();

        size_t end = std::min(start + batch_size, total);

        std::vector<Matrix<double>> subX(Xvec.begin() + start, Xvec.begin() + end);

        std::vector<Matrix<double>> subY(Yvec.begin() + start, Yvec.begin() + end);

        auto batchData = data->CloneEmpty();
        batchData->Copy(subX, subY);

        // Add the batch to the vector
        batches.push_back(std::move(batchData));
    }
}

DataLoader::Iterator DataLoader::Begin() noexcept {
    return Iterator(this, 0);
}

DataLoader::Iterator DataLoader::End() noexcept {
    return Iterator(this, batches.size());
}

DataLoader::Iterator::Iterator(DataLoader* loader, size_t index) noexcept {
    this->loader = loader;
    this->idx = index;
}

const DataSet& DataLoader::Iterator::operator*() const {
    return *(loader->batches[idx]);
}

DataSet* DataLoader::Iterator::operator->() const {
    return loader->batches[idx].get();
}

DataLoader::Iterator& DataLoader::Iterator::operator++() noexcept {
    idx = (idx + 1) % loader->batches.size();
    return *this;
}

DataLoader::Iterator DataLoader::Iterator::operator++(int) noexcept  {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool DataLoader::Iterator::operator==(const Iterator& other) const noexcept {
    return loader == other.loader && idx == other.idx;
}

bool DataLoader::Iterator::operator!=(const Iterator& other) const noexcept {
    return !(*this == other);
}

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
        Matrix<double> weights(prev_size, curr_size);

        for (size_t r = 0; r < weights.Rows(); r++) {
            for (size_t c = 0; c < weights.Cols(); c++) {
                weights.Set(r, c, dist(gen));
            }
        }

        // Initialize biases to random values
        Matrix<double> biases(1, curr_size);
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

inline double NN::MSEError(Matrix<double> output, Matrix<double> target) const {
    Matrix<double> diff = output - target;

    double sum = 0.0;
    for (size_t row = 0; row < diff.Rows(); row++) {
        for (size_t col = 0; col < diff.Cols(); col++) {
            sum += diff.Get(row, col) * diff.Get(row, col);
        }
    }

    return sum;
}

std::vector<double> NN::Forward(const std::vector<double>& inputs) const {
    // Convert inputs to 1×n matrix
    Matrix<double> current_input(1, inputs.size());
    for (size_t col = 0; col < inputs.size(); col++)
        current_input.Set(0, col, inputs[col]);

    // Pass through each layer
    Layer* current = head;
    while (current) {
        // z = xW + b
        Matrix<double> z = current_input * current->weights;
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
    Matrix<double> input_matrix(1, inputs.size());
    for (size_t col = 0; col < inputs.size(); col++) {
        input_matrix.Set(0, col, inputs[col]);
    }

    std::vector<Matrix<double>> layer_outputs;
    Matrix<double> current_input = input_matrix;

    Layer* current = head;
    while (current) {
        // z = xW + b
        Matrix<double> z = current_input * current->weights;
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

    Matrix<double> output = current_input; // final layer output

    // Get loss
    Matrix<double> target_matrix(1, target.size());
    for (size_t col = 0; col < target.size(); col++) {
        target_matrix.Set(0, col, target[col]);
    }

    Matrix<double> diff = output - target_matrix;
    double loss = MSEError(output, target_matrix);

    // Backwards pass 
    Matrix<double> dprev = diff; // gradient of loss w.r.t output
    for (int layer_idx = layer_outputs.size() - 1; layer_idx >= 0; layer_idx--) {
        Matrix<double> activation = layer_outputs[layer_idx];

        // derivative of sigmoid
        Matrix<double> dactivation(activation.Rows(), activation.Cols());
        for (size_t row = 0; row < activation.Rows(); row++) {
            for (size_t col = 0; col < activation.Cols(); col++) {
                double a = activation.Get(row, col);
                dactivation.Set(row, col, a * (1 - a));
            }
        }

        // dz = dprev ⊙ sigmoid'(z)
        Matrix<double> dz(activation.Rows(), activation.Cols());
        for (size_t row = 0; row < dz.Rows(); row++) {
            for (size_t col = 0; col < dz.Cols(); col++) {
                dz.Set(row, col, dprev.Get(row, col) * dactivation.Get(row, col));
            }
        }

        // input to this layer
        Matrix<double> input_to_layer = (layer_idx == 0) ? input_matrix : layer_outputs[layer_idx - 1];

        // gradient w.r.t weights and biases
        Matrix<double> dweights = input_to_layer.Transpose() * dz;

        Layer* layer_ptr = head;
        for (int i = 0; i < layer_idx; i++) layer_ptr = layer_ptr->next;

        // update weights
        layer_ptr->weights = layer_ptr->weights - (dweights * lr);

        // update biases
        Matrix<double> dbias(1, dz.Cols());
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
        auto data = ptr->Clone();
        std::tuple<std::vector<Matrix<double>>, std::vector<Matrix<double>>> tuple = data->ToVector();
        Matrix<double> X = std::get<0>(tuple)[0];
        Matrix<double> Y = std::get<1>(tuple)[0];

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