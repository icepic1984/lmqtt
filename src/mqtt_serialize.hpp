#ifndef MQTT_SERIALIZE_HPP
#define MQTT_SERIALIZE_HPP

namespace lmqtt {

namespace detail {

template <typename InputIterator>
inline uint16_t read_big_endian(InputIterator start)
{
	return(*(start+1) << 0 ) | *(start) << 8;
}

template <typename T,typename InputIterator>
struct deserialize_helper;

template <typename T, typename InputIterator>
struct deserialize_helper
{
   static T deserialize(InputIterator begin){
	   return(*begin);
   }
};

template <typename InputIterator>
struct deserialize_helper<uint16_t,InputIterator>
{
   static uint16_t deserialize(InputIterator begin){
	   return(read_big_endian(begin));
   }
};

template <typename InputIterator>
struct deserialize_helper<std::string, InputIterator>
{
   static std::string deserialize(InputIterator begin, std::size_t n){
	   std::string str(n,' ');
	   for(std::size_t i = 0; i < n; ++i){
		   str[i] = *(begin+i);
	   }
	   return str;
   }
};

template <typename T, typename InputIterator>
struct serialize_helper;

template <typename InputIterator>
struct serialize_helper<uint8_t,InputIterator>
{
   static void serialize(InputIterator begin, uint8_t value){
	   	const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(&value);
		std::copy(value_begin,value_begin + 1, begin);
   }
};

template <typename InputIterator>
struct serialize_helper<uint16_t,InputIterator>
{
   static void serialize(InputIterator begin, uint16_t value){
	   uint16_t tmp = ntohs(value);
	   const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(&tmp);
	   std::copy(value_begin,value_begin + sizeof(uint16_t), begin);
   }
};

template <typename InputIterator>
struct serialize_helper<std::string,InputIterator>
{
   static void serialize(InputIterator begin, const std::string& value){
	   std::size_t size = value.size();
	   const uint8_t* value_begin = reinterpret_cast<const uint8_t*>(value.data());
	   std::copy(value_begin,value_begin + size, begin);
   }
};

}

template <typename InputIterator,typename T>
inline void serialize(InputIterator begin, const T& value)
{
	detail::serialize_helper<T,InputIterator>::serialize(begin,value);
}

template <typename T,typename InputIterator>
inline T deserialize(InputIterator begin)
{
	return detail::deserialize_helper<T,InputIterator>::deserialize(begin);
}

template <typename T,typename InputIterator>
inline T deserialize(InputIterator begin,std::size_t n)
{
	return detail::deserialize_helper<T,InputIterator>::deserialize(begin,n);
}
	      
}


#endif
