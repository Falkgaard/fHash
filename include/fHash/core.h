//# pragma once
# ifndef HASH_HH_WMG7UEGC
# define HASH_HH_WMG7UEGC
# include <cstdint>
# include <type_traits>
# include <concepts>
# include <string_view>
# include <bit>
# ifndef FALK_NS
#    define FALK_NS falk
# endif
namespace FALK_NS {
   namespace hash {
      namespace {
         using u8  = std::uint8_t;
         using u16 = std::uint16_t;
         using u32 = std::uint32_t;
         using u64 = std::uint64_t;
         using i8  = std::int8_t;
         using i16 = std::int16_t;
         using i32 = std::int32_t;
         using i64 = std::int64_t;
         using f32 = float;
         using f64 = double;

         template <typename>
         struct always_false final { static constexpr bool value = false; };

         template <typename T>
         [[nodiscard]] auto constexpr
         typename_of() noexcept {
            using S = std::string_view::size_type;
            auto parse = []( std::string_view str, S trim_begin, S trim_end ) {
               str.remove_prefix( trim_begin );
               str.remove_suffix( trim_end   );
               return str;
            };
            #if   defined( __clang__ )
               return parse( __PRETTY_FUNCTION__ , 32, 1 );
            #elif defined( __GNUC__ )
               return parse( __PRETTY_FUNCTION__ , 47, 1 );
            #elif defined( _MSC_VER )
               return parse( __FUNCSIG__ , 0, 0 ); // TODO: MSVC bounds
            #else
               static_assert( always_false_v<T>, "Unsupported compiler!" );
            #endif
         }
      } // end-of-unnamed-namespace in #FALK_NS::hash

      using hash_t = u64;

      template <typename T = int>
      bool constexpr always_false_v = always_false<T>::value;

      hash_t constexpr prime { 0x0000'0100'0000'01B3 };
      hash_t constexpr basis { 0xCBF2'9CE4'8422'2325 };

      #define FWD(x) std::forward<decltype((x))>((x))

      template <typename T>
      concept primitive = std::integral<T> or std::floating_point<T>
              or std::convertible_to<T,std::string_view>;

   // hash single value:

      // customization point for user data-types via template specialization
      template <typename T>
      [[nodiscard]] hash_t constexpr
      hash_impl( hash_t const carry, T const &key ) noexcept {
         static_assert( always_false_v<T>, "Specialization missing!" );
         return 0;
      }

      // core fnv-1a hash algorithm:
      [[nodiscard]] hash_t constexpr
      hash_impl( hash_t const carry, u64 const key ) noexcept {
         hash_t result { carry };
         result ^= static_cast<hash_t>( key );
         result *= prime;
         return result;
      }

      [[nodiscard]] hash_t constexpr
      hash_with_carry( hash_t const carry, std::convertible_to<std::string_view> auto const &key ) noexcept {
         hash_t            result  { carry };
         std::string_view  sv      { key   };
         for ( char const c: sv )
            result = hash_impl( result, static_cast<u64>(c) );
         return result;
      }

      template <std::integral T> requires (
         not (std::signed_integral<T> and sizeof(T) != sizeof(i64)) // handle i64 case separately below
         and not std::is_pointer_v<T>                               // disallow pointers
      )
      [[nodiscard]] hash_t constexpr
      hash_with_carry( hash_t const carry, T const key ) noexcept
      {
         return hash_impl( carry, static_cast<u64>(key) ); // non-i64 case
      }

      [[nodiscard]] hash_t constexpr 
      hash_with_carry( hash_t const carry, i64 const key ) noexcept
      {
         return hash_impl( carry, std::bit_cast<u64>(key) ); // i64 case
      }

      template <typename T> requires (not primitive<T>)
      [[nodiscard]] hash_t constexpr
      hash_with_carry( hash_t const carry, T const &key ) noexcept {
         return hash_impl<T>( carry, key );
      }

      template <typename T>
      [[nodiscard]] hash_t constexpr
      hash( T const &key ) noexcept {
         return hash_with_carry( basis, key );
      }
      
      template <std::floating_point T>
      [[nodiscard]] hsh_t constexpr
      hash_with_carry( hash_t const carry, T const key ) noexcept {
         if constexpr ( std::same_as<T,f32> )
            return hash_impl( carry, static_cast<u64>(std::bit_cast<u32>(key)) );
         else if constexpr ( std::same_as<T,f64> )
            return hash_impl( carry, std::bit_cast<u64>(key) );
         else static_assert( always_false_v<T>, "Unsupported floating point precision!" );
      }

   // hash pack:

      [[nodiscard]] hash_t constexpr
      hash_pack_with_carry( hash_t const carry, auto const &tail ) noexcept {
         return hash_with_carry( carry, tail );
      }
      
      [[nodiscard]] hash_t constexpr
      hash_pack_with_carry( hash_t const carry, auto const &head, auto const &...rest ) noexcept {
         return hash_pack_with_carry( hash_with_carry( carry, head ), rest... );
      }

      [[nodiscard]] hash_t constexpr
      hash_pack( auto const &head, auto const &...rest ) noexcept {
         return hash_pack_with_carry( basis, head, rest... );
      }

   // hash types:

      hash_t constexpr type_hash_basis {
         # ifndef FHASH_CUSTOM_TYPE_BASIS
            basis
         # else
            FHASH_CUSTOM_TYPE_BASIS // optional to make hash the hash of type Foo different from the hash of "Foo"
         # endif
      };

      template <class T>
      [[nodiscard]] auto constexpr
      hash_type() noexcept
      {
         return hash_with_carry( type_hash_basis, typename_of<T>() );
      }

      template <class T>
      [[nodiscard]] auto constexpr
      hash_type_with_carry( hash_t const carry ) noexcept
      {
         return hash_with_carry( carry, typename_of<T>() );
      }

      template <class T>
      struct type_wrapper final {
         using class_type = T;
      };

      template <class T>
      type_wrapper<T> constexpr wrap_type = {};

      template <class T>
      [[nodiscard]] hash_t constexpr
      hash_with_carry( hash_t const carry, type_wrapper<T> ) noexcept {
         return hash_type_with_carry<T>( carry );
      }

   // hash members:

      template <class T, typename M>
      [[nodiscard]] auto constexpr
      hash_member( M T::*member, T const &v ) noexcept
      {
         return hash_with_carry( hash_type<T>(), v.*member );
      }

      template <class T, typename M>
      [[nodiscard]] auto constexpr
      hash_member_with_carry( hash_t const carry, M T::*member, T const &v ) noexcept
      {
         return hash_with_carry( hash_type_with_carry<T>(carry), v.*member );
      }

      template <class T, typename M>
      struct member_wrapper final {
         using class_type  = T;
         using member_type = M;
         using member_ptr  = M T::*;
         member_ptr member;
         T const *instance;
      };

      template <class T, typename M>
      [[nodiscard]] member_wrapper<T,M> constexpr
      wrap_member( M T::*member, T const &instance ) noexcept {
         return { member, &instance };
      }

      template <class T, typename M>
      [[nodiscard]] hash_t constexpr
      hash_with_carry( hash_t const carry, member_wrapper<T,M> /*&&*/m ) noexcept {
         return hash_member_with_carry( carry, m.member, *(m.instance) );
      }

   // literal operator:

      namespace literals {
         [[nodiscard]] auto constexpr
         operator""_h ( char const * const s, std::size_t const n ) noexcept
         {
            return hash( std::string_view(s,n) );
         }
	   } // end-of-namespace #FALK_NS::hash::literals

   } // end-of-namespace #FALK_NS::hash

   namespace literals {
      using namespace hash::literals; // export to top level literal namespace
   } // end-of-namespace #FALK_NS::literals

} // end-of-namespace #FALK_NS

#undef FWD

namespace fhash = falk::hash; // shorthand namespace

#endif // end-of-header-guard HASH_HH_WMG7UEGC

///////////////////////////////////////////////////////// DEMO /////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
//#include "falk/hash.hh"

struct Foo {
   int    i = 420;
   bool   b = true;
   double f = 6.9;
};

// user data-type customization:
template <>
[[nodiscard]] fhash::hash_t constexpr
fhash::hash_impl<Foo>( fhash::hash_t const carry, Foo const &f ) noexcept {
   return hash_pack_with_carry( hash_type_with_carry<Foo>(carry), f.i, f.b, f.f );
}

int main() {
   using namespace fhash;
   using namespace fhash::literals;
   Foo f;
   std::cout << std::hex << std::uppercase
             << "hello"_h                                            << ", "  // hash literal operator
             << hash( 42069 )                                        << ", "  // hash single value
             << hash_type<Foo>()                                     << ", "  // hash single type
             << hash_member( &Foo::i, f )                            << ", "  // hash single member (and type; see below)
             << hash_pack( true, 2.0, 3, '4', 5.0f, "six", 0x7, f )  << ", "  // hash pack of values (order matters!)
             << hash_pack( wrap_member(&Foo::i, f), wrap_type<Foo> ) << '\n'; // supports custom wrappers for member/type

   struct A { int i = 4;        } constexpr a;
   struct B { int i = 4, j = 4; } constexpr b;
   static_assert( hash_member(&A::i, a) != hash_member(&B::i, b) ); // same type & value, different classes => different hash
   static_assert( hash_member(&B::i, b) != hash_member(&B::j, b) ); // same type & value,      same classes =>      same hash
   // TODO: make the second case produce different hashes as well
}a
