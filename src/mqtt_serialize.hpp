#ifndef MQTT_SERIALIZE_HPP
#define MQTT_SERIALIZE_HPP

namespace lmqtt {
namespace detail {

template <typename InputIterator>
inline uint16_t read_big_endian(InputIterator start)
{
	return(*(start+1) << 0 ) | *(start) << 8;
}

template <typename T>
struct get_size_helper;

template <typename T>
struct get_size_helper
{
   static std::size_t get_size(const T value){
	   return sizeof(value);
   }
};

template <>
struct get_size_helper<std::string>
{
   static std::size_t get_size(const std::string& value){
	   return value.size();
   }
};

template <>
struct get_size_helper<std::vector<uint8_t>>
{
	static std::size_t get_size(const std::vector<uint8_t>& value) {
		return value.size();
	 }

};

template <typename InputIterator, typename T>
struct deserialize_helper;

template <typename InputIterator, typename T>
struct deserialize_helper
{
	static InputIterator deserialize(InputIterator begin,
	                                 InputIterator end, T& value) {
		std::size_t size = get_size_helper<T>::get_size(value);
		if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
		value = *begin;
		return begin+size;
	}
};

template <typename InputIterator>
struct deserialize_helper<InputIterator,uint16_t>
{
	static InputIterator deserialize(InputIterator begin,
	                                 InputIterator end, uint16_t& value) {
		std::size_t size = get_size_helper<uint16_t>::get_size(value);
		if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
		value = read_big_endian(begin);
		return begin+size;
	}
};

template <typename InputIterator>
struct deserialize_helper<InputIterator,std::string>
{
   static InputIterator deserialize(InputIterator begin, InputIterator end,
                                    std::string& value){
	   std::size_t size =  get_size_helper<std::string>::get_size(value);
	   if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
	   for(std::size_t i = 0; i < size; i++){
		   value[i] = *(begin+i);
	   }
	   return begin+size;
   }
};


template <typename InputIterator>
struct deserialize_helper<InputIterator,std::vector<uint8_t>>
{
   static InputIterator deserialize(InputIterator begin, InputIterator end,
                                    std::vector<uint8_t>& value){
	   std::size_t size = get_size_helper<std::vector<uint8_t>>::get_size(value);
	   if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
	   for(std::size_t i = 0; i < size; i++){
		   value[i] = *(begin+i);
	   }
	   return begin+size;
   }
};

template <typename InputIterator, typename T>
struct serialize_helper;

template <typename InputIterator>
struct serialize_helper<InputIterator, uint8_t>
{
   static InputIterator serialize(InputIterator begin, InputIterator end, uint8_t value){
	   std::size_t size = get_size_helper<uint8_t>::get_size(value);
	   if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
	   const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(&value);
	   std::copy(value_begin,value_begin + size, begin);
	   return begin+size;
   }
};

template <typename InputIterator>
struct serialize_helper<InputIterator,uint16_t>
{
   static InputIterator serialize(InputIterator begin, InputIterator end,
                                  uint16_t value){
	   std::size_t size = get_size_helper<uint16_t>::get_size(value);
	   if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
	   uint16_t tmp = ntohs(value);
	   const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(&tmp);
	   std::copy(value_begin,value_begin + sizeof(uint16_t), begin);
	   return begin+size;
   }
};

template <typename InputIterator>
struct serialize_helper<InputIterator,std::string>
{
   static InputIterator serialize(InputIterator begin, InputIterator end,
                                  const std::string& value){
	   std::size_t size = get_size_helper<std::string>::get_size(value);
	   if(std::distance(begin,end) < size) throw std::out_of_range("Out of range");
	   const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(value.data());
	   std::copy(value_begin,value_begin + size, begin);
	   return begin+size;
   }
};

}

template <typename InputIterator,typename T>
inline InputIterator serialize(InputIterator begin, InputIterator end,
                               const T& value)
{
	return(detail::serialize_helper<InputIterator,T>::serialize(begin,end,value));
}

template <typename InputIterator, typename T>
inline InputIterator deserialize(InputIterator begin, InputIterator end, T& value)
{
	static_assert(std::is_same<T,std::string>() ||
	              std::is_same<T,std::vector<uint8_t>>() ||
	              std::is_same<T,uint8_t>() ||
	              std::is_same<T,uint16_t>(), "Type not supported");
	return detail::deserialize_helper<InputIterator,T>::
	   deserialize(begin,end,value);
}


}

#endif
