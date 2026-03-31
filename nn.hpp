#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <tuple>
#include <utility>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    const char* path; // Path of source file.
    const char* source; // Source code.
    ShaderType type; // Type of shader - vertex, fragment, compute, etc.
    std::shared_ptr<S> shader; // Pointer to graphics API shader.

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
        this->shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
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
        // Null check
        if (other.shader) {
            this->shader = other.shader;
        } else {
            this->shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        }
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
        // Remove values from other
        other.path = "";
        other.source = "";
        other.type = ShaderType::None;
        // Null check
        if (other.shader) {
            this->shader = other.shader;
            other.shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        } else {
            this->shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        }
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
    virtual ~Shader() = 0;

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
        // Null check
        if (other.shader) {
            this->shader = other.shader;
        } else {
            this->shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        }
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
        // Remove data from other
        other.path = "";
        other.source = "";
        other.type = ShaderType::None;
        // Null check
        if (other.shader) {
            this->shader = other.shader;
            other.shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        } else {
            this->shader = std::shared_ptr<S>(nullptr, [this](S* ptr) { if (!ptr) return; this->DestroyShader(); delete ptr; });
        }
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
        std::shared_ptr<B> buffer; // Graphics API representation

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
            this->buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
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
            // Null check
            if (other.buffer) {
                this->buffer = other.buffer;
            } else {
                this->buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            }
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
            // Delete data from other
            other.type = BufferType::None;
            other.read = false;
            other.write = false;
            other.binding = 0;
            // Null check
            if (other.buffer) {
                this->buffer = other.buffer;
                other.buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            } else {
                this->buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            }
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
        virtual ~Buffer() = 0;

        /**
         * @brief Gets the graphics API representation of the buffer.
         * 
         * Returns raw graphics API buffer.
         * 
         * @return The raw buffer id.
         */
        B GetId() const noexcept {
            return *buffer;
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
            // Null check
            if (other.buffer) {
                this->buffer = other.buffer;
            } else {
                this->buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            }
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
            // Delete other's data
            other.type = BufferType::None;
            other.read = false;
            other.write = false;
            other.binding = 0;
            // Null check
            if (other.buffer) {
                this->buffer = other.buffer;
                other.buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            } else {
                this->buffer = std::shared_ptr<B>(nullptr, [this](B* ptr) { if (!ptr) return; this->DestroyBuffer(); delete ptr; });
            }
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
        // Pointer to graphics API representation
        std::shared_ptr<A> array;

    public:
        /**
         * @brief Constructor.
         * 
         * Sets array to nullptr, with custom deleter.
         */
        VertexArray() noexcept {
            array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
        }

        /**
         * @brief Copy constructor
         * 
         * Copies data from other into this.
         * 
         * @param other A const reference to another vertex array.
         */
        VertexArray(const VertexArray& other) noexcept {
            // Null check
            if (other.array) {
                this->array = other.array;
            } else {
                this->array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            }
        }

        /**
         * @brief Move constructor.
         * 
         * Moves data from other into this.
         * 
         * @param other Another vertex array to have its data moved into this.
         */
        VertexArray(VertexArray&& other) noexcept {
            // Null check
            if (other.array) {
                this->array = other.array;
                other.array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            } else {
                this->array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            }
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
        virtual ~VertexArray() = 0;

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
        virtual void Unbind() = 0;

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
            // Null check
            if (other.array) {
                this->array = other.array;
            } else {
                this->array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            }
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
            // Null check
            if (other.array) {
                this->array = other.array;
                other.array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            } else {
                this->array = std::shared_ptr<A>(nullptr, [this](A* ptr) { if (!ptr) return; this->DestroyVertexArray(); delete ptr; });
            }
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
 * @struct Vec3
 * @brief Representation of 3D vector.
 * 
 * Has 3 float components: x, y, and z.
 */
struct Vec3 {
    float x; // X component
    float y; // Y component
    float z; // Z component

    /**
     * @brief Default constructor.
     * 
     * Initializes all components to 0.
     */
    Vec3() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = 0.0f;
    }

    /**
     * @brief Constructor.
     * 
     * Initializes all components.
     * 
     * @param x The x component.
     * @param y The y component.
     * @param z The z component.
     */
    Vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

/**
 * @struct Mat4
 * @brief Representation of 4x4 matrix.
 * 
 * Has a 4x4 float array, leading to 16 total float values.
 */
struct Mat4 {
    float data[16]; // 4x4 matrix

    /**
     * @brief Constructor.
     * 
     * Intializes values to 0.0.
     */
    Mat4() {
        for (int i = 0; i < 16; i++) {
            data[i] = 0.0f;
        }
    }

    /**
     * @brief Returns identity matrix.
     * 
     * Returns a matrix that when used for transformations no changes are applied.
     * 
     * @return The identity matrix.
     */
    static Mat4 Identity() {
        Mat4 m;
        m(0,0)=1;
        m(1,1)=1;
        m(2,2)=1;
        m(3,3)=1;
        return m;
    }

    /**
     * @brief Get operator.
     * 
     * Gets a float value at col,row.
     * 
     * @param col The col to get data at.
     * @param row The row to get data at.
     * @return A reference to the data at col,row.
     */
    float& operator()(int column, int row) {
        return data[column * 4 + row];
    }

    /**
     * @brief Get operator.
     * 
     * Gets a const float at col,row.
     * 
     * @param col The col to get data at.
     * @param row The row to get data at.
     * @return A const reference to the data at col,row.
     */
    const float& operator()(int column, int row) const {
        return data[column * 4 + row];
    }

    /**
     * @brief Pointer get function.
     * 
     * Gets the raw data for usage.
     * 
     * @return The raw data in the mat4.
     */
    const float* ptr() const {
        return data;
    }
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
        std::shared_ptr<I> program;
        // Uniform location cache
        std::unordered_map<std::string, size_t> uniforms;

    protected:
        /**
         * @brief Compiles a shader into the Shader class from source.
         * 
         * A pure virtual function whose implementation should compile shader source into the Shader class.
         * 
         * @param type The type of shader being compiled.
         * @param source The source code of the shader being compiled.
         * @return The newly created Shader instance in a unique pointer.
         */
        virtual std::unique_ptr<Shader<I>> Compile(ShaderType type, const char* source) = 0;

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
            this->program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });
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
            this->program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });

            // Create program
            this->CreateShaderProgram();
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
            this->num_buffers = other.num_buffers;
            this->num_vertex_arrays = other.num_vertex_arrays;
            this->purpose = other.purpose;
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
            // Null check
            if (other.program) {
                this->program = other.program;
                other.program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });
            } else {
                this->program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });
            }
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
            this->SSBO = SSBO->Copy();
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
         * @brief Creates a new copy of this.
         * 
         * A pure virtual function whose implementation should create a new shader program with the data of this.
         * 
         * @return A unique pointer to a shader program with the data of this.
         */
        virtual std::unique_ptr<ShaderProgram<I>> Copy() = 0;

        /**
         * @brief Destructor.
         * 
         * A pure virtual function whose implementation should clean up any unhandled resources.
         */
        virtual ~ShaderProgram() = 0;

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
         * @brief Attaches a shader to the program from source.
         * 
         * A pure virtual function whose implementation should compile the shader, before attaching it to the program.
         * 
         * @param type The type of shader being attached.
         * @param source The full source code of the shader.
         */
        virtual void AttachFromSource(ShaderType type, const char* source) = 0;

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
         * @brief Sets a uniform in the shader to a vector3 value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to a vector3 value.
         * 
         * @param name The name of the uniform being set.
         * @param value The values being set to the uniform.
         */
        virtual void SetVec3(const char* name, Vec3& value) = 0;

        /**
         * @brief Sets a uniform in the shader to a matrix4 value.
         * 
         * A pure virtual function whose implementation should set a uniform in shaders to a matrix4 value.
         * 
         * @param name The name of the uniform being set.
         * @param value The values being set to the uniform.
         */
        virtual void SetMat4(const char* name, Mat4& value) = 0;

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
        virtual void Draw() = 0;

        
        /**
         * @brief Move operator.
         * 
         * Moves data from other into this.
         * 
         * @param other Another program whose data will be moved into this.
         * @return The program with the newly moved data.
         */
        ShaderProgram<I>& operator=(ShaderProgram<I>&& other) noexcept {
            // Copy values into this
            this->num_shaders = other.num_shaders;
            this->num_buffers = other.num_buffers;
            this->num_vertex_arrays = other.num_vertex_arrays;
            this->purpose = other.purpose;
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
            // Null check
            if (other.program) {
                this->program = other.program;
                other.program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });
            } else {
                this->program = std::shared_ptr<I>(nullptr, [this](I* ptr) { if (!ptr) return; this->DestroyShaderProgram(); delete ptr; });
            }
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
        std::unique_ptr<W, void (*)(W*)> window;

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
            // Preserve custom deleter
            this->window = std::make_unique<W, void (*)(W*)>(nullptr, [this](W* ptr) { if (!ptr) return; this->DestroyWindow(); delete ptr; });
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

            this->CreateWindow();
        }

        // Window is unique pointer, no copy operations
        Window(const Window& other) = delete;

        /**
         * @brief Move constructor.
         * 
         * Moves data from other into this, and set other's data to 0/null.
         * 
         * @param other A different window whose data will be moved into this.
         */
        Window(Window&& other) noexcept {
            // Move values
            this->width = other.width;
            this->height = other.height;
            this->title = other.title;
            this->window = std::move(other.window);
            
            // Set other's values to null
            other.width = 0;
            other.height = 0;
            other.title = "";
            other.window = std::make_unique<W, void (*)(W*)>(nullptr, [this](W* ptr) { if (!ptr) return; this->DestroyWindow(); delete ptr; });
        }

        /**
         * @brief Creates a window according to member variables in graphics API.
         * 
         * A pure virtual function whose implementation should create a window in the graphics API, according to specified member variables.
         */
        virtual void CreateWindow() = 0;

        /**
         * @brief Destroyes window in graphics api.
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
        virtual ~Window() = 0;

        /**
         * @brief Set all data to null.
         * @warning Should not call delete or DestroyWindow, unique pointer handles that.
         * 
         * A pure virtual function whose implementation should clear all data in this.
         */
        virtual void Clear() = 0;

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
            this->window = std::move(other.window);

            other.width = 0;
            other.height = 0;
            other.title = "";
            other.window = std::make_unique<W, void (*)(W*)>(nullptr, [this](W* ptr) { if (!ptr) return; this->DestroyWindow(); delete ptr; });

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
        Window<W> window;
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
            this->window = Window<W>();
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
         * A pure virtual function whose implementation should copy data from another context.
         * 
         * @param other Another context whose data will be cloned.
         */
        virtual void Clone(GraphicsAPIContext<W, I>& other) = 0;

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
        virtual ~GraphicsAPIContext() = 0;

        /**
         * @brief Move operator.
         * 
         * Moves data from another context into this.
         * 
         * @param other A different context whose data will be moved into this.
         * @return The context with newly moved data.
         */
        GraphicsAPIContext<W, I>& operator=(GraphicsAPIContext<W, I>&& other) noexcept {
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

/**
 * @struct Layer
 * @brief A singular layer inside a neural network
 * 
 * Contains weights and biases, additionally contains pointers to prev and next layers in the linked list.
 */
struct Layer {
    // The weights used for forward pass
    Matrix weights;
    size_t num_weights;
    // The biases used for forward pass
    Matrix biases;

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
    Layer(Matrix& weights, size_t num_weights, Matrix& biases, Layer* next, Layer* prev) {
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
        inline double MSEError(Matrix output, Matrix target) const;
    
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