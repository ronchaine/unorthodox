#ifndef UNORTHODOX_VERTEX_HPP
#define UNORTHODOX_VERTEX_HPP

#include <unorthodox/colour.hpp>

#include <vector>

#include <iostream>

/*
 * "vertex type" is just a amalgamation of different vertex components
 * with a few utility functions thrown in.  Components are defined in
 * this namespace.  They need to have "component_type" type alias,
 * "components" as the count of components in the component (e.g.
 * red-green-blue is 3 components inside the component), size reserved
 * for a single component e.g. sizeof(component_type).  And if the
 * component holds more than one set of the values (e.g. more than
 * one set of texture coordinates) in array_size
 */
namespace unorthodox::graphics::vertex_components
{
    /*
     * Information structure for spatial coordinates
     */
    template <typename T, uint32_t Dimension> struct spatial_info
    {
        using component_type = T;
        
        constexpr static uint32_t components    = Dimension;
        constexpr static uint32_t component_size = sizeof(component_type);
        constexpr static uint32_t array_size    = 0;
        constexpr static bool     allow_padding = true;
    };

    /*
     * Position component, gives vertex x,y,z,w member variables depending
     * on the dimensionality of the type
     */
    template <typename T, uint32_t Dimension> struct position;

    template <typename T> struct position<T, 0> : public spatial_info<T, 0> {};
    template <typename T> struct position<T, 1> : public spatial_info<T, 1> { T x; };
    template <typename T> struct position<T, 2> : public spatial_info<T, 2> { T x; T y; };
    template <typename T> struct position<T, 3> : public spatial_info<T, 3> { T x; T y; T z; };
    template <typename T> struct position<T, 4> : public spatial_info<T, 4> { T x; T y; T z; T w; };

    /*
     * Texture coordinate component, can hold more than
     * one set of texture coordinates
     */
    template <uint32_t UV_count> struct texcoord_info
    {
        using component_type = uint16_t;
        constexpr static uint32_t components = 2;
        constexpr static uint32_t component_size = sizeof(component_type);
        constexpr static uint32_t array_size = UV_count;
        constexpr static bool     allow_padding = false;
    };
    template <uint32_t UV_count> struct texture_coordinates : public texcoord_info<UV_count>
    {
        std::array<uint16_t, UV_count> u;
        std::array<uint16_t, UV_count> v;
    };
    template <> struct texture_coordinates<1> : public texcoord_info<1>
    {
        uint16_t u;
        uint16_t v;
    };
    template <> struct texture_coordinates<0> {};
}

namespace unorthodox::graphics
{
    class vertex_array;
    struct vertex_batch;

    template <typename T> concept any_vertex_type = requires(T t)
    {
        true;
    };

    /*
     * Describes vertex component's layout
     */
    struct vertex_component_layout
    {
        uint8_t components = 0;
        uint8_t component_size = 0;
        uint16_t count = 0;
    };

    /*
     * A set of vertices with same features, so they can be rendered
     * with one call
     */
    // TODO: constexpr when C++20 support improves
    struct vertex_batch
    {
        vertex_batch() = default;

        uint8_t vertex_size;

        std::vector<vertex_component_layout> component_layouts;
        std::vector<std::byte> data;
    };

    inline std::ostream& operator<<(std::ostream& stream, const vertex_batch&)
    {
        return stream;
    }

    /*
     * Vertex template type.  Basically just inherits all the components
     * requested and has some functions to make usage easier
     */
    template <typename... Other_Components>
    struct vertex : public Other_Components...
    {
        // TODO: constexpr when C++20 support improves
        static vertex_batch create_suitable_batch() noexcept;
        static bool is_batch_suitable(const vertex_batch&) noexcept;

        constexpr size_t serialise_to(char* target, const vertex_batch& batch) const noexcept;
    };

    template <typename... Other_Components>
    vertex_batch vertex<Other_Components...>::create_suitable_batch() noexcept
    {
        constexpr static std::array<uint8_t, sizeof...(Other_Components)> layout_components = { Other_Components::components... };
        constexpr static std::array<uint8_t, sizeof...(Other_Components)> layout_component_sizes = { Other_Components::component_size... };
        constexpr static std::array<uint16_t, sizeof...(Other_Components)> layout_array_sizes = { Other_Components::array_size... };

        vertex_batch batch;

        batch.vertex_size = sizeof(vertex<Other_Components...>);
        batch.component_layouts.reserve(sizeof...(Other_Components));

        for (size_t i = 0; i < sizeof...(Other_Components); ++i)
            batch.component_layouts.emplace_back(vertex_component_layout{
                layout_components[i],
                layout_component_sizes[i],
                layout_array_sizes[i]
            });

        return batch;
    }

    template <typename... Other_Components>
    bool vertex<Other_Components...>::is_batch_suitable(const vertex_batch& batch) noexcept
    {
        constexpr static std::array<uint8_t, sizeof...(Other_Components)> layout_components = { Other_Components::components... };
        constexpr static std::array<uint8_t, sizeof...(Other_Components)> layout_component_sizes = { Other_Components::component_size... };
        constexpr static std::array<uint16_t, sizeof...(Other_Components)> layout_array_sizes = { Other_Components::array_size... };
        constexpr static std::array<bool, sizeof...(Other_Components)> component_allows_padding = { Other_Components::allow_padding... };

        for (size_t i = 0; i < sizeof...(Other_Components); ++i)
        {
            std::cout << "type details (" << i << ")\n   components:" << static_cast<int>(layout_components[i]) << "\n";
            std::cout << "   size / component: " << static_cast<int>(layout_component_sizes[i]) << "\n";
            std::cout << "   count: " << layout_array_sizes[i] << "\n";
        }

        /*
        vertex_batch() = default;

        uint8_t vertex_size;

        std::vector<vertex_component_layout> component_layouts;
        std::vector<std::byte> data;
        */

        // Require there are same amount of components and the same vertex size
        if (batch.vertex_size != sizeof(vertex<Other_Components...>))
                return false;
        if (batch.component_layouts.size() != sizeof...(Other_Components))
                return false;

        for (constexpr size_t i = 0; i < sizeof...(Other_Components); ++i)
        {
            using op = std::conditional_t<component_allows_padding[i], std::less<uint16_t>, std::not_equal_to<uint16_t>>;
//            if (layout_components[i] != 
        }

        return true;
    }

    template <typename... Other_Components>
    constexpr size_t vertex<Other_Components...>::serialise_to(char* target, const vertex_batch& batch_info) const noexcept
    {
        std::cout << "batch info:\n"
                  << " vertex size: " << static_cast<int>(batch_info.vertex_size) << "B\n";

        target[0] = 0;

        return 0;
    }

    /*
     * Set of vertex batches that can be sent to the GPU.  The structure
     * divides the vertices given to different batches.
     */
    class vertex_array
    {
        public:
            template <typename T, typename... Ts> requires any_vertex_type<T>
            void add_vertices(T vertex, Ts... rest);

        private:
            template <typename T>
            vertex_batch& find_suitable_batch() noexcept;

            template <typename T, typename... Ts> requires any_vertex_type<T>
            void add_to_batch(char* add_ptr, const vertex_batch& info, T vertex, Ts... rest) noexcept;

            std::vector<vertex_batch> batches;
    };

    template <typename T, typename... Ts> requires any_vertex_type<T>
    void vertex_array::add_vertices(T vertex, Ts... rest)
    {
        vertex_batch& batch = find_suitable_batch<T>();
        char* ptr = reinterpret_cast<char*>(&batch.data[batch.data.size()]);
        batch.data.resize(batch.data.size() + (sizeof...(rest) + 1) * batch.vertex_size);

        add_to_batch(ptr, batch, std::forward<T>(vertex), rest...);
    }

    template <typename T, typename... Ts> requires any_vertex_type<T>
    void vertex_array::add_to_batch(char* append_ptr, const vertex_batch& info, T vertex, Ts... rest) noexcept
    {
        vertex.serialise_to(append_ptr, info);
        append_ptr += info.vertex_size;
        if constexpr (sizeof...(rest) > 0)
            add_to_batch(append_ptr, info, std::move(rest...));
    }

    template <typename T>
    vertex_batch& vertex_array::find_suitable_batch() noexcept
    {
        for (auto& batch : batches)
            if (T::is_batch_suitable(batch))
                return batch;

        batches.emplace_back(T::create_suitable_batch());
        return batches.back();
    }
}

#endif
