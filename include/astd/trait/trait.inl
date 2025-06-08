template<typename T>
	requires std::is_enum_v<T>
Enum<T>::Enum() : m_value(type()) {}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>::Enum(const type& value) : m_value(value) {}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>::Enum(const T& value) : m_value(to_underlying(value)) {}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>::Enum(const Enum& other) : m_value(other.m_value) {}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>::Enum(Enum&& other) noexcept : m_value(std::move(other.m_value)) {}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator==(const Enum& value) const
{
	return m_value == value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator!=(const Enum& value) const
{
	return m_value != value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator>(const Enum& value) const
{
	return m_value > value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator>=(const Enum& value) const
{
	return m_value >= value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator<(const Enum& value) const
{
	return m_value < value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator<=(const Enum& value) const
{
	return m_value <= value.m_value;
}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>& Enum<T>::operator=(const T& value)
{
    m_value = to_underlying(value);
    return *this;
}

template<typename T>
	requires std::is_enum_v<T>
Enum<T>& Enum<T>::operator=(const Enum& value)
{
	if (value != *this)
		m_value = value.m_value;
	return *this;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator==(const T& value) const
{
    return m_value == to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator!=(const T& value) const
{
    return m_value != to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator>(const T& value) const
{
	return m_value > to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator>=(const T& value) const
{
	return m_value >= to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator<(const T& value) const
{
	return m_value < to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr bool Enum<T>::operator<=(const T& value) const
{
	return m_value <= to_underlying(value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr Enum<T>::operator bool() const
{
	return !!m_value;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr Enum<T>::operator T() const
{
	return static_cast<T>(m_value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr Enum<T>::operator type() const
{
	return m_value;
}




template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>::BitFlag(const type& value) : Enum<T>(value) {}

template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>::BitFlag(const T& value) : Enum<T>(value) {}

template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>::BitFlag(const BitFlag& other) : Enum<T>(static_cast<Enum<T>>(other)) {}

template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>::BitFlag(BitFlag&& other) noexcept : Enum<T>(static_cast<Enum<T>>(other)) {}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T>& BitFlag<T>::operator|=(const BitFlag& value)
{
	this->m_value |= value.m_value;
	return *this;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T>& BitFlag<T>::operator&=(const BitFlag& value)
{
	this->m_value &= value.m_value;
	return *this;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T>& BitFlag<T>::operator^=(const BitFlag& value)
{
	this->m_value ^= value.m_value;
	return *this;
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator|(const BitFlag& value) const
{
	return BitFlag(this->m_value | value.m_value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator&(const BitFlag& value) const
{
	return BitFlag(this->m_value & value.m_value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator^(const BitFlag& value) const
{
	return BitFlag(this->m_value ^ value.m_value);
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator|(const T& value) const
{
	return BitFlag(this->m_value | to_underlying(value));
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator&(const T& value) const
{
	return BitFlag(this->m_value & to_underlying(value));
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator^(const T& value) const
{
	return BitFlag(this->m_value ^ to_underlying(value));
}

template<typename T>
	requires std::is_enum_v<T>
constexpr BitFlag<T> BitFlag<T>::operator~() const
{
	return BitFlag(~this->m_value);
}

template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>& BitFlag<T>::operator=(const BitFlag<T>& value)
{
	if (this->m_value != value.m_value)
        this->m_value = value.m_value;
    return *this;
}

template<typename T>
	requires std::is_enum_v<T>
BitFlag<T>& BitFlag<T>::operator=(const T& value)
{
	if (this->m_value != to_underlying(value))
		this->m_value = to_underlying(value);
	return *this;
}