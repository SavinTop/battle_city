#pragma once

#include <variant>
#include "serde/serialization.h"

namespace msg
{
    template <typename... ARGS>
    struct fld_el
    {

        fld_el() = default;

        template <typename T>
        void assign(T value = T(), bool active = false)
        {
            m_value = value;
            m_active = active;
            m_size = serde::type_size<T>(value);
        }

        template <typename T>
        bool is_type() const
        {
            return std::holds_alternative<T>(m_value);
        }

        bool is_avaliable() const
        {
            return m_active;
        }

        void set_active(bool active){
            m_active = active;
        }

        size_t size() const
        {
            return m_size;
        }

        template<typename T>
        void set(T value){
            m_value = value;
            m_size = serde::type_size<T>(std::get<T>(m_value));
        }

        template<typename T>
        T get() const{
            return std::get<T>(m_value);
        }

    private:
        std::variant<ARGS...> m_value;
        bool m_active;
        size_t m_size;
    };

    template <size_t SIZE, class FLD_TYPE>
    struct fld_list
    {
        template <typename T>
        const FLD_TYPE &operator[](T fld) const
        {
            return m_els[static_cast<size_t>(fld)];
        }

        template <typename T>
        FLD_TYPE &operator[](T fld)
        {
            return m_els[static_cast<size_t>(fld)];
        }

        FLD_TYPE m_els[SIZE];
    };

} // namespace msg
