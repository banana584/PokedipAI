#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <tuple>
#include <utility>
#include <memory>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <initializer_list>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

template <typename T>
class Matrix;

template <typename T>
class Vector;

/**
 * @brief Reads a file and returns the contents.
 * 
 * This reads a file from a specified path and returns the contents.
 * 
 * @param path The path of the file to read.
 * @return The contents of the file read.
 */
std::string ReadFile(const char* path);

/**
 * @enum ShaderType
 * @brief Contains different purposes of shaders.
 * 
 * Has 3 different purposes of a shader: vertex (computes position of vertices); fragment (computes colour of each pixel); and compute (runs tasks in parallel).
 */
enum class ShaderType {
    Vertex, // Vertex shader
    Fragment, // Fragment shader
    Compute, // Compute shader
    None // Null
};

/**
 * @class Shader
 * @brief Generic representation of a shader.
 * 
 * Represents a shader in a graphics API - generic representation.
 */
template <typename S>
class Shader {
    protected:
        const char* path; // Path of source file.
        std::string source; // Source code.
        ShaderType type; // Type of shader - vertex, fragment, compute, etc.
        S shader{};

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values to Null.
         */
        Shader() noexcept {
            // All null values
            this->path = "";
            this->source = "";
            this->type = ShaderType::None;
            // Null with custom deleter.
            this->shader = {};
        }

        /**
         * @brief Copy constructor.
         * 
         * Initializes values in this to values in other.
         * 
         * @param other A const reference to another shader to copy values of.
         */
        Shader(const Shader<S>& other) noexcept {
            // Copy values
            this->path = other.path;
            this->source = other.source;
            this->type = other.type;
            this->shader = other.shader;
        }

        /**
         * @brief Move constructor.
         * 
         * Initializes values in this to values in other, then deletes values in other.
         * 
         * @param other Another shader to move data into this.
         */
        Shader(Shader<S>&& other) noexcept {
            // Copy values
            this->path = other.path;
            this->source = other.source;
            this->type = other.type;
            this->shader = other.shader;
            // Remove values from other
            other.path = "";
            other.source = "";
            other.type = ShaderType::None;
            other.shader = {};
        }

        /**
         * @brief Creates graphics API representation of shader from member variables.
         * 
         * A pure virtual function whose implementation should create/compile shader from member variables such as source and type. Result should be stores in shader variable.
         */
        virtual void CreateShader() = 0;

        /**
         * @brief Destroys graphics API representation of shader.
         * @warning Should only be used when reference count is 0 to prevent double frees or use after frees.
         * @warning Pointer is not deleted.
         * 
         * A pure virtual function whose implementation should destroy the shader from member variables but not delete the pointer.
         */
        virtual void DestroyShader() = 0;

        /**
         * @brief Creates a new shader from args.
         * 
         * A pure virtual function whose implementation should create a new shader based off args.
         * 
         * @param type The type of shader to create.
         * @param path The path to the source code of the shader.
         */
        virtual void Clone(ShaderType type, const char* path) = 0;

        /**
         * @brief Creates a new shader from args.
         * 
         * A pure virtual function whose implementation should create a new shader based off args.
         * 
         * @param type The type of shader to create.
         * @param other Graphics API representation of shader.
         */
        virtual void Clone(ShaderType type, S& other) = 0;

        /**
         * @brief Creates unique pointer to empty version of Shader.
         * 
         * A pure virtual function whose implementation should create a new empty shader and return it using a covariant return type.
         * 
         * @return A unique pointer to an empty shader.
         */
        virtual std::unique_ptr<Shader<S>> CopyEmpty() = 0;

        /**
         * @brief Creates unique pointer to copy of this.
         * 
         * A pure virtual function whose implementation should create a new copy of this and return it using a covariant return type.
         * 
         * @return A unique pointer to a copy of this.
         */
        virtual std::unique_ptr<Shader<S>> Copy() = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should clean up all resources not handled already.
         */
        virtual ~Shader() {return;};

        /**
         * @brief Gets id under the hood.
         * 
         * Retrieves the graphics API representation of the shader.
         * 
         * @return The id of the shader.
         */
        S GetID() const noexcept {
            return shader;
        }

        /**
         * @brief Copy operator.
         * 
         * Copies data from other and puts it into this.
         * 
         * @param other The other shader to copy data from.
         * @return The shader with newly copied data.
         */
        Shader<S>& operator=(const Shader<S>& other) noexcept {
            // Equality check
            if (this == &other) return *this;

            // Copy variables
            this->path = other.path;
            this->source = other.source;
            this->type = other.type;
            this->shader = other.shader;

            return *this;
        }

        /**
         * @brief Move operator.
         * 
         * Moves data from other and puts it into this.
         * 
         * @param other The other shader to move data from.
         * @return The shader with newly moved data.
         */
        Shader<S>& operator=(Shader<S>&& other) noexcept {
            // Equality check
            if (this == &other) return *this;

            // Copy data into this
            this->path = other.path;
            this->source = other.source;
            this->type = other.type;
            this->shader = other.shader;
            // Remove data from other
            other.path = "";
            other.source = "";
            other.type = ShaderType::None;
            other.shader = {};

            return *this;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 shaders are equal.
         * 
         * @param other A different shader to compare to this.
         * @return True if they are equal, false otherwise.
         */
        bool operator==(const Shader<S>& other) const noexcept {
            return this->path == other.path && *this->shader == *other.shader && this->source == other.source && this->type == other.type;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 shaders are not equal.
         * 
         * @param other A different shader to compare to this.
         * @return True if they are not equal, false otherwise.
         */
        bool operator!=(const Shader<S>& other) const noexcept {
            return !(*this == other);
        }

        // Delete other comparison operators.
        bool operator>(const Shader<S>& other) = delete;
        bool operator>=(const Shader<S>& other) = delete;
        bool operator<(const Shader<S>& other) = delete;
        bool operator<=(const Shader<S>& other) = delete;
};

/**
 * @enum BufferType
 * @brief The type of buffer.
 * 
 * Contains generic types of graphics API buffers.
 */
enum class BufferType {
    VertexBufferObject, // VBO
    UniformBufferObject, // UBO
    ShaderStorageBufferObject, // SSBO
    None // Null
};

/**
 * @enum BufferUsageType
 * @brief Contains different operations buffers can do.
 * 
 * Contains generic types of operations graphics buffers can perform.
 */
enum class BufferUsageType {
    STREAM_DRAW, // Draw
    STREAM_READ, // Read
    STREAM_COPY, // Copy
    STATIC_DRAW, // Draw
    STATIC_READ, // Read
    STATIC_COPY, // Copy
    DYNAMIC_DRAW, // Draw
    DYNAMIC_READ, // Read
    DYNAMIC_COPY // Copy
};

/**
 * @class Buffer
 * @brief A generic representation of a graphics API buffer, for example VBO or SSBO.
 * 
 * Contains information about generic representations of graphics buffers.
 */
template <typename B>
class Buffer {
    protected:
        BufferType type; // The type of buffer - for example VBO or UBO
        bool read; // If it can be read from
        bool write; // If it can be written to
        size_t binding; // Binding on shader side
        B buffer{}; // Graphics API representation

    public:
        /**
         * @brief Constructor.
         * 
         * Initializes all values to null.
         */
        Buffer() noexcept {
            // Null values
            this->type = BufferType::None;
            this->read = false;
            this->write = false;
            this->binding = 0;
            // Set custom deleter
            this->buffer = {};
        }

        /**
         * @brief Copy constructor.
         * 
         * Copies data from other into this.
         * 
         * @param other A const reference to another buffer to copy data from.
         */
        Buffer(const Buffer<B>& other) noexcept {
            // Copy data from other into this
            this->type = other.type;
            this->read = other.read;
            this->write = other.write;
            this->binding = other.binding;
            this->buffer = other.buffer;
        }

        /**
         * @brief Move constructor
         * 
         * Moves data from other into this.
         * 
         * @param other A different buffer to move data from.
         */
        Buffer(Buffer<B>&& other) noexcept {
            // Copy data
            this->type = other.type;
            this->read = other.read;
            this->write = other.write;
            this->binding = other.binding;
            this->buffer = other.buffer;
            // Delete data from other
            other.type = BufferType::None;
            other.read = false;
            other.write = false;
            other.binding = 0;
            other.buffer = {};
        }

        /**
         * @brief Creates a buffer from member varaibles.
         * 
         * A pure virtual function whose implementation should create the graphics API representation of a buffer.
         */
        virtual void CreateBuffer() = 0;

        /**
         * @brief Destroys graphics API representation of buffer.
         * @warning Should only be used when reference count is 0 to prevent double frees or use after frees.
         * @warning Pointer is not deleted.
         * 
         * A pure virtual function whose implementation should destroy the buffer from member variables but not delete the pointer.
         */
        virtual void DestroyBuffer() = 0;

        /**
         * @brief Creates a new buffer from args.
         * 
         * A pure virtual function whose implementation should construct a new buffer from arguments.
         * 
         * @param type The type of buffer to construct.
         * @param binding The binding of the buffer to construct.
         */
        virtual void Clone(BufferType type, size_t binding) = 0;

        /**
         * @brief Creates a new buffer from args.
         * 
         * A pure virtual function whose implementation should constructs a new buffer from arguments.
         * 
         * @param type The type of buffer to construct.
         * @param other A graphics API representation of buffer.
         */
        virtual void Clone(BufferType type, B& other) = 0;

        /**
         * @brief Creates an empty buffer representation.
         * 
         * A pure virtual function whose implementation should create a new empty buffer and return it.
         * 
         * @return A unique pointer to a new empty buffer representation.
         */
        virtual std::unique_ptr<Buffer<B>> CopyEmpty() = 0;

        /**
         * @brief Creates a new buffer representation which is a copy of this.
         * 
         * A pure virtual function whose implementation should create a new buffer representation filled with the data of this.
         * 
         * @return A unique pointer to a new buffer, which has a copy of data from this.
         */
        virtual std::unique_ptr<Buffer<B>> Copy() = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should clean up all resources that are not already handled.
         */
        virtual ~Buffer() {};

        /**
         * @brief Gets the graphics API representation of the buffer.
         * 
         * Returns raw graphics API buffer.
         * 
         * @return The raw buffer id.
         */
        B GetID() const noexcept {
            return buffer;
        }

        /**
         * @brief Binds a buffer to a target.
         * 
         * A pure virtual function whose implementation should convert type to the graphics API and bind.
         */
        virtual void Bind() const = 0;

        /**
         * @brief Unbinds a buffer from its target.
         * @warning Buffer must be binded before this is called.
         * 
         * A pure virtual function whose implementation should unbind the buffer from its target.
         */
        virtual void Unbind() const = 0;

        /**
         * @brief Binds buffer and adds data.
         * 
         * A pure virtual function whose implementation should bind the buffer using the Bind() function, then add data on.
         * 
         * @param size The size of the buffer objects new data store.
         * @param data A void pointer to the data that will be buffered.
         * @param usage The expected usage pattern of the data store.
         */
        virtual void SetData(size_t size, const void* data, BufferUsageType usage) = 0;

        /**
         * @brief Binds buffer and updates a subset of the buffer's data store.
         * 
         * A pure virtual function whose implementation should bind the buffer using the Bind() function, then update a subset of the data store.
         * 
         * @param offset The offset into the data store where the data should be updated.
         * @param size The size of the data being replaced.
         * @param data The new data to write into the buffer data store.
         */
        virtual void Update(size_t offset, size_t size, const void* data) = 0;

        /**
         * @brief Binds buffer to indexed buffer target in shader.
         * 
         * A pure virtual function whose implementation should bind the buffer to a location in the shader.
         * 
         * @param binding The location in the shader for the buffer to be binded to.
         */
        virtual void BindBase(size_t binding) = 0;

        /**
         * @brief Copy operator.
         * 
         * Copies data from other into this.
         * 
         * @param other A const reference to a buffer whose data will be copied into this.
         * @return The buffer with newly copied data.
         */
        Buffer<B>& operator=(const Buffer<B>& other) noexcept {
            // Check if equal
            if (this == &other) return *this;

            // Copy data from other to this
            this->type = other.type;
            this->read = other.read;
            this->write = other.write;
            this->binding = other.binding;
            this->buffer = other.buffer;

            return *this;
        }

        /**
         * @brief Move operator.
         * 
         * Moves data from other into this.
         * 
         * @param other Another buffer whose data will be moved into this.
         * @return The buffer with the newly moved data.
         */
        Buffer<B>& operator=(Buffer<B>&& other) noexcept {
            // Check if equal
            if (this == &other) return *this;

            // Copy data into this
            this->type = other.type;
            this->read = other.read;
            this->write = other.write;
            this->binding = other.binding;
            this->buffer = other.buffer;
            // Delete other's data
            other.type = BufferType::None;
            other.read = false;
            other.write = false;
            other.binding = 0;
            other.buffer = {};

            return *this;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 buffers are equal.
         * 
         * @param other A const reference to another buffer to check against this.
         * @return True if they are equal, otherwise false.
         */
        bool operator==(const Buffer<B>& other) const noexcept {
            return this->binding == other.binding && this->buffer == other.buffer && this->read = other.read && this->write == other.write && this->type == other.type;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 buffers are not equal.
         * 
         * @param other A const reference to another buffer to check against this.
         * @return True if they are not equal, otherwise false.
         */
        bool operator!=(const Buffer<B>& other) const noexcept {
            return !(*this == other);
        }

        // Delete other comparison operators.
        bool operator>(const Buffer<B>& other) = delete;
        bool operator>=(const Buffer<B>& other) = delete;
        bool operator<(const Buffer<B>& other) = delete;
        bool operator<=(const Buffer<B>& other) = delete;
};

/**
 * @enum VertexArrayAttribType
 * @brief The type of attribute in a vertex array.
 * 
 * Contains information about the type of each attribute inside a vertex array.
 */
enum class VertexArrayAttribType {
    BYTE, // Signed byte
    UNSIGNED_BYTE, // Unsigned byte
    SHORT, // Signed short
    UNSIGNED_SHORT, // Unsigned short
    INT, // Signed int
    UNSIGNED_INT, // Unsigned int
    HALF_FLOAT, // Half size float
    FLOAT, // Full size float
    DOUBLE, // Double size float
    INT_2_10_10_10_REV, // 4 values packed into signed int
    UNSIGNED_INT_2_10_10_10_REV // 4 values packed into unsigned int
};

/**
 * @class VertexArray
 * @brief Generic representation of vertex arrays.
 * 
 * Represents a vertex array, for example an OpenGL VAO.
 */
template <typename A>
class VertexArray {
    protected:
        // Graphics API representation
        A array{};

    public:
        /**
         * @brief Constructor.
         * 
         * Sets array to nullptr, with custom deleter.
         */
        VertexArray() noexcept {
            array = {};
        }

        /**
         * @brief Copy constructor
         * 
         * Copies data from other into this.
         * 
         * @param other A const reference to another vertex array.
         */
        VertexArray(const VertexArray& other) noexcept {
            this->array = other.array;
        }

        /**
         * @brief Move constructor.
         * 
         * Moves data from other into this.
         * 
         * @param other Another vertex array to have its data moved into this.
         */
        VertexArray(VertexArray&& other) noexcept {
            this->array = other.array;
            other.array = {};
        }

        /**
         * @brief Creates a graphics API representation of the vertex array.
         * 
         * A pure virtual function whose implementation should create the vertex array and store it.
         */
        virtual void CreateVertexArray() = 0;

        /**
         * @brief Destroyes the graphics API representation of the vertex array.
         * @warning Should only be used when reference count is 0 to prevent double frees or use after frees.
         * @warning Pointer is not deleted.
         * 
         * A pure virtual function whose implementation should destroy the vertex array from member variables but not delete the pointer.
         */
        virtual void DestroyVertexArray() = 0;

        /**
         * @brief Copies vertex array from graphics API representation
         * 
         * A pure virtual function whose implementation should copy data from a graphics API representation of a vertex array.
         * 
         * @param other A graphics API representation of a vertex array.
         */
        virtual void Clone(A& other) = 0;

        /**
         * @brief Creates an empty version of a vertex array.
         * 
         * A pure virtual function whose implementation should create a unique pointer of an empty vertex array.
         * 
         * @return A unique pointer of an empty vertex array.
         */
        virtual std::unique_ptr<VertexArray<A>> CopyEmpty() = 0;

        /**
         * @brief Creates a copy of this.
         * 
         * A pure virtual function whose implementation should create a copy of this.
         * 
         * @return A unique pointer to a copy of this.
         */
        virtual std::unique_ptr<VertexArray<A>> Copy() = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should clean up any unhandled resources.
         */
        virtual ~VertexArray() {};

        /**
         * @brief Gets vertex array under the hood.
         * 
         * Retrieves the graphics API representation of the vertex array.
         * 
         * @return The id of the vertex array.
         */
        A GetID() const noexcept {
            return *array;
        }

        /**
         * @brief Binds a vertex array in the graphics API.
         * 
         * A pure virtual function whose implementation should bind the vertex array.
         */
        virtual void Bind() const = 0;

        /**
         * @brief Unbinds a vertex array in the graphics API.
         * 
         * A pure virtual function whose implementation should unbind the vertex array.
         */
        virtual void Unbind() const = 0;

        /**
         * @brief Define an array of generic vertex attribute data.
         * 
         * A pure virtual function whose implementation should write data into the vertex array.
         * 
         * @param index The index of the generic vertex attribute to be modified.
         * @param size The number of components per generic vertex attribute.
         * @param type The type of data of each component in the array.
         * @param normalised If fixed point data values should be normalised.
         * @param stride The byte offset between consecutive generic vertex attributes.
         * @param offset The offset of the first component of the first generic vertex attribute in the array.
         */
        virtual void AddAttribute(size_t index, size_t size, VertexArrayAttribType type, bool normalised, size_t stride, const void* offset) = 0;

        /**
         * @brief Copy operator.
         * 
         * Copies values from other into this.
         * 
         * @param other A const reference to another vertex array whose values will be copied into this.
         * @return The vertex array with newly copied values.
         */
        VertexArray<A>& operator=(const VertexArray<A>& other) noexcept {
            if (this == &other) return *this;
            
            this->array = other.array;

            return *this;
        }

        /**
         * @brief Move operator.
         * 
         * Moves values from other into this.
         * 
         * @param other Another vertex array whose values will be moved into this.
         * @return The vertex array with newly moved values.
         */
        VertexArray<A>& operator=(VertexArray<A>&& other) noexcept {
            if (this == &other) return *this;

            this->array = other.array;
            other.array = {};

            return *this;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 vertex arrays are equal.
         * 
         * @param other Another vertex array to compare to this.
         * @return True if they are equal, false otherwise.
         */
        bool operator==(const VertexArray<A>& other) const noexcept {
            return this->array == other.array;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 vertex arrays are not equal.
         * 
         * @param other Another vertex array to compare to this.
         * @return True if they are not equal, flase otherwise.
         */
        bool operator!=(const VertexArray<A>& other) const noexcept {
            return !(*this == other);
        }

        // Delete other comparison operators.
        bool operator>(const VertexArray<A>& other) = delete;
        bool operator>=(const VertexArray<A>& other) = delete;
        bool operator<(const VertexArray<A>& other) = delete;
        bool operator<=(const VertexArray<A>& other) = delete;
};

/**
 * @enum ShaderProgramPurpose
 * @brief Specifies the purpose of a shader program
 * 
 * States what a shader program does, if that may be rendering or computing.
 */
enum class ShaderProgramPurpose {
    Render, // Rendering
    Compute, // Computing
    None // Null
};

/**
 * @enum DrawMode
 * @brief Specifies how vertices should be drawn.
 * 
 * States how vertices given to shaders should be drawn.
 */
enum class DrawMode {
    POINTS,
    LINE_STRIP,
    LINE_LOOP,
    LINES,
    LINE_STRIP_ADJACENCY,
    LINES_ADJACENCY,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLES,
    TRIANGLE_STRIP_ADJACENCY,
    TRIANGLES_ADJACENCY,
    PATCHES,
    None
};

/**
 * @class ShaderProgram
 * @brief Generic representation of a shader program.
 * 
 * Represents a shader program, whether it is used to draw or compute.
 */
template <typename I>
class ShaderProgram {
    protected:
        // Amount of shaders
        size_t num_shaders;
        // What the shader program is used for
        ShaderProgramPurpose purpose;
        // UBO and SSBO
        std::unique_ptr<Buffer<I>> UBO;
        std::unique_ptr<Buffer<I>> SSBO;
        // Graphics API representation of shader program
        I program;
        // Uniform location cache
        std::unordered_map<std::string, size_t> uniforms;

    protected:
        /**
         * @brief Compiles a shader into the Shader class from source.
         * 
         * A pure virtual function whose implementation should compile shader source into the Shader class.
         * 
         * @param type The type of shader being compiled.
         * @param path The source code location of the shader being compiled.
         * @return The newly created Shader instance in a unique pointer.
         */
        virtual std::unique_ptr<Shader<I>> Compile(ShaderType type, const char* path) = 0;

        /**
         * @brief Gets the location of a uniform in a shader from name.
         * 
         * A pure virtual function whose implementation should check the uniform cache, before checking with the graphics API if it is not found.
         * 
         * @param name The name of the uniform being searched for.
         * @return The location of the uniform searched for.
         */
        virtual size_t GetUniformLocation(const char* name) = 0;

    public:
        /**
         * @brief Constructor.
         * 
         * Sets all values to null.
         */
        ShaderProgram() noexcept {
            // Initialize to all 0/null
            this->num_shaders = 0;
            this->purpose = ShaderProgramPurpose::None;
            this->UBO = nullptr;
            this->SSBO = nullptr;
            // Preserve custom deleter
            this->program = {};
        }

        /**
         * @brief Constructor.
         * 
         * Initilizes all values.
         * 
         * @param num_shaders The amount of shaders in the program.
         * @param purpose The purpose of the shader program.
         */
        ShaderProgram(size_t num_shaders, ShaderProgramPurpose purpose) noexcept {
            // Initialize varaibles with args
            this->num_shaders = num_shaders;
            this->purpose = purpose;
            // Preserve deleters
            this->program = {};
        }

        /**
         * @brief Move constructor
         * 
         * Moves values from other into this.
         * 
         * @param other Another shader program whose values will be moved into this.
         */
        ShaderProgram(ShaderProgram<I>&& other) noexcept {
            // Copy values into this
            this->num_shaders = other.num_shaders;
            this->purpose = other.purpose;
            this->program = other.program;
            // Remove variables from other
            other.num_shaders = 0;
            other.purpose = ShaderProgramPurpose::None;
            other.program = {};
        }

        /**
         * @brief Creates a graphics API representation of the program.
         * 
         * A pure virtual function whose implementation should create a shader program in the graphics API.
         */
        virtual void CreateShaderProgram() = 0;

        /**
         * @brief Destroys the shader program in the graphics API.
         * @warning Should only be used when reference count is 0 to prevent double frees or use after frees.
         * @warning Pointer is not deleted.
         * 
         * A pure virtual function whose implementation should destroy the shader program from member variables but not delete the pointer.
         */
        virtual void DestroyShaderProgram() = 0;

        /**
         * @brief Copies a UBO into this.
         * 
         * Copies the data from an existing UBO into this.
         * 
         * @param UBO An existing UBO to copy.
         */
        void CreateUBO(Buffer<I>& UBO) {
            this->UBO = UBO.Copy();
        }

        /**
         * @brief Copies an SSBO into this.
         * 
         * Copies the data from an existing SSBO into this.
         * 
         * @param SSBO An existing SSBO to copy.
         */
        void CreateSSBO(Buffer<I>& SSBO) {
            this->SSBO = SSBO.Copy();
        }

        /**
         * @brief Creates a new empty shader program.
         * 
         * A pure virtual function whose implementation should create a new empty shader program.
         * 
         * @return A unique pointer to an empty shader program.
         */
        virtual std::unique_ptr<ShaderProgram<I>> CopyEmpty() = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should clean up any unhandled resources.
         */
        virtual ~ShaderProgram() {};

        /**
         * @brief Gets program under the hood.
         * 
         * Retrives the graphics API representation of the program.
         * 
         * @return The id of the program in the graphics API.
         */
        I GetID() const noexcept {
            return program;
        }

        /**
         * @brief Attaches a shader to the program from file.
         * 
         * A pure virtual function whose implementation should compile the shader, before attaching it to the program.
         * 
         * @param type The type of shader being attached.
         * @param path The path of the file where the shader source is stored.
         */
        virtual void AttachFromFile(ShaderType type, const char* path) = 0;

        /**
         * @brief Links the shaders together into the program.
         * 
         * A pure virtual function whose implementation should link the shaders into the program.
         */
        virtual void Link() = 0;

        /**
         * @brief Attaches program to rendering or computing.
         * @warning Must be called before rendering or computing.
         * 
         * A pure virtual function whose implementation should use the program. This is called before rendering or computing.
         */
        virtual void Use() const = 0;

        /**
         * @brief Deattaches program to rendering or computing.
         * 
         * A pure virtual function whose implementation should unuse the program. This is caleld after rendering or computing.
         */
        virtual void Unuse() = 0;

        /**
         * @brief Sets a uniform in the shader to an int value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to an int value.
         * 
         * @param name The name of the uniform being set.
         * @param value The value being set to the uniform.
         */
        virtual void SetInt(const char* name, int value) = 0;

        /**
         * @brief Sets a uniform in the shader to a float value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to a float value.
         * 
         * @param name The name of the uniform being set.
         * @param value The value being set to the uniform.
         */
        virtual void SetFloat(const char* name, float value) = 0;

        /**
         * @brief Sets a uniform in the shader to a vector value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to a vector value.
         * 
         * @param name The name of the uniform being set.
         * @param value The values being set to the uniform.
         */
        virtual void SetVec(const char* name, Vector<float>& value) = 0;

        /**
         * @brief Sets a uniform in the shader to a matrix value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to a matrix value.
         * 
         * @param name The name of the uniform being set.
         * @param value The values being set to the uniform.
         */
        virtual void SetMat(const char* name, Matrix<float>& value) = 0;

        virtual void BindUBO(const char* block, size_t binding) = 0;

        virtual void BindSSBO(const char* block, size_t binding) = 0;

        /**
         * @brief Dispatches tasks to run in parallel.
         * 
         * A pure virtual function whose implementation should check if the shader is compute, and if it is dispatch an amount of tasks to run on the GPU.
         * 
         * @param x The amount of threads to run in the x direction.
         * @param y The amount of threads to run in the y direction.
         * @param z The amount of threads to run in the z direction.
         */
        virtual void Dispatch(size_t x, size_t y, size_t z) = 0;

        /**
         * @brief Waits for dispatched tasks to complete.
         * 
         * A pure virtual function whose implementation should wait until dispatched commands are complete, then return.
         */
        virtual void WaitDispatchComplete() = 0;

        /**
         * @brief Gets results from dispatched task.
         * @warning The tasks must be complete before calling this. Use WaitDispatchComplete() to make sure it is safe to call this.
         * 
         * A pure virtual function whose implementation should get the result of a dispatched task.
         * 
         * @return The result from dispatched tasks.
         */
        virtual void* GetDispatchResult() = 0;

        /**
         * @brief Draws vertices to the pipeline.
         * @warning If you want to see shapes, the window must be swapped.
         * 
         * A pure virtual function whose implementation should send vertices down the pipeline, but not update the window.
         */
        virtual void Draw(VertexArray<I>& VAO, DrawMode mode, std::vector<size_t> starts, std::vector<size_t> sizes) = 0;

        /**
         * @brief Move operator.
         * 
         * Moves data from other into this.
         * 
         * @param other Another program whose data will be moved into this.
         * @return The program with the newly moved data.
         */
        ShaderProgram<I>& operator=(ShaderProgram<I>&& other) noexcept {
            if (this == &other) return *this;
            
            // Copy values into this
            this->num_shaders = other.num_shaders;
            this->num_buffers = other.num_buffers;
            this->num_vertex_arrays = other.num_vertex_arrays;
            this->purpose = other.purpose;
            this->program = other.program;
            // Deep copy vectors
            this->buffers = std::vector<Buffer<I>>(other.buffers.begin(), other.buffers.end());
            this->vertex_arrays = std::vector<VertexArray<I>>(other.vertex_arrays.begin(), other.vertex_arrays.end());
            // Remove variables from other
            other.num_shaders = 0;
            other.num_buffers = 0;
            other.num_vertex_arrays = 0;
            other.purpose = ShaderProgramPurpose::None;
            other.buffers.clear();
            other.vertex_arrays.clear();
            other.program = {};

            return *this;
        }
        
        /**
         * @brief Equality operator.
         * 
         * Checks if 2 programs are equal.
         * 
         * @param other A const reference to another program.
         * @return True if they are equal, otherwise false.
         */
        bool operator==(const ShaderProgram<I>& other) const noexcept {
            return this->num_shaders == other.num_shaders && this->purpose == other.purpose && this->program == other.program;
        }

        /**
         * @brief Inequality operator.
         * 
         * Check if 2 programs are not equal.
         * 
         * @param other A const reference to another program.
         * @return True if they are not equal, otherwise false.
         */
        bool operator!=(const ShaderProgram<I>& other) const noexcept {
            return !(*this == other);
        }

        // Copy operator and constructor won't work because of unique pointers
        ShaderProgram(const ShaderProgram<I>& other) = delete;
        ShaderProgram<I>& operator=(const ShaderProgram<I>& other) = delete;
        // Delete other comparison operators.
        bool operator>(const ShaderProgram<I>& other) = delete;
        bool operator>=(const ShaderProgram<I>& other) = delete;
        bool operator<(const ShaderProgram<I>& other) = delete;
        bool operator<=(const ShaderProgram<I>& other) = delete;
};

/**
 * @class Window
 * @brief A generic representation of a window in graphics APIs.
 * 
 * A class that represents a window in supported graphics APIs.
 */
template <typename W>
class Window {
    protected:
        // Size of window
        size_t width;
        size_t height;
        // Window title
        const char* title;
        // Graphics API representation
        W* window{};

    public: 
        /**
         * @brief Default constructor.
         * 
         * Intializes all values to 0 or null.
         */
        Window() noexcept {
            this->width = 0;
            this->height = 0;
            this->title = "";
            this->window = {};
        }

        /**
         * @brief Constructor.
         * 
         * Initializes values to args.
         * 
         * @param width The width of the window.
         * @param height The height of the window.
         * @param title The title of the window - text displayed on titlebar.
         */
        Window(size_t width, size_t height, const char* title) noexcept {
            this->width = width;
            this->height = height;
            this->title = title;
        }

        // Window is unique pointer, no copy operations
        Window(const Window<W>& other) = delete;

        /**
         * @brief Move constructor.
         * 
         * Moves data from other into this, and set other's data to 0/null.
         * 
         * @param other A different window whose data will be moved into this.
         */
        Window(Window<W>&& other) noexcept {
            // Move values
            this->width = other.width;
            this->height = other.height;
            this->title = other.title;
            
            // Set other's values to null
            other.width = 0;
            other.height = 0;
            other.title = "";
            other.DestroyWindow();
            other.window = nullptr;
        }

        /**
         * @brief Creates a window according to member variables in graphics API.
         * 
         * A pure virtual function whose implementation should create a window in the graphics API, according to specified member variables.
         */
        virtual void CreateWindow() = 0;

        /**
         * @brief Destroyes window in graphics API.
         * @warning Should only be used when reference count is 0 to prevent double frees or use after frees.
         * @warning Pointer is not deleted.
         * 
         * A pure virtual function whose implementation should destroy the window if it can in the graphics API, if not then just return and set to null.
         */
        virtual void DestroyWindow() = 0;

        /**
         * @brief Clones data from args.
         * 
         * A pure virtual function whose implementation should initialize member variables from args.
         * 
         * @param width The width of the window.
         * @param height The height of the window.
         * @param title The title of the window.
         */
        virtual void Clone(size_t width, size_t height, const char* title) = 0;

        /**
         * @brief Clones data from args.
         * 
         * A pure virtual function whose implementation should initialize member varaibles from args.
         * 
         * @param other A pointer to a graphics API representation of the window.
         */
        virtual void Clone(W* other) = 0;

        /**
         * @brief Creates an empty window.
         * 
         * A pure virtual function whose implementation should create a new empty Window instance.
         * 
         * @return A unique pointer to an empty window.
         */
        virtual std::unique_ptr<Window<W>> CopyEmpty() = 0;

        /**
         * @brief Cleans up any resources not handled already.
         * 
         * A pure virtual function whose implementation should clean up resources not already accounted for.
         */
        virtual ~Window() {return;};

        /**
         * @brief Checks if the window should close.
         * 
         * A pure virtual function whose implementation should check if the window should close.
         * 
         * @return True if the window should close, otherwise false.
         */
        virtual bool ShouldClose() = 0;

        /**
         * @brief Gets window id under the hood.
         * 
         * Retrieves the graphics API window id.
         * 
         * @return The window in the graphics API.
         */
        W* GetID() const noexcept {
            return window;
        }

        /**
         * @brief Set all data to null.
         * @warning Should not call delete or DestroyWindow, unique pointer handles that.
         * 
         * A pure virtual function whose implementation should clear all data in this.
         */
        virtual void ClearData() = 0;

        /**
         * @brief Swaps display buffers.
         * 
         * A pure virtual function whose implementation should call the graphics API to render.
         */
        virtual void Display() = 0;

        /**
         * @brief Move operator.
         * 
         * Moves data from other into this, and clears other.
         * 
         * @param other A different window to move data into this.
         * @return The window with newly moved data.
         */
        Window<W>& operator=(Window<W>&& other) noexcept {
            if (this == &other) return *this;

            this->width = other.width;
            this->height = other.height;
            this->title = other.title;
            this->window = other.window;

            other.width = 0;
            other.height = 0;
            other.title = "";
            other.DestroyWindow();
            delete other.window;
            other.window = nullptr;

            return *this;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 windows are equal.
         * 
         * @param other A different window to compare against this.
         * @return True if they are equal, otherwise false.
         */
        bool operator==(const Window<W>& other) const noexcept {
            return this->width == other.width && this->height == other.height && this->title == other.title;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 windows are not equal.
         * 
         * @param other A different window to compare against this.
         * @return True if they are not equal, otherwise false.
         */
        bool operator!=(const Window<W>& other) const noexcept {
            return !(*this == other);
        }
};

/**
 * @class GraphicsAPIContext
 * @brief Generic representation of the context.
 * 
 * A class that represents the context of the graphics API. Should handle initialization and cleanup of graphics API.
 */
template <typename W, typename I>
class GraphicsAPIContext {
    protected:
        // If graphics API is initialized.
        bool initialized;
        // The window - required for loading graphics API.
        std::unique_ptr<Window<W>> window;
        // Vertex buffer - not required.
        std::unique_ptr<Buffer<I>> VBO;
        // Vertex array - not required.
        std::unique_ptr<VertexArray<I>> VAO;
        // Shader program.
        std::unique_ptr<ShaderProgram<I>> program;

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values in this to 0/null.
         */
        GraphicsAPIContext() noexcept {
            this->initialized = false;
            this->window = nullptr;
            this->VBO = nullptr;
            this->VAO = nullptr;
            this->program = nullptr;
        }

        /**
         * @brief Constructor.
         * 
         * Sets member varaibles to args and initializes.
         * 
         * @param win_width The width of the window.
         * @param win_height The height of the window.
         * @param win_title The title of the window.
         */
        GraphicsAPIContext(size_t win_width, size_t win_height, const char* win_title) noexcept {
            this->initialized = false;
            this->window = Window<W>(win_width, win_height, win_title);
            this->VBO = nullptr;
            this->VAO = nullptr;
            this->program = nullptr;

            this->Initialize();
        }

        /**
         * @brief Move constructor.
         * 
         * Moves varaibles from other into this.
         * 
         * @param other A different context whose data will be moved into this.
         */
        GraphicsAPIContext(GraphicsAPIContext<W, I>&& other) noexcept {
            this->initialized = other.initialized;
            this->window = std::move(other.window);
            this->VBO = std::move(other.VBO);
            this->VAO = std::move(other.VAO);
            this->program = std::move(other.program);

            other.initialized = false;
            other.window = nullptr;
            other.VBO = nullptr;
            other.VAO = nullptr;
            other.program = nullptr;
        }

        /**
         * @brief Copies a VBO.
         * 
         * Copies an existing VBO into this.
         * 
         * @param VBO An existing VBO.
         */
        void CreateVBO(Buffer<I>& VBO) {
            this->VBO = VBO.Copy();
        }

        /**
         * @brief Copies a VAO.
         * 
         * Copies an existing VAO into this.
         * 
         * @param VAO An existing VAO.
         */
        void CreateVAO(VertexArray<I>& VAO) {
            this->VAO = VAO.Copy();
        }

        /**
         * @brief Copies a program.
         * 
         * Copies an existing shader program into this.
         * 
         * @param program An existing shader program.
         */
        void CreateProgram(ShaderProgram<I>& program) {
            this->program = program.Copy();
        }

        /**
         * @brief Initializes graphics API and window.
         * 
         * A pure virtual function whose implementation should initialize the graphics API and window.
         */
        virtual void Initialize() = 0;

        /**
         * @brief Cleans up graphics API.
         * 
         * A pure virtual function whose implementation should clean up the graphics API.
         */
        virtual void Uninitialize() = 0;

        /**
         * @brief Copies data.
         * 
         * A pure virtual function whose implementation should set data from args.
         * 
         * @param win_width The width of the window.
         * @param win_height The height of the window.
         * @param win_title The title of the window.
         */
        virtual void Clone(size_t win_width, size_t win_height, const char* win_title) = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should handle clean up data - this includes calling Unitialize().
         */
        virtual ~GraphicsAPIContext() {return;};

        /**
         * @brief Move operator.
         * 
         * Moves data from another context into this.
         * 
         * @param other A different context whose data will be moved into this.
         * @return The context with newly moved data.
         */
        GraphicsAPIContext<W, I>& operator=(GraphicsAPIContext<W, I>&& other) noexcept {
            if (this == &other) return *this;
            
            this->Unitialize();

            this->initialized = other.initialized;
            this->window = std::move(other.window);
            this->VBO = std::move(other.VBO);
            this->VAO = std::move(other.VAO);
            this->program = std::move(other.program);

            other.initialized = false;
            other.window = nullptr;
            other.VBO = nullptr;
            other.VAO = nullptr;
            other.program = nullptr;

            return *this;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 contexts are equal.
         * 
         * @param other A different context.
         * @return True if they are equal, otherwise false.
         */
        bool operator==(GraphicsAPIContext<W, I>& other) const noexcept {
            return this->initialized == other.initialized && this->window == other.window && this->VBO == other.VBO && this->VAO == other.VAO && this->program == other.program;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 contexts are not equal.
         * 
         * @param other A different context.
         * @return True if they are not equal, otherwise false.
         */
        bool operator!=(GraphicsAPIContext<W, I>& other) const noexcept {
            return !(*this == other);
        }
};

/**
 * @class OpenGLShader
 * @brief OpenGL representation of shaders.
 * 
 * Inherits from generic Shader class, specifying to OpenGL.
 */
class OpenGLShader : public Shader<GLuint> {
    protected:
        /**
         * @brief Helper function to convert enum to OpenGL.
         * 
         * Converts the ShaderType enum to OpenGL GLenum values.
         * 
         * @return The type of shader in GLenum form.
         */
        GLenum GetType() const;

    public:
        /**
         * @brief Default constructor.
         * 
         * @see Shader::Shader
         * 
         * Calls Shader default constructor.
         */
        OpenGLShader() noexcept;

        /**
         * @brief Copy constructor.
         * 
         * @see Shader::Shader
         * 
         * Calls Shader copy constructor.
         */
        OpenGLShader(const OpenGLShader& other) noexcept;

        /**
         * @brief Move constructor.
         * 
         * @see Shader::Shader
         * 
         * Calls Shader move constructor.
         */
        OpenGLShader(OpenGLShader&& other) noexcept;

        /**
         * @brief Destructor.
         * 
         * Cleans up shader data.
         */
        ~OpenGLShader() override;

        /**
         * @brief Creates OpenGL shader from member variables.
         * 
         * Reads member variables and uses OpenGL to create a shader.
         */
        void CreateShader() override;

        /**
         * @brief Destroyes OpenGL shader.
         * 
         * Reads OpenGL shader and destroys it.
         */
        void DestroyShader() override;

        /**
         * @brief Sets member variables from args.
         * 
         * Reads arguments and initializes a shader from them.
         * 
         * @param type The type of shader to make.
         * @param path The path of source code to read.
         */
        void Clone(ShaderType type, const char* path) override;

        /**
         * @brief Sets member variables from args.
         * 
         * Reads arguments and initializes a shader from them.
         * 
         * @param type The type of shader to make.
         * @param other A graphics API representation of the shader.
         */
        void Clone(ShaderType type, GLuint& other) override;

        /**
         * @brief Creates an empty shader.
         * 
         * Makes a new unique pointer to an empty shader.
         * 
         * @return A unique pointer to a new empty shader.
         */
        std::unique_ptr<Shader<GLuint>> CopyEmpty() override;

        /**
         * @brief Creates a copy of this.
         * 
         * Makes a new unique pointer to a copy of this.
         * 
         * @return A unique pointer to a new copy of this.
         */
        std::unique_ptr<Shader<GLuint>> Copy() override;
};

/**
 * @class OpenGLBuffer
 * @brief OpenGL representation of buffers.
 * 
 * Inherited from generic Buffer class, to specify OpenGL.
 */
class OpenGLBuffer : public Buffer<GLuint> {
    protected:
        /**
         * @brief Converts generic BufferType enum into GLenum.
         * 
         * Converts generic enum of BufferType into specific OpenGL GLenum.
         * 
         * @return The GLenum of the BufferType.
         */
        GLenum GetType() const;

        /**
         * @brief Converts generic BufferUsageType enum into GLenum.
         * 
         * Converts generic enum of BufferType into specific OpenGL GLenum.
         * 
         * @param type The BufferUsageType to convert.
         * @return The GLenum of the BufferUsageType.
         */
        GLenum GetUsageType(BufferUsageType type) const;

        /**
         * @brief Gets default Read/Write permissions from type.
         * 
         * Converts buffer type into default permissions of Read/Write.
         */
        void GetReadWrite();

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values to 0.
         */
        OpenGLBuffer() noexcept;

        /**
         * @brief Copy constructor.
         * 
         * Copies data from another buffer.
         * 
         * @param other A different OpenGLBuffer to copy data from.
         */
        OpenGLBuffer(const OpenGLBuffer& other) noexcept;

        /**
         * @brief Move constructor.
         * 
         * Moves data from another buffer.
         * 
         * @param other A different OpenGLBuffer to move data from.
         */
        OpenGLBuffer(OpenGLBuffer&& other) noexcept;

        /**
         * @brief Destructor.
         * 
         * Cleans up OpenGL data.
         */
        ~OpenGLBuffer() override;

        /**
         * @brief Creates buffer in OpenGL.
         * 
         * Calls OpenGL to create a buffer based off member variables.
         */
        void CreateBuffer() override;

        /**
         * @brief Destroyes buffer in OpenGL.
         * 
         * Calls OpenGL to destroy a buffer.
         */
        void DestroyBuffer() override;

        /**
         * @brief Initializes member variables from args.
         * 
         * Copies args and initializes this from them.
         * 
         * @param type The type of buffer to make this.
         * @param binding The binding of the buffer in the shaders.
         */
        void Clone(BufferType type, size_t binding) override;

        /**
         * @brief Initializes member variables from args.
         * 
         * Copies args and initializes this from them.
         * 
         * @param type The type of buffer to make this.
         * @param other The OpenGL representation of a buffer.
         */
        void Clone(BufferType type, GLuint& other) override;

        /**
         * @brief Creates an empty copy of a buffer.
         * 
         * Constructs a new default buffer.
         * 
         * @return A unique pointer to a new empty buffer.
         */
        std::unique_ptr<Buffer<GLuint>> CopyEmpty() override;

        /**
         * @brief Creates a copy of this.
         * 
         * Constructs a new buffer with a copy of this.
         * 
         * @return A unique pointer to a new copy of this.
         */
        std::unique_ptr<Buffer<GLuint>> Copy() override;

        /**
         * @brief Binds buffer in OpenGL.
         * 
         * Calls OpenGL to bind the buffer.
         */
        void Bind() const override;

        /**
         * @brief Unbinds buffer in OpenGL.
         * 
         * Calls OpenGL to unbind the buffer.
         */
        void Unbind() const override;

        /**
         * @brief Puts data into a buffer.
         * 
         * Binds the buffer, before putting size bytes of data into it.
         * 
         * @param size The amount of data in bytes to put in.
         * @param data The data to put in.
         * @param usage The usage type of data.
         */
        void SetData(size_t size, const void* data, BufferUsageType usage) override;

        /**
         * @brief Updates a subset of data in a buffer.
         * 
         * Binds the buffer, before updating a subset of data.
         * 
         * @param offset The offset to the start of data to update.
         * @param size The amount of data in bytes to update.
         * @param data The data to put in.
         */
        void Update(size_t offset, size_t size, const void* data) override;

        /**
         * @brief Binds a buffer to a binding in the shader.
         * @warning Only works on UBO and SSBO
         * 
         * Binds the buffer to a specific binding in shaders.
         * 
         * @param binding The binding to put the buffer on in the shader.
         */
        void BindBase(size_t binding) override;
};

/**
 * @class OpenGLVertexArray
 * @brief OpenGL specified version of generic Vertex Array.
 * 
 * Inherits from generic Vertex Array, specifying to OpenGL.
 */
class OpenGLVertexArray : public VertexArray<GLuint> {
    protected:
        /**
         * @brief Converts generic Vertex Array Attrib Type into GLenum.
         * 
         * Converts generic enum into specific OpenGL GLenum.
         * 
         * @param type The Vertex Array Attrib Type to convert.
         * @return The specific GLenum value.
         */
        GLenum GetVertexArrayAttribType(VertexArrayAttribType type) const;

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values to 0.
         */
        OpenGLVertexArray() noexcept;

        /**
         * @brief Copy constructor.
         * 
         * Copies values from another Vertex Array into this.
         * 
         * @param other A different Vertex Array to copy into this.
         */
        OpenGLVertexArray(const OpenGLVertexArray& other) noexcept;

        /**
         * @brief Move constructor.
         * 
         * Moves values from another Vertex Array into this.
         * 
         * @param other A different Vertex Array to move into this.
         */
        OpenGLVertexArray(OpenGLVertexArray&& other) noexcept;

        /**
         * @brief Destructor.
         * 
         * Cleans up Vertex Array in OpenGL.
         */
        ~OpenGLVertexArray() override;

        /**
         * @brief Creates a Vertex Array in OpenGL.
         * 
         * Calls OpenGL to create a Vertex Array.
         */
        void CreateVertexArray() override;

        /**
         * @brief Destroys a Vertex Array in OpenGL.
         * 
         * Calls OpenGL to destroy a Vertex Array.
         */
        void DestroyVertexArray() override;

        /**
         * @brief Copies values from another Vertex Array into this.
         * 
         * Takes an OpenGL raw Vertex Array and copies it into this.
         * 
         * @param other A raw OpenGL Vertex Array to copy.
         */
        void Clone(GLuint& other) override;

        /**
         * @brief Creates an empty copy of a Vertex Array.
         * 
         * Constructs a new empty Vertex Array.
         * 
         * @return A unique pointer to a new empty Vertex Array.
         */
        std::unique_ptr<VertexArray<GLuint>> CopyEmpty() override;

        /**
         * @brief Creates a new copy of this.
         * 
         * Constructs a new Vertex Array with the data of this.
         * 
         * @return A unique pointer to a new Vertex Array with the data of this.
         */
        std::unique_ptr<VertexArray<GLuint>> Copy() override;

        /**
         * @brief Binds the Vertex Array.
         * 
         * Calls OpenGL to bind the Vertex Array.
         */
        void Bind() const override;

        /**
         * @brief Unbinds the Vertex Array.
         * 
         * Calls OpenGL to unbind the Vertex Array.
         */
        void Unbind() const override;

        /**
         * @brief Adds an attribute into the Vertex Array.
         * 
         * Writes new data into the Vertex Array.
         * 
         * @param index The index of the vertex attribute to be modified.
         * @param size The number of components per vertex attribute.
         * @param type The type of data of each component.
         * @param normalised If fixed point data values should be normalised.
         * @param stride The byte offset between consecutive vertex attributes.
         * @param offset The offset of the first component of the first vertex attribute.
         */
        void AddAttribute(size_t index, size_t size, VertexArrayAttribType type, bool normalised, size_t stride, const void* offset) override;
};

/**
 * @class OpenGLShaderProgram
 * @brief OpenGL specific version of ShaderProgram.
 * 
 * Inherited from the ShaderProgram and specifies for OpenGL.
 */
class OpenGLShaderProgram : public ShaderProgram<GLuint> {
    protected:
        /**
         * @brief Creates and compiles a shader.
         * 
         * Constructs a new compiled shader from args.
         * 
         * @param type The type of shader to compile.
         * @param path The path to the source code.
         * @return A unique pointer to the new compiled shader.
         */
        std::unique_ptr<Shader<GLuint>> Compile(ShaderType type, const char* path) override;
    
        /**
         * @brief Gets the location of a uniform.
         * 
         * Checks cache for uniform, if it is not there OpenGL is called and location is added to cache.
         * 
         * @param name The name of the uniform being searched for.
         * @return The location of the uniform.
         */
        size_t GetUniformLocation(const char* name) override;

        /**
         * @brief Converts a draw mode into OpenGL enum.
         * 
         * Converts a generic Draw Mode enum into a specific OpenGL GLenum.
         * 
         * @param mode The generic draw mode to convert into OpenGL.
         * @return The specific OpenGL value.
         */
        GLenum GetMode(DrawMode mode);

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values to 0.
         */
        OpenGLShaderProgram() noexcept;

        /**
         * @brief Constructor.
         * 
         * Initializes values from args.
         * 
         * @param purpose The purpose of the shader program.
         */
        OpenGLShaderProgram(ShaderProgramPurpose purpose) noexcept;

        /**
         * @brief Move constructor.
         * 
         * Moves data from another program into this.
         * 
         * @param other A different program whose data will be moved into this.
         */
        OpenGLShaderProgram(OpenGLShaderProgram&& other) noexcept;

        /**
         * @brief Destructor.
         * 
         * Cleans up OpenGL data.
         */
        ~OpenGLShaderProgram() override;

        /**
         * @brief Creates a shader program.
         * 
         * Reads member variables and initialzies a shader program from them.
         */
        void CreateShaderProgram() override;

        /**
         * @brief Destroyes a shader program.
         * 
         * Calls OpenGL to destroy the current program.
         */
        void DestroyShaderProgram() override;

        /**
         * @brief Creates an empty new program.
         * 
         * Constructs a new program filled with empty data.
         * 
         * @return A unique pointer to a new empty program.
         */
        std::unique_ptr<ShaderProgram<GLuint>> CopyEmpty() override;

        /**
         * @brief Attaches a shader to the program.
         * 
         * Constructs a shader and attaches it to the program.
         * 
         * @param type The type of shader to attach.
         * @param path The path to source code of the shader.
         */
        void AttachFromFile(ShaderType type, const char* path) override;

        /**
         * @brief Links all attached shaders together.
         * 
         * Calls OpenGL to link shaders and program together.
         */
        void Link() override;

        /**
         * @brief Sets the program as currently used.
         * 
         * Calls OpenGL to use the program.
         */
        void Use() const override;

        /**
         * @brief Sets the program as unused.
         * 
         * Calls OpenGL to stop using the program.
         */
        void Unuse() override;

        /**
         * @brief Sets an int uniform.
         * 
         * Sets an integer into a uniform value.
         * 
         * @param name The name of the uniform.
         * @param value The integer value.
         */
        void SetInt(const char* name, int value) override;

        /**
         * @brief Sets a float uniform.
         * 
         * Sets a float into a uniform value.
         * 
         * @param name The name of the uniform.
         * @param value The float value.
         */
        void SetFloat(const char* name, float value) override;

        /**
         * @brief Sets a vector uniform.
         * 
         * Sets a vector into a uniform value.
         * 
         * @param name The name of the uniform.
         * @param value The vector value.
         */
        void SetVec(const char* name, Vector<float>& value) override;

        /**
         * @brief Sets a matrix uniform.
         * 
         * Sets a matrix into a uniform value.
         * 
         * @param name The name of the uniform.
         * @param value The matrix value.
         */
        void SetMat(const char* name, Matrix<float>& value) override;

        /**
         * @brief Binds the UBO to a block and binding.
         * 
         * Calls OpenGL to bind the UBO in the shader.
         * 
         * @param block The block to bind the UBO to.
         * @param binding The binding to bind the UBO to.
         */
        void BindUBO(const char* block, size_t binding) override;

        /**
         * @brief Binds the SSBO to a block and binding.
         * 
         * Calls OpenGL to bind the SSBO in the shader.
         * 
         * @param block The block to bind the SSBO to.
         * @param binding The binding to bind the SSBO to.
         */
        void BindSSBO(const char* block, size_t binding) override;

        /**
         * @brief Dispatches a compute onto the GPU.
         * 
         * Calls OpenGL to dispatch a compute for the GPU to process.
         * 
         * @param x The x size.
         * @param y The y size.
         * @param z The z size.
         */
        void Dispatch(size_t x, size_t y, size_t z) override;

        /**
         * @brief Waits for a compute to complete.
         * 
         * Calls OpenGL to wait for a compute to finish, then returns.
         */
        void WaitDispatchComplete() override;

        /**
         * @brief Gets the result from the GPU after a compute.
         * 
         * Gets result back from the GPU via OpenGL.
         * 
         * @return A void* to the data from the GPU.
         */
        void* GetDispatchResult() override;

        /**
         * @brief Draws vertices to the screen
         * 
         * Calls OpenGL to call shaders and draw vertices.
         * 
         * @param VAO The VAO with vertex information.
         * @param mode The draw mode to use.
         * @param starts A vector of when each shape starts.
         * @param sizes A vector of how many vertices each shape has.
         */
        void Draw(VertexArray<GLuint>& VAO, DrawMode mode, std::vector<size_t> starts, std::vector<size_t> sizes) override;
};

/**
 * @class OpenGLWindow
 * @brief OpenGL representation of generic window.
 * 
 * Inherits from Window class, specifying to OpenGL.
 */
class OpenGLWindow : public Window<GLFWwindow> {
    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes all values to 0.
         */
        OpenGLWindow() noexcept;

        /**
         * @brief Constructor.
         * 
         * Initializes member variables from args.
         * 
         * @param width The width of the window in pixels.
         * @param height The height of the window in pixels.
         * @param title The title of the window.
         */
        OpenGLWindow(size_t width, size_t height, const char* title) noexcept;

        /**
         * @brief Move constructor.
         * 
         * Moves data from another window into this.
         * 
         * @param other A different window whose data will be moved into this.
         */
        OpenGLWindow(OpenGLWindow&& other) noexcept;

        /**
         * @brief Destructor.
         * 
         * Cleans up data.
         */
        ~OpenGLWindow() override;

        /**
         * @brief Creates a window from member variables.
         * 
         * Creates a new window from GLFW.
         */
        void CreateWindow() override;

        /**
         * @brief Destroys a window.
         * 
         * Calls GLFW to destroy a window.
         */
        void DestroyWindow() override;

        /**
         * @brief Sets member variables from args.
         * 
         * Initializes a window from args.
         * 
         * @param width The width of the window.
         * @param height The height of the window.
         * @param title The title of the window.
         */
        void Clone(size_t width, size_t height, const char* title) override;

        /**
         * @brief Initilizes a window from GLFW window.
         * 
         * Gets data from another window in GLFW.
         * 
         * @param other GLFW representation of a window.
         */
        void Clone(GLFWwindow* other) override;

        /**
         * @brief Creates a new empty window.
         * 
         * Creates a new window filled with empty data.
         * 
         * @return A unique pointer to a new empty window.
         */
        std::unique_ptr<Window<GLFWwindow>> CopyEmpty() override;

        /**
         * @brief Checks if a window should close.
         * 
         * Calls GLFW to check if a window should close this frame.
         * 
         * @return True if the window should close, otherwise false.
         */
        bool ShouldClose() override;

        /**
         * @brief Set all data to 0.
         * 
         * Clears all data in the window to 0.
         */
        void ClearData() override;

        /**
         * @brief Displays what is drawn on the window.
         * 
         * Calls GLFW to display what is currently drawn.
         */
        void Display() override;
};

class OpenGLContext : GraphicsAPIContext<GLFWwindow, GLuint> {
    public:
        /**
         * @brief Default constructor.
         * 
         * Sets data to 0 and initializes OpenGL.
         */
        OpenGLContext() noexcept;

        /**
         * @brief Destructor.
         * 
         * Clears data and uninitializes OpenGL.
         */
        ~OpenGLContext() override;

        /**
         * @brief Initializes OpenGL.
         * 
         * Sets up OpenGL for use.
         */
        void Initialize() override;

        /**
         * @brief Uninitializes OpenGL.
         * 
         * Cleans up OpenGL data after use.
         */
        void Uninitialize() override;

        /**
         * @brief Initializes data from args.
         * 
         * Sets member variables from arguments.
         * 
         * @param win_width The width of the current window.
         * @param win_height The height of the current window.
         * @param win_title The title of the current window.
         */
        void Clone(size_t win_width, size_t win_height, const char* win_title) override;

};

/**
 * @class Vector
 * @brief Represents a vector of any size.
 * 
 * Vector class - physics vector not container - that supports any number of components.
 */
template <typename T>
class Vector {
    protected:
        // Amount of components
        size_t size;
        // Dynamic component array
        T* data;

    protected:
        /**
         * @brief Checks if 2 vectors have equal size.
         * 
         * Compares 2 vectors for equal sizes.
         * 
         * @param other A const reference to another vector to check against this.
         * @return True if sizes are equal, otherwise false.
         */
        bool CheckSize(const Vector<T>& other) const noexcept {
            return this->size == other.size;
        }

    public:
        /**
         * @brief Constructor.
         * 
         * Initializes dynamic array from std::vector.
         * 
         * @param data An std::vector of the components we want.
         */
        Vector(std::vector<T> data) noexcept {
            this->size = data.size();

            this->data = new T[this->size];
            std::copy(data.begin(), data.end(), this->data);
        }

        /**
         * @brief Constructor.
         * 
         * Intializes dynamic array from initializer list.
         * 
         * @param data An std::initializer_list of the components we want.
         */
        Vector(std::initializer_list<T> data) {
            this->size = data.size();

            this->data = new T[this->size];
            std::copy(data.begin(), data.end(), this->data);
        }

        /**
         * @brief Default constructor.
         * 
         * Initializes items to 0.
         */
        Vector() noexcept {
            this->size = 0;
            // Make sure we do not accidentally delete nullptr.
            this->data = new T[1]{};
        }

        /**
         * @brief Copy constructor.
         * 
         * Deep copies data from another vector.
         * 
         * @param other Another vector whose data will be copied.
         */
        Vector(const Vector<T>& other) noexcept {
            this->size = other.size;
            this->data = new T[this->size];
            std::copy(other.data, other.data + other.size, this->data);
        }

        /**
         * @brief Move constructor.
         * 
         * Moves data from another vector to create a new one.
         * 
         * @param other Another vector whose data will be moved.
         */
        Vector(Vector<T>&& other) noexcept {
            this->size = other.size;
            this->data = new T[this->size];
            std::copy(other.data, other.data + other.size, this->data);

            other.size = 0;
            delete[] other.data;
            other.data = nullptr;
        }

        /**
         * @brief Prints all values in vector.
         * 
         * Prints the values in vector, with the format of leaving a space inbetween each element.
         */
        void Print() const noexcept {
            for (size_t i = 0; i < size; i++) {
                std::cout << i << " ";
            }
            std::cout << "\n";
        }

        /**
         * @brief Helper function to get size.
         * @warning Is not mathemetical, see Length() for the mathematical length of vector.
         * 
         * Gets the amount of components.
         * 
         * @return Amount of components in vector.
         */
        size_t Size() const noexcept {
            return size;
        }

        /**
         * @class Iterator
         * @brief Iterator for vector.
         * 
         * Provides iteration access for vector class.
         */
        class Iterator {
            protected:
                // Pointer to owner
                Vector<T>* vec;
                // Current index
                size_t idx;

            public:
                /**
                 * @brief Constructor.
                 * 
                 * Initializes values in iterator.
                 * 
                 * @param vec A pointer to the owner of the iterator.
                 * @param index The start index of the iterator.
                 */
                Iterator(Vector<T>* vec, size_t index) noexcept {
                    this->vec = vec;
                    this->idx = index;
                }

                /**
                 * @brief Dereference operator.
                 * 
                 * Gets the value of the item at the current index.
                 * 
                 * @return The item at the current index.
                 */
                const T& operator*() const {
                    return (*vec)[idx];
                }

                /**
                 * @brief Arrow operator.
                 * 
                 * Gets a pointer to the item at the current index.
                 * 
                 * @return A pointer to the item at the current index.
                 */
                T* operator->() const {
                    return &vec[idx];
                }

                /**
                 * @brief Increment operator.
                 * 
                 * Increases index by 1.
                 * 
                 * @return This after index is increased by 1.
                 */
                Iterator& operator++() noexcept {
                    idx = idx + 1;
                    return *this;
                }

                /**
                 * @brief Post-increment operator.
                 * 
                 * Increases index by 1.
                 * 
                 * @return This before index is increased by 1.
                 */
                Iterator operator++(int) noexcept {
                    Iterator tmp = *this;
                    ++(*this);
                    return tmp;
                }

                /**
                 * @brief Equality operator.
                 * 
                 * Checks if 2 iterators are equal.
                 * 
                 * @param other A different iterator to compare to this.
                 * @return True if they are equal, otherwise false.
                 */
                bool operator==(const Iterator& other) const noexcept {
                    return vec == other.vec && idx == other.idx;
                }

                /**
                 * @brief Inequality operator.
                 * 
                 * Checks if 2 iterators are not equal.
                 * 
                 * @param other A different iterator to compare to this.
                 * @return True if they are not equal, otherwise false.
                 */
                bool operator!=(const Iterator& other) const noexcept {
                    return !(*this == other);
                }
        };

        /**
         * @brief Gets an iterator to the beginning of components.
         * 
         * Returns an iterator to the beginning of components.
         * 
         * @return An iterator to the beginning of components.
         */
        Iterator Begin() noexcept {
            return Iterator(this, 0);
        }

        /**
         * @brief Gets an iterator to the end of components.
         * 
         * Returns an iterator to the end of components.
         * 
         * @return An iterator to the end of components.
         */
        Iterator End() noexcept {
            return Iterator(this, size);
        }

        /**
         * @brief Gets an element at an index.
         * 
         * Checks for out of bounds index, before retrieving element at index.
         * 
         * @param index The index to get element at.
         * @return The element at the requested index.
         */
        T Get(size_t index) const {
            if (index > size) {
                throw std::out_of_range("Index out of range");
            }
            return data[index];
        }

        /**
         * @brief Sets an element at an index.
         * 
         * Checks for out of bounds index, before setting element at index.
         * 
         * @param index The index to set element at.
         * @param data The data to set at the element.
         */
        void Set(size_t index, T& data) {
            if (index > size) {
                throw std::out_of_range("Index out of range");
            }
            data[index] = data;
        }

        /**
         * @brief Addition operator.
         * 
         * Adds each element in 2 vectors together.
         * 
         * @param other Another vector to add to this.
         * @return A new vector with the added values.
         */
        Vector<T>& operator+(const Vector<T>& other) const {
            if (!CheckSize(other)) {
                throw std::runtime_error("Sizes do not match");
            }

            Vector<T> new_vec(size);

            for (size_t i = 0; i < size; i++) {
                new_vec.Set(i, data[i] + other.data[i]);
            }

            return new_vec;
        }

        /**
         * @brief Subtraction operator.
         * 
         * Subtracts each element in 2 vectors together.
         * 
         * @param other Another vector to subtract from this.
         * @return A new vector with the subtracted values.
         */
        Vector<T>& operator-(const Vector<T>& other) const {
            if (!CheckSize(other)) {
                throw std::runtime_error("Sizes do not match");
            }

            Vector<T> new_vec(size);

            for (size_t i = 0; i < size; i++) {
                new_vec.Set(i, data[i] - other.data[i]);
            }

            return new_vec;
        }

        /**
         * @brief Dot product.
         * 
         * Multiplies each element in 2 vectors together and sums them.
         * 
         * @param other Another vector to use in the dot product.
         * @return The sum of multiplied values.
         */
        T& operator*(const Vector<T>& other) const {
            if (!CheckSize(other)) {
                throw std::runtime_error("Sizes do not match");
            }

            T sum = T();

            for (size_t i = 0; i < size; i++) {
                sum += data[i] * other.data[i];
            }

            return sum;
        }

        /**
         * @brief Scalar multiplication.
         * 
         * Multiplies each element by a constant scalar.
         * 
         * @param scalar The scalar to multiply each element by.
         * @return The vector with the newly multiplied components.
         */
        Vector<T>& operator*(const double scalar) const noexcept {
            Vector<T> new_vec(size);

            for (size_t i = 0; i < size; i++) {
                new_vec.Set(i, data[i] * scalar);
            }

            return new_vec;
        }

        /**
         * @brief Scalar division.
         * 
         * Divides each element by a constant scalar.
         * 
         * @param scalar The scalar to divide each element by.
         * @return The vector with the newly divided components.
         */
        Vector<T>& operator/(const double scalar) const {
            if (scalar == 0.0) {
                throw std::invalid_argument("Cannot divide by 0");
            }

            Vector<T> new_vec(size);

            for (size_t i = 0; i < size; i++) {
                new_vec.Set(i, data[i] / scalar);
            }

            return new_vec;
        }

        /**
         * @brief Negative operator.
         * 
         * Makes each element in the vector negative.
         * 
         * @return The vector with each element negative.
         */
        Vector<T>& operator-() const noexcept {
            Vector<T> new_vec(size);

            for (size_t i = 0; i < size; i++) {
                new_vec.Set(i, -data[i]);
            }

            return new_vec;
        }

        /**
         * @brief Equality operator.
         * 
         * Checks if 2 vectors are equal.
         * 
         * @param other A different vector to compare against this.
         * @return True if they are equal, otherwise false.
         */
        bool operator==(const Vector<T>& other) const noexcept {
            if (this->size != other.size) {
                return false;
            }
            for (size_t i = 0; i < size; i++) {
                if (data[i] != other.data[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Inequality operator.
         * 
         * Checks if 2 vectors are not equal.
         * 
         * @param other A different vector to compare against this.
         * @return True if they are not equal, otherwise false.
         */
        bool operator!=(const Vector<T>& other) const noexcept {
            return !(*this == other);
        }

        /**
         * @brief Gets the squared length of the vector.
         * @warning Does not get amount of components, but instead mathematical length.
         * 
         * @see Length()
         * 
         * Gets the squared mathematical length of the vector, by squaring each element and summing them.
         * 
         * @return The squared length of the vector.
         */
        size_t LengthSquared() const noexcept {
            size_t sum = 0;
            for (size_t i = 0; i < size; i++) {
                sum += data[i] * data[i];
            }
            return sum;
        }

        /**
         * @brief Gets the length of the vector.
         * @warning Does not get amount of components, but instead mathematical length.
         * 
         * @see LengthSquared()
         * 
         * Gets the mathematical length of the vector, by square rooting the squared length.
         * 
         * @return The mathematical length of the vector.
         */
        size_t Length() const noexcept {
            return sqrt(LengthSquared());
        }

        /**
         * @brief Gets the normalised vector.
         * 
         * Divides each element of the vector by its length, to get the normalised vector.
         * 
         * @return The normalised vector.
         */
        Vector<T>& Normalised() const noexcept {
            size_t length = Length();
            if (length == 0) {
                Vector<T> new_vec(size);
                for (size_t i = 0; i < size; i++) {
                    new_vec.data[i] = 0;
                }
                return new_vec;
            }
            return *this / length;
        }

        /**
         * @brief Gets the cross product of 2 vectors.
         * 
         * Gets the cross product of 2 vectors in 3D space.
         * 
         * @param other A different 3D vector to get the cross product with.
         * @return The cross product of this and other.
         */
        Vector<T>& Cross(const Vector<T>& other) const {
            if (size != 3 || other.size != 3) {
                throw std::invalid_argument("Cross product only works with 3 components");
            }

            T a1 = data[0]; T a2 = data[1]; T a3 = data[2];
            T b1 = other.data[0]; T b2 = other.data[1]; T b3 = other.data[3];

            Vector<T> new_vec(size);
            new_vec.data[0] = a2 * b3 - a3 * b2;
            new_vec.data[1] = a3 * b1 - a1 * b3;
            new_vec.data[2] = a1 * b2 - a2 * b1;

            return new_vec;
        }

        /**
         * @brief Gets the distance between 2 vectors.
         * 
         * Subtracts other from this to get the difference, then finds the length of that.
         * 
         * @param other A different vector to find the distance between.
         * @return The distance between this and other.
         */
        size_t Distance(const Vector<T>& other) const {
            return (*this - other).Length();
        }

        /**
         * @brief Blends between 2 vectors based of t.
         * 
         * Uses the lerp formula to blend between 2 vectors with t.
         * 
         * @param other The target vector this is being blended towards.
         * @param t The time values for the lerp formula.
         * @return The current lerped vector.
         */
        Vector<T>& Lerp(const Vector<T>& other, double t) const {
            if (!CheckSize(other)) {
                throw std::invalid_argument("Sizes must match");
            }
            return *this + (other - *this) * t;
        }

        /**
         * @brief Copy operator.
         * 
         * Copies data from another vector into this.
         * 
         * @param other A different vector whose data will be copied.
         * @return The vector with newly copied data.
         */
        Vector<T>& operator=(const Vector<T>& other) noexcept {
            if (this == &other) return *this;

            this->size = other.size;
            delete[] this->data;
            this->data = new T[size];
            std::copy(other.data, other.data + size, this.data);

            return *this;
        }

        /**
         * @brief Move operator.
         * 
         * Moves data from another vector into this.
         * 
         * @param other A different vector whose data will be moved.
         * @return The vector with newly moved data.
         */
        Vector<T>& operator=(Vector<T>&& other) noexcept {
            if (this == &other) return *this;

            this->size = other.size;
            delete[] this->data;
            this->data = new T[size];
            std::copy(other.data, other.data + size, this.data);

            other.size = 0;
            delete[] other.data;
            other.data = nullptr;

            return *this;
        }

        /**
         * @brief Get operator.
         * @warning Bounds checking is not performed, so indexes must be correct.
         * 
         * Gets data at index, does not perform bounds checking.
         * 
         * @param index The index to retrieve item at.
         * @return A reference to the item at index.
         */
        T& operator[](const size_t index) {
            return data[index];
        }

        /**
         * @brief Const get operator.
         * @warning Bounds checking is not performed, so indexes must be correct.
         * 
         * Gets data at index, does not perform bounds checking.
         * 
         * @param index The index to retrieve item at.
         * @return A const reference to the item at index.
         */
        const T& operator[](const size_t index) const {
            return data[index];
        }
};

/**
 * @class Matrix
 * @brief A class to represent 2D matrices.
 * 
 * A class that is a 2D matrix, but a 1D matrix is used and indices are converted. Supports addition, subtraction, multiplication and scalar multiplication currently.
 */
template <typename T>
class Matrix {
    protected:
        // Size of 2D array.
        size_t rows;
        size_t cols;
        // 1D matrix allocated using new/delete.
        T* data;

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
        size_t GetIndex(size_t row, size_t col) const {
            return row * cols + col;
        }

        /**
         * @brief Checks if 2 matrices have the same dimensions.
         * 
         * This checks if cols and rows are exactly the same between 2 matrices.
         * 
         * @param other The other matrix to compare sizes with this.
         * @return True if the sizes are equal, False if they are different.
         */
        bool CheckSize(const Matrix<T>& other) const {
            return rows == other.rows && cols == other.cols;
        }
    
    public:
        /**
         * @brief Constructor that creates data.
         * 
         * This dynamically allocates the data array, of size cols * rows, and initialises it to be all 0.
         * 
         * @param cols The cols of the 2D array.
         * @param rows The rows of the 2D array.
         */
        Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
            data = new T[rows * cols]{};
        }

        /**
         * @brief Copy constructor
         * 
         * This copies data - deep copy - from another matrix and creates a new matrix with the same data.
         * 
         * @param other A reference to a different matrix to copy into this.
         */
        Matrix(Matrix& other) {
            this->rows = other.rows;
            this->cols = other.cols;
            data = new T[rows * cols];
            std::copy(other.data, other.data + rows * cols, data);
        }

        /**
         * @brief Copy constructor
         * 
         * This copies data - deep copy - from another matrix and creates a new matrix with the same data.
         * 
         * @param other A const reference to a different matrix to copy into this.
         */
        Matrix(const Matrix<T>& other) : rows(other.rows), cols(other.cols) {
            data = new T[rows * cols];
            std::copy(other.data, other.data + rows * cols, data);
        }

        /**
         * @brief Default constructor
         * 
         * This sets rows and cols to 0, and data to nullptr. Default values for all members.
         */
        Matrix() : rows(0), cols(0), data(nullptr) {}

        /**
         * @brief Move constructor for matrices.
         * 
         * Sets data in new matrix to data in other matrix with a deep copy, and destroys the old matrix.
         * 
         * @param other A different matrix to be destroyed and have its data moved into this.
         */
        Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols) {
            this->data = new T[rows * cols];
            std::copy(other.data, other.data + rows * cols, this->data);

            delete other.data;
            other.data = nullptr;
            other.rows = 0;
            other.cols = 0;
        }

        /**
         * @brief Destructor that destroyes class.
         * 
         * This deallocates the data array with delete.
         */
        ~Matrix() {
            delete[] data;
        }

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
        double Get(size_t row, size_t col) const {
            if (row >= rows || col >= cols) {
                throw std::out_of_range("Index out of range");
            }

            return data[GetIndex(row, col)];
        }

        /**
         * @brief Sets data in array at an index.
         * 
         * This maps the 2D indices to a 1D index and sets the data at the requested index.
         * 
         * @param row The row to set the data at.
         * @param col The col to set the data at.
         * @param data The data to set at the requested index.
         */
        void Set(size_t row, size_t col, double data) {
            if (row >= rows || col >= cols)
                throw std::out_of_range("Index out of range");

            this->data[GetIndex(row, col)] = data;
        }

        /**
         * @brief Performs the transposition of the current matrix. Does not modify data.
         * 
         * This swaps the rows and cols of the matrix and returns the result. Data is not modified as a new matrix is returned.
         * 
         * @return The newly created matrix with the transposed data.
         */
        Matrix Transpose() const {
            Matrix result(cols, rows);

            for (size_t r = 0; r < rows; r++)
                for (size_t c = 0; c < cols; c++)
                    result.Set(c, r, Get(r,c));

            return result;
        }

        /**
         * @brief Returns the identity matrix.
         * 
         * This returns a new matrix where rows and cols increase at the same time, creating a diagonal line from the top left to the bottom right of 1s.
         * 
         * @param size The rows and cols - 1 argument as they must be equal.
         * @return A new identity matrix.
         */
        static Matrix Identity(size_t size) noexcept {
            Matrix new_matrix(size, size);

            for (size_t index = 0; index < size; index++) {
                new_matrix.Set(index, index, 1);
            }

            return new_matrix;
        }

        /**
         * @brief Helper function to print the data in the matrix.
         * 
         * Prints all data in matrix, with 1 space inbetween each item a row and a newline between each row.
         */
        void Print() const {
            for (size_t r = 0; r < rows; r++) {
                for (size_t c = 0; c < cols; c++)
                    std::cout << Get(r,c) << " ";
                std::cout << "\n";
            }
        }

        /**
         * @brief Helper function to get cols.
         * 
         * Returns the value of cols.
         * 
         * @return The cols of the matrix.
         */
        size_t Cols() const noexcept { return cols; }

        /**
         * @brief Helper function to get rows.
         * 
         * Returns the value of rows.
         * 
         * @return The rows of the matrix.
         */
        size_t Rows() const noexcept { return rows; }

        /**
         * @brief Adds 2 matrices.
         *
         * This adds each number in a matrix and returns the result (Aₓᵧ + Bₓᵧ where col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this add other.
         */
        Matrix operator+(const Matrix& other) const {
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

        /**
         * @brief Subtracts 2 matricies.
         * 
         * This subtracts each number in a matrix and returns the result (Aₓᵧ - Bₓᵧ where col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this subtract other.
         */
        Matrix operator-(const Matrix& other) const {
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

        /**
         * @brief Multiplies 2 matricies.
         * 
         * This multiplies each number in a matrix and returns the result (Σₖ₌₁b ​Aₓₖ​Bₖᵧ where k is the current iteration, b is the last and col and row are indicies).
         * 
         * @param other A const reference to another matrix.
         * @return A new matrix which is the result of this multiplied other.
         */
        Matrix operator*(const Matrix& other) const {
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

        /**
         * @brief Multiplies a matrix and a scalar.
         * 
         * This multiplies each number in a matrix by a scalar and returns the result (Aₓᵧ * n where n is a constant scalar, col and row are indicies).
         * 
         * @param scalar A const scalar.
         * @return A new matrix which is the result of this multiplied by a scalar - other.
         */
        Matrix operator*(const double scalar) const {
            Matrix result(rows, cols);

            for (size_t r = 0; r < rows; r++) {
                for (size_t c = 0; c < cols; c++) {
                    result.Set(r,c, Get(r,c) * scalar);
                }
            }

            return result;
        }


        /**
         * @brief Copy operator for matrices.
         * 
         * Sets this to other, including size and data. Old data is lost and deallocated.
         * 
         * @param other A const reference to another matrix to copy.
         * @return The new data copied, a reference to this.
         */
        Matrix& operator=(const Matrix& other) {
            if (this == &other) return *this;

            delete[] data;

            rows = other.rows;
            cols = other.cols;

            data = new double[rows * cols];
            std::copy(other.data, other.data + rows * cols, data);

            return *this;
        }

        /**
         * @brief Move operator for matrices.
         * 
         * Sets data in this to data in other matrix with a deep copy, and destroys the old matrix.
         * 
         * @param other A different matrix to be destroyed and have its data moved into this.
         * @return The matrix with the moved data stored.
         */
        Matrix& operator=(Matrix&& other) noexcept {
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

        /**
         * @brief Gets item at row, col.
         * @warning No bounds checking.
         * 
         * Operator that provides get access, does not provide bounds checking.
         * 
         * @param row The row to get data at.
         * @param col The col to get data at.
         * @return A reference to the data at row,col.
         */
        T& operator()(size_t row, size_t col) {
            return data[GetIndex(row, col)];
        }

        /**
         * @brief Gets const item at row,col.
         * @warning No bounds checking.
         * 
         * Operator that provides get access, does not provide bounds checking.
         * 
         * @param row The row to get data at.
         * @param col The col to get data at.
         * @return A const reference to the data at row,col.
         */
        const T& operator()(size_t row, size_t col) const {
            return data[GetIndex(row, col)];
        }

        /**
         * @brief Equality operator for matrices.
         * 
         * Checks if sizes and data are equal. Returns true if they are and false otherwise.
         * 
         * @param other A matrix to compare against this.
         * @return True if they are equal, false otherwise.
         */
        bool operator==(const Matrix& other) const {
            if (!CheckSize(other)) return false;

            for (size_t i = 0; i < rows*cols; i++) {
                if (data[i] != other.data[i]) {
                    return false;
                }
            }

            return true;
        }
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
        std::vector<Matrix<double>> X;
        std::vector<Matrix<double>> Y;

    public:
        /**
         * @brief Default constructor
         * 
         * Sets X and Y to empty vectors. Data should be put in with AddSample or Copy after.
         */
        DataSet() {X = std::vector<Matrix<double>>(); Y = std::vector<Matrix<double>>();};

        /**
         * @brief Constructor
         * 
         * Sets X and Y to vectors with 1 element each, which is the data passed in.
         * 
         * @param X A const reference to a matrix of the train/test data.
         * @param Y A const reference to a matrix of the target data.
         */
        DataSet(const Matrix<double>& X, const Matrix<double>& Y) : X({X}), Y({Y}) {};

        /**
         * @brief Copies data into vectors.
         * 
         * Pure virtual function whose implementations should copy data into vectors by overwriting old data.
         * 
         * @param X A pointer to a matrix that will overwrite X data in the class.
         * @param Y A pointer to a matrix that will overwrite Y data in the class.
         */
        virtual void Copy(Matrix<double>* X, Matrix<double>* Y) = 0;

        /**
         * @brief Copies vectors into class.
         * 
         * Pure virtual function whose implementation should copy data into vectors by overwriting old data with full new data.
         * 
         * @param X A reference to a vector of new X data that will overwrite old X data.
         * @param Y A reference to a vector of new Y data that will overwrite old Y data.
         */
        virtual void Copy(std::vector<Matrix<double>>& X, std::vector<Matrix<double>>& Y) = 0;

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
        virtual void AddSample(Matrix<double>& X, Matrix<double>& Y) = 0;

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
        virtual std::tuple<Matrix<double>, Matrix<double>> Get(size_t index) const = 0;

        /**
         * @brief Gets vector of data matrices.
         * 
         * A pure virtual function whose implementation should return the X vector.
         * 
         * @return The vector of X data.
         */
        virtual std::vector<Matrix<double>> GetX() const = 0;

        /**
         * @brief Gets vector of target matrices.
         * 
         * A pure virtual function whose implementation should return the Y vector.
         * 
         * @return The vector of Y data.
         */
        virtual std::vector<Matrix<double>> GetY() const = 0;

        /**
         * @brief Converts all data in vectors to vectors of row matrices.
         * 
         * A pure virtual function whose implementation should convert each matrix into different row matrices and append them into vectors and return them as tuples.
         * 
         * @return A tuple of the vectors of row matrices.
         */
        virtual std::tuple<std::vector<Matrix<double>>, std::vector<Matrix<double>>> ToVector() const = 0;
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
        bool shuffle;
        // Static random number generator - only 1 exists.
        static std::mt19937 gen;
    
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
                Iterator(DataLoader* loader, size_t index) noexcept;

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
                Iterator& operator++() noexcept;

                /**
                 * @brief Increases index by 1.
                 * 
                 * Increases index by 1 (wraps around if needed) and returns the value of *this before incrementing.
                 * 
                 * @return The state of the iterator before incrementing.
                 */
                Iterator operator++(int) noexcept;

                /**
                 * @brief Checks if iterator is equal to another.
                 * 
                 * Checks if loader pointers and index are equal.
                 * 
                 * @param other A const reference to a different iterator.
                 * @return True if they are equal, false otherwise.
                 */
                bool operator==(const Iterator& other) const noexcept;

                /**
                 * @brief Checks if iterator is not equal to another.
                 * 
                 * Checks if they are equal and inverts result.
                 * 
                 * @param other A const reference to a different iterator.
                 * @return True if they are not equal, false otherwise.
                 */
                bool operator!=(const Iterator& other) const noexcept;
        };

        /**
         * @brief Gets an iterator to the start of the data.
         * 
         * Creates a new iterator with the index of 0.
         * 
         * @return An iterator pointing to the beginning of batch data.
         */
        Iterator Begin() noexcept;

        /**
         * @brief Gets an interator the end of the data.
         * 
         * Created a new iterator with the index of size of batches.
         * 
         * @return An interator pointing to the end of batch data.
         */
        Iterator End() noexcept;
};

/**
 * @struct Layer
 * @brief A singular layer inside a neural network
 * 
 * Contains weights and biases, additionally contains pointers to prev and next layers in the linked list.
 */
struct Layer {
    // The weights used for forward pass
    Matrix<double> weights;
    size_t num_weights;
    // The biases used for forward pass
    Matrix<double> biases;

    // Pointers for linked list
    Layer* next;
    Layer* prev;

    /**
     * @brief Constructor.
     * 
     * Assignes weights, biases and pointers.
     * 
     * @param weights A matrix about the weights used in forward passes.
     * @param num_weights The amount of weights in this layer.
     * @param biases The bias added to each node after weighted sum is calculated.
     * @param next A pointer to the next layer in the linked list.
     * @param prev A pointer to the previous layer in the linked list.
     */
    Layer(Matrix<double>& weights, size_t num_weights, Matrix<double>& biases, Layer* next, Layer* prev) {
        // Assign all variables
        this->weights = weights;
        this->num_weights = num_weights;
        this->biases = biases;
        this->next = next;
        this->prev = prev;
    }
};

/**
 * @class NN
 * @brief Neural network representation
 * 
 * Supports any number of inputs, outputs, hidden layers and hidden neurons. Has Forward pass, backward pass and training. Currently only supports sigmoid activation.
 */
class NN {
    protected:
        // Beginning of hidden layers and amount of hidden + outputs layers.
        Layer* head;
        size_t num_layers;
        // Amount of inputs and outputs.
        size_t inputs;
        size_t outputs;
        // Static random number generator - only 1 exists.
        static std::mt19937 gen;

    protected:
        /**
         * @brief Sigmoid activation function.
         * 
         * Converts linear output into a sigmoid curve.
         * 
         * @param x Linear input value
         * @return Value on sigmoid curve corresponding to x.
         */
        inline double Sigmoid(double x) const;

        /**
         * @brief Calculated mean squared error from outputs and targets.
         * 
         * Subtracts the matrices, sums the result and squares it to calculate loss.
         * 
         * @param output The models predicted output.
         * @param target The expected output.
         */
        inline double MSEError(Matrix<double> output, Matrix<double> target) const;
    
    public:
        /**
         * @brief Constructor.
         * 
         * Takes in inputs, stores variables and creates hidden layers.
         * 
         * @param inputs The amount of inputs for the model to take in.
         * @param outputs The amount of outputs for the model to give out.
         * @param num_layers The amount of hidden layers - does not include inputs but does include outputs.
         * @param num_neurons The amount of neurons in each layer - does not include inputs but does include outputs.
         */
        NN(size_t inputs, size_t outputs, size_t num_layers, std::vector<size_t> num_neurons);

        /**
         * @brief Destructor
         * 
         * Deletes all allocated layers and cleans up.
         */
        ~NN();

        /**
         * @brief Passes inputs through network
         * 
         * Takes inputs, passes through each hidden and output layers. Then it flattens the result into a vector.
         * 
         * @param inputs The inputs to pass into the network.
         * @return A vector of the predictions from the network.
         */
        std::vector<double> Forward(const std::vector<double>& inputs) const;

        /**
         * @brief Backward pass through network
         * 
         * Takes inputs, targets and learning rate, then updates parameters with new weights and biases.
         * 
         * @param inputs Inputs to give into network.
         * @param target The expected outputs from the network.
         * @param lr The learning rate, multiplied by changes. Smaller value for slower but more accurate learning.
         * @return The loss from backwards pass.
         */
        double Backward(const std::vector<double>& inputs, const std::vector<double>& targets, double lr);

        /**
         * @brief Trains the network from data.
         * 
         * Runs backwards pass for each batch of data for each epoch.
         * 
         * @param epochs The amount of times to repeat the backwards pass.
         * @param lr The learning rate, smaller value to learn slower and more accurately.
         * @param loader The data loader full of training data.
         */
        void Train(size_t epochs, double lr, DataLoader& loader);
};