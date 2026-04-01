#include "nn.hpp"

std::mt19937 NN::gen{std::random_device{}()};
std::mt19937 DataLoader::gen{std::random_device{}()};

std::string ReadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLenum OpenGLShader::GetType() const {
    switch (this->type) {
        case ShaderType::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderType::Fragment:
            return GL_FRAGMENT_SHADER;
        case ShaderType::Compute:
            return GL_COMPUTE_SHADER;
        default:
            throw std::runtime_error("Invalid shader type");
    }
}

OpenGLShader::OpenGLShader() noexcept : Shader<GLuint>() {
    return;
}

OpenGLShader::OpenGLShader(const OpenGLShader& other) noexcept : Shader<GLuint>(other) {
    return;
}

OpenGLShader::OpenGLShader(OpenGLShader&& other) noexcept : Shader<GLuint>(other) {
    return;
}

void OpenGLShader::CreateShader() {
    if (!source.c_str()) {
        throw std::runtime_error("Shader source code not found");
    }

    shader = glCreateShader(GetType());
    const char* ptr = source.c_str();
    glShaderSource(shader, 1, &ptr, nullptr);
    glCompileShader(shader);
}

void OpenGLShader::DestroyShader() {
    glDeleteShader(shader);
}

void OpenGLShader::Clone(ShaderType type, const char* path) {
    this->type = type;
    this->path = path;
    this->source = ReadFile(path);
    this->CreateShader();
}

void OpenGLShader::Clone(ShaderType type, GLuint& other) {
    this->type = type;
    this->path = "";
    this->source = "";
    this->shader = other;
}

std::unique_ptr<Shader<GLuint>> OpenGLShader::CopyEmpty() {
    std::unique_ptr<Shader<GLuint>> new_shader;

    return new_shader;
}

std::unique_ptr<Shader<GLuint>> OpenGLShader::Copy() {
    std::unique_ptr<OpenGLShader> new_shader(new OpenGLShader(*this));

    return new_shader;
}

OpenGLShader::~OpenGLShader() {
    DestroyShader();
    return;
}

GLenum OpenGLBuffer::GetType() const {
    switch (type) {
        case BufferType::VertexBufferObject:
            return GL_ARRAY_BUFFER;
        case BufferType::UniformBufferObject:
            return GL_UNIFORM_BUFFER;
        case BufferType::ShaderStorageBufferObject:
            return GL_SHADER_STORAGE_BUFFER;
        default:
            throw std::runtime_error("Invalid buffer type");
    }
}

GLenum OpenGLBuffer::GetUsageType(BufferUsageType type) const {
    switch (type) {
        case BufferUsageType::DYNAMIC_COPY:
            return GL_DYNAMIC_COPY;
        case BufferUsageType::DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        case BufferUsageType::DYNAMIC_READ:
            return GL_DYNAMIC_READ;
        case BufferUsageType::STATIC_COPY:
            return GL_STATIC_COPY;
        case BufferUsageType::STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsageType::STATIC_READ:
            return GL_STATIC_READ;
        case BufferUsageType::STREAM_COPY:
            return GL_STREAM_COPY;
        case BufferUsageType::STREAM_DRAW:
            return GL_STREAM_DRAW;
        case BufferUsageType::STREAM_READ:
            return GL_STREAM_READ;
        default:
            throw std::runtime_error("Invalid buffer usage type");
    }
}

void OpenGLBuffer::GetReadWrite() {
    switch (type) {
        case BufferType::VertexBufferObject:
            read = false;
            write = true;
            break;
        case BufferType::UniformBufferObject:
            read = false;
            write = true;
            break;
        case BufferType::ShaderStorageBufferObject:
            read = true;
            write = true;
            break;
        default:
            throw std::runtime_error("Invalid buffer type");
    }
}

OpenGLBuffer::OpenGLBuffer() noexcept : Buffer<GLuint>() {
    return;
}

OpenGLBuffer::OpenGLBuffer(const OpenGLBuffer& other) noexcept : Buffer<GLuint>(other) {
    return;
}

OpenGLBuffer::OpenGLBuffer(OpenGLBuffer&& other) noexcept : Buffer<GLuint>(other) {
    return;
}

void OpenGLBuffer::CreateBuffer() {
    glGenBuffers(1, &buffer);
}

void OpenGLBuffer::DestroyBuffer() {
    glDeleteBuffers(1, &buffer);
}

void OpenGLBuffer::Clone(BufferType type, size_t binding) {
    DestroyBuffer();

    this->type = type;
    this->binding = binding;
    GetReadWrite();

    CreateBuffer();
}

void OpenGLBuffer::Clone(BufferType type, GLuint& other) {
    DestroyBuffer();

    this->type = type;
    this->buffer = other;
}

std::unique_ptr<Buffer<GLuint>> OpenGLBuffer::CopyEmpty() {
    std::unique_ptr<OpenGLBuffer> new_buffer;

    return new_buffer;
}

std::unique_ptr<Buffer<GLuint>> OpenGLBuffer::Copy() {
    std::unique_ptr<OpenGLBuffer> new_buffer(new OpenGLBuffer(*this));

    return new_buffer;
}

OpenGLBuffer::~OpenGLBuffer() {
    DestroyBuffer();
    return;
}

void OpenGLBuffer::Bind() const {
    glBindBuffer(GetType(), buffer);
}

void OpenGLBuffer::Unbind() const {
    glBindBuffer(GetType(), 0);
}

void OpenGLBuffer::SetData(size_t size, const void* data, BufferUsageType usage) {
    Bind();
    glBufferData(GetType(), size, data, GetUsageType(usage));
}

void OpenGLBuffer::Update(size_t offset, size_t size, const void* data) {
    Bind();
    glBufferSubData(GetType(), offset, size, data);
}

void OpenGLBuffer::BindBase(size_t binding) {
    if (type != BufferType::UniformBufferObject && type != BufferType::ShaderStorageBufferObject) {
        throw std::runtime_error("Only UBO and SSBO can be used in BindBase");
    }

    glBindBufferBase(GetType(), binding, buffer);
}

GLenum OpenGLVertexArray::GetVertexArrayAttribType(VertexArrayAttribType type) const {
    switch (type) {
        case VertexArrayAttribType::BYTE:
            return GL_BYTE;
        case VertexArrayAttribType::DOUBLE:
            return GL_DOUBLE;
        case VertexArrayAttribType::FLOAT:
            return GL_FLOAT;
        case VertexArrayAttribType::HALF_FLOAT:
            return GL_HALF_FLOAT;
        case VertexArrayAttribType::INT:
            return GL_INT;
        case VertexArrayAttribType::INT_2_10_10_10_REV:
            return GL_INT_2_10_10_10_REV;
        case VertexArrayAttribType::SHORT:
            return GL_SHORT;
        case VertexArrayAttribType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case VertexArrayAttribType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case VertexArrayAttribType::UNSIGNED_INT_2_10_10_10_REV:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case VertexArrayAttribType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        default:
            throw std::runtime_error("Invalid vertex array attrib type");
    }
}

OpenGLVertexArray::OpenGLVertexArray() noexcept : VertexArray<GLuint>() {
    return;
}

OpenGLVertexArray::OpenGLVertexArray(const OpenGLVertexArray& other) noexcept : VertexArray<GLuint>(other) {
    return;
}

OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray&& other) noexcept : VertexArray<GLuint>(other) {
    return;
}


void OpenGLVertexArray::CreateVertexArray() {
    glGenVertexArrays(1, &array);
}

void OpenGLVertexArray::DestroyVertexArray() {
    glDeleteVertexArrays(1, &array);
}

void OpenGLVertexArray::Clone(GLuint& other) {
    this->array = other;
}

std::unique_ptr<VertexArray<GLuint>> OpenGLVertexArray::CopyEmpty() {
    std::unique_ptr<OpenGLVertexArray> new_vertex_array;

    return new_vertex_array;
}

std::unique_ptr<VertexArray<GLuint>> OpenGLVertexArray::Copy() {
    std::unique_ptr<OpenGLVertexArray> new_vertex_array(new OpenGLVertexArray(*this));

    return new_vertex_array;
}

OpenGLVertexArray::~OpenGLVertexArray() {
    DestroyVertexArray();
    return;
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(array);
}

void OpenGLVertexArray::Unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexArray::AddAttribute(size_t index, size_t size, VertexArrayAttribType type, bool normalised, size_t stride, const void* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GetVertexArrayAttribType(type), normalised, stride, offset);
}

std::unique_ptr<Shader<GLuint>> OpenGLShaderProgram::Compile(ShaderType type, const char* path) {
    std::unique_ptr<OpenGLShader> shader(new OpenGLShader());
    shader->Clone(type, path);

    return shader;
}

size_t OpenGLShaderProgram::GetUniformLocation(const char* name) {
    if (uniforms.count(name)) {
        return uniforms[name];
    }

    GLint loc = glGetUniformLocation(program, name);

    uniforms[name] = loc;

    return loc;
}

GLenum OpenGLShaderProgram::GetMode(DrawMode mode) {
    switch (mode) {
        case DrawMode::POINTS:
            return GL_POINTS;
        case DrawMode::LINE_STRIP:
            return GL_LINE_STRIP;
        case DrawMode::LINE_LOOP:
            return GL_LINE_LOOP;
        case DrawMode::LINES:
            return GL_LINES;
        case DrawMode::LINE_STRIP_ADJACENCY:
            return GL_LINE_STRIP_ADJACENCY;
        case DrawMode::LINES_ADJACENCY:
            return GL_LINES_ADJACENCY;
        case DrawMode::TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case DrawMode::TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        case DrawMode::TRIANGLES:
            return GL_TRIANGLES;
        case DrawMode::TRIANGLE_STRIP_ADJACENCY:
            return GL_TRIANGLE_STRIP_ADJACENCY;
        case DrawMode::TRIANGLES_ADJACENCY:
            return GL_TRIANGLES_ADJACENCY;
        case DrawMode::PATCHES:
            return GL_PATCHES;
        default:
            throw std::runtime_error("Invalid drawing mode");
    }
}

OpenGLShaderProgram::OpenGLShaderProgram() noexcept : ShaderProgram<GLuint>() {
    return;
}

OpenGLShaderProgram::OpenGLShaderProgram(ShaderProgramPurpose purpose) noexcept : ShaderProgram<GLuint>(0, purpose) {
    // Create program
    this->program = {};
    this->CreateShaderProgram();
}

OpenGLShaderProgram::OpenGLShaderProgram(OpenGLShaderProgram&& other) noexcept : ShaderProgram<GLuint>((ShaderProgram<GLuint>&&)other) {
    return;
}

void OpenGLShaderProgram::CreateShaderProgram() {
    program = glCreateProgram();
}

void OpenGLShaderProgram::DestroyShaderProgram() {
    glDeleteProgram(program);
}

std::unique_ptr<ShaderProgram<GLuint>> OpenGLShaderProgram::CopyEmpty() {
    std::unique_ptr<OpenGLShaderProgram> new_program;
    
    return new_program;
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    DestroyShaderProgram();
    return;
}

void OpenGLShaderProgram::AttachFromFile(ShaderType type, const char* path) {
    std::unique_ptr<Shader<GLuint>> shader = Compile(type, path);

    num_shaders++;

    glAttachShader(program, shader->GetID());
}

void OpenGLShaderProgram::Link() {
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        throw std::runtime_error(log);
    }
}

void OpenGLShaderProgram::Use() const {
    glUseProgram(program);
}

void OpenGLShaderProgram::Unuse() {
    glUseProgram(0);
}

void OpenGLShaderProgram::SetInt(const char* name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void OpenGLShaderProgram::SetFloat(const char* name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void OpenGLShaderProgram::SetVec(const char* name, Vector<float>& value) {
    switch (value.Size()) {
        case 1:
            glUniform1fv(GetUniformLocation(name), 1, &*value.Begin());
            break;
        case 2:
            glUniform2fv(GetUniformLocation(name), 1, &*value.Begin());
            break;
        case 3:
            glUniform3fv(GetUniformLocation(name), 1, &*value.Begin());
            break;
        case 4:
            glUniform4fv(GetUniformLocation(name), 1, &*value.Begin());
            break;
        default:    
            throw std::runtime_error("Too many components in vector");
    }
}

void OpenGLShaderProgram::SetMat(const char* name, Matrix<float>& value) {
    if (value.Rows() != value.Cols()) {
        throw std::invalid_argument("Matrix must be square");
    }

    float data = value.Get(0, 0);

    switch (value.Rows()) {
        case 2:
            glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &data);
            break;
        case 3:
            glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &data);
            break;
        case 4:
            glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &data);
            break;
        default:
            throw std::runtime_error("Too many or too little components in matrix");
    }
}

void OpenGLShaderProgram::BindUBO(const char* block, size_t binding) {
    GLuint index = glGetUniformBlockIndex(program, block);

    glUniformBlockBinding(program, index, binding);
}

void OpenGLShaderProgram::BindSSBO(const char* block, size_t binding) {
    GLuint index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, block);

    glShaderStorageBlockBinding(program, index, binding);
}

void OpenGLShaderProgram::Dispatch(size_t x, size_t y, size_t z) {
    glDispatchCompute(x, y, z);
}

void OpenGLShaderProgram::WaitDispatchComplete() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void* OpenGLShaderProgram::GetDispatchResult() {
    SSBO->Bind();

    void* res = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    SSBO->Unbind();

    return res;
}

void OpenGLShaderProgram::Draw(VertexArray<GLuint>& VAO, DrawMode mode, std::vector<size_t> starts, std::vector<size_t> sizes) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    VAO.Bind();

    for (size_t i = 0; i < starts.size(); i++) {
        glDrawArrays(GetMode(mode), starts[i], sizes[i]);
    }
}

OpenGLWindow::OpenGLWindow() noexcept : Window<GLFWwindow>() {
    return;
}

OpenGLWindow::OpenGLWindow(size_t width, size_t height, const char* title) noexcept : Window<GLFWwindow>(width, height, title) {
    return;
}

OpenGLWindow::OpenGLWindow(OpenGLWindow&& other) noexcept : Window<GLFWwindow>((Window<GLFWwindow>&&)other) {
    this->CreateWindow();
    return;
}

void OpenGLWindow::CreateWindow() {
    this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);
}

void OpenGLWindow::DestroyWindow() {
    if (!window) return;

    glfwDestroyWindow(window);
}

void OpenGLWindow::Clone(size_t width, size_t height, const char* title) {
    this->DestroyWindow();
    
    this->width = width;
    this->height = height;
    this->title = title;

    CreateWindow();
}

void OpenGLWindow::Clone(GLFWwindow* other) {
    glfwGetWindowSize(other, (int*)&this->width, (int*)&this->height);
    this->title = "";
    DestroyWindow();
    this->window = other;
}

std::unique_ptr<Window<GLFWwindow>> OpenGLWindow::CopyEmpty() {
    std::unique_ptr<OpenGLWindow> new_window;

    return new_window;
}

OpenGLWindow::~OpenGLWindow() {
    DestroyWindow();
    return;
}

bool OpenGLWindow::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void OpenGLWindow::ClearData() {
    this->width = 0;
    this->height = 0;
    this->title = "";
    this->window = nullptr;
}

void OpenGLWindow::Display() {
    glfwSwapBuffers(window);
}

OpenGLContext::OpenGLContext() noexcept : GraphicsAPIContext<GLFWwindow, GLuint>() {
    Initialize();
    return;
}

void OpenGLContext::Initialize() {
    if (initialized) return;

    glfwInit();

    window = std::unique_ptr<OpenGLWindow>(new OpenGLWindow());
    window->Clone(1, 1, "A");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW initialization failed");
    }

    initialized = true;
}

void OpenGLContext::Uninitialize() {
    if (!initialized) return;

    glfwTerminate();

    initialized = false;
}

void OpenGLContext::Clone(size_t win_width, size_t win_height, const char* win_title) {
    this->window->Clone(win_width, win_height, win_title);
}

OpenGLContext::~OpenGLContext() {
    Uninitialize();
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