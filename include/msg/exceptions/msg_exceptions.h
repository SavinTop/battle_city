#pragma once

#include<stdexcept>

namespace msg{

namespace serde{
using serde_error = std::runtime_error;
}

using type_error = std::runtime_error;
using immutable_error = std::runtime_error;

using length_error = std::length_error;

}