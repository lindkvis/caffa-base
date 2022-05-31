///////////////////////////////////////////////////////////////////////////////
//
// Adapted from Microsoft GSL
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iosfwd> // for ostream
#include <system_error> // for hash
#include <type_traits> // for enable_if_t, is_convertible, is_assignable

//
// Temporary until MSVC STL supports no-exceptions mode.
// Currently terminate is a no-op in this mode, so we add termination behavior back
//
#if defined( _MSC_VER ) && ( defined( _KERNEL_MODE ) || ( defined( _HAS_EXCEPTIONS ) && !_HAS_EXCEPTIONS ) )
#define CAFFA_KERNEL_MODE

#define CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND
#include <intrin.h>
#define RANGE_CHECKS_FAILURE 0

#if defined( __clang__ )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
#endif // defined(__clang__)

#else // defined(_MSC_VER) && (defined(_KERNEL_MODE) || (defined(_HAS_EXCEPTIONS) &&
      // !_HAS_EXCEPTIONS))

#include <exception>

#endif // defined(_MSC_VER) && (defined(_KERNEL_MODE) || (defined(_HAS_EXCEPTIONS) &&
       // !_HAS_EXCEPTIONS))

//
// make suppress attributes parse for some compilers
// Hopefully temporary until suppression standardization occurs
//
#if defined( __clang__ )
#define CAFFA_SUPPRESS( x ) [[caffa::suppress( "x" )]]
#else
#if defined( _MSC_VER ) && !defined( __INTEL_COMPILER )
#define CAFFA_SUPPRESS( x ) [[caffa::suppress( x )]]
#else
#define CAFFA_SUPPRESS( x )
#endif // _MSC_VER
#endif // __clang__

#define CAFFA_STRINGIFY_DETAIL( x ) #x
#define CAFFA_STRINGIFY( x ) CAFFA_STRINGIFY_DETAIL( x )

#if defined( __clang__ ) || defined( __GNUC__ )
#define CAFFA_LIKELY( x ) __builtin_expect( !!( x ), 1 )
#define CAFFA_UNLIKELY( x ) __builtin_expect( !!( x ), 0 )

#else

#define CAFFA_LIKELY( x ) ( !!( x ) )
#define CAFFA_UNLIKELY( x ) ( !!( x ) )
#endif // defined(__clang__) || defined(__GNUC__)

//
// CAFFA_ASSUME(cond)
//
// Tell the optimizer that the predicate cond must hold. It is unspecified
// whether or not cond is actually evaluated.
//
#ifdef _MSC_VER
#define CAFFA_ASSUME( cond ) __assume( cond )
#elif defined( __GNUC__ )
#define CAFFA_ASSUME( cond ) ( ( cond ) ? static_cast<void>( 0 ) : __builtin_unreachable() )
#else
#define CAFFA_ASSUME( cond ) static_cast<void>( ( cond ) ? 0 : 0 )
#endif

//
// CAFFA.assert: assertions
//

namespace caffa
{

namespace details
{
#if defined( CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND )

    typedef void( __cdecl* terminate_handler )();

    // clang-format off
    CAFFA_SUPPRESS(f.6) // NO-FORMAT: attribute
    // clang-format on
    [[noreturn]] inline void __cdecl default_terminate_handler() { __fastfail( RANGE_CHECKS_FAILURE ); }

    inline caffa::details::terminate_handler& get_terminate_handler() noexcept
    {
        static terminate_handler handler = &default_terminate_handler;
        return handler;
    }

#endif // defined(CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND)

    [[noreturn]] inline void terminate() noexcept
    {
#if defined( CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND )
        ( *caffa::details::get_terminate_handler() )();
#else
        std::terminate();
#endif // defined(CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND)
    }

} // namespace details
} // namespace caffa

#define CAFFA_CONTRACT_CHECK( type, cond ) \
    ( CAFFA_LIKELY( cond ) ? static_cast<void>( 0 ) : caffa::details::terminate() )

#define Expects( cond ) CAFFA_CONTRACT_CHECK( "Precondition", cond )
#define Ensures( cond ) CAFFA_CONTRACT_CHECK( "Postcondition", cond )

#if defined( CAFFA_MSVC_USE_STL_NOEXCEPTION_WORKAROUND ) && defined( __clang__ )
#pragma clang diagnostic pop
#endif

namespace caffa
{
namespace details
{
    template <typename T, typename = void>
    struct is_comparable_to_nullptr : std::false_type
    {
    };

    template <typename T>
    struct is_comparable_to_nullptr<T, std::enable_if_t<std::is_convertible<decltype( std::declval<T>() != nullptr ), bool>::value>>
        : std::true_type
    {
    };
} // namespace details

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class T>
class not_null
{
public:
    static_assert( details::is_comparable_to_nullptr<T>::value, "T cannot be compared to nullptr." );

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr not_null( U&& u )
        : ptr_( std::forward<U>( u ) )
    {
        Expects( ptr_ != nullptr );
    }

    template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
    constexpr not_null( T u )
        : ptr_( std::move( u ) )
    {
        Expects( ptr_ != nullptr );
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr not_null( const not_null<U>& other )
        : not_null( other.get() )
    {
    }

    not_null( const not_null& other ) = default;
    not_null& operator=( const not_null& other ) = default;
    constexpr std::conditional_t<std::is_copy_constructible<T>::value, T, const T&> get() const
    {
        Ensures( ptr_ != nullptr );
        return ptr_;
    }

    constexpr                  operator T() const { return get(); }
    constexpr decltype( auto ) operator->() const { return get(); }
    constexpr decltype( auto ) operator*() const { return *get(); }

    // prevents compilation when someone attempts to assign a null pointer constant
    not_null( std::nullptr_t ) = delete;
    not_null& operator=( std::nullptr_t ) = delete;

    // unwanted operators...pointers only point to single objects!
    not_null& operator++()                       = delete;
    not_null& operator--()                       = delete;
    not_null  operator++( int )                  = delete;
    not_null  operator--( int )                  = delete;
    not_null& operator+=( std::ptrdiff_t )       = delete;
    not_null& operator-=( std::ptrdiff_t )       = delete;
    void      operator[]( std::ptrdiff_t ) const = delete;

private:
    T ptr_;
};

template <class T>
auto make_not_null( T&& t ) noexcept
{
    return not_null<std::remove_cv_t<std::remove_reference_t<T>>>{ std::forward<T>( t ) };
}

template <class T>
std::ostream& operator<<( std::ostream& os, const not_null<T>& val )
{
    os << val.get();
    return os;
}

template <class T, class U>
auto operator==( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() == rhs.get() ) )
    -> decltype( lhs.get() == rhs.get() )
{
    return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() != rhs.get() ) )
    -> decltype( lhs.get() != rhs.get() )
{
    return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() < rhs.get() ) )
    -> decltype( lhs.get() < rhs.get() )
{
    return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() <= rhs.get() ) )
    -> decltype( lhs.get() <= rhs.get() )
{
    return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() > rhs.get() ) )
    -> decltype( lhs.get() > rhs.get() )
{
    return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=( const not_null<T>& lhs, const not_null<U>& rhs ) noexcept( noexcept( lhs.get() >= rhs.get() ) )
    -> decltype( lhs.get() >= rhs.get() )
{
    return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-( const not_null<T>&, const not_null<U>& ) = delete;
template <class T>
not_null<T> operator-( const not_null<T>&, std::ptrdiff_t ) = delete;
template <class T>
not_null<T> operator+( const not_null<T>&, std::ptrdiff_t ) = delete;
template <class T>
not_null<T> operator+( std::ptrdiff_t, const not_null<T>& ) = delete;

} // namespace caffa

namespace std
{
template <class T>
struct hash<caffa::not_null<T>>
{
    std::size_t operator()( const caffa::not_null<T>& value ) const { return hash<T>{}( value.get() ); }
};

} // namespace std