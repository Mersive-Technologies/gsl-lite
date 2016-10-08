//
// gsl-lite is based on GSL: Guideline Support Library,
// https://github.com/microsoft/gsl
//
// Copyright (c) 2015 Martin Moene
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

#include "gsl-lite.t.h"
#include <functional>

CASE( "finally: Allows lambda to run" )
{
#if gsl_CPP11_OR_GREATER
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _ = finally( [&]() { F::incr( i ); } );
        EXPECT( i == 0 );
    }
    EXPECT( i == 1 );
#else
    EXPECT( !!"lambda is not available (no C++11)" );
#endif
}

CASE( "finally: Allows function with bind" )
{
#if gsl_CPP11_OR_GREATER
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _ = finally( std::bind( &F::incr, std::ref( i ) ) );
        EXPECT( i == 0 );
    }
    EXPECT( i == 1 );
#else
    EXPECT( !!"auto and std::ref perhaps not available (no C++11)" );
#endif
}

namespace{ int g_i = 0; }

CASE( "finally: Allows pointer to function" )
{
    struct F { static void incr() { g_i += 1; } };

#if gsl_CPP11_OR_GREATER

    g_i = 0;
    {
        auto _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#else
    g_i = 0;
    {
        final_act _ = finally( &F::incr );
        EXPECT( g_i == 0 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "finally: Allows to move final_act" )
{
#if gsl_CPP11_OR_GREATER
    struct F { static void incr( int & i ) { i += 1; } };

    int i = 0;
    {
        auto _1 = finally( [&]() { F::incr( i ); } );
        {
            auto _2 = std::move( _1 );
            EXPECT( i == 0 );
        }
        EXPECT( i == 1 );
        {
            auto _2 = std::move( _1 );
            EXPECT( i == 1 );
        }
        EXPECT( i == 1 );
    }
    EXPECT( i == 1 );
#else
    struct F { static void incr() { g_i += 1; } };

    g_i = 0;
    {
        final_act _1 = finally( &F::incr );
        {
            final_act _2 = _1;
            EXPECT( g_i == 0 );
        }
        EXPECT( g_i == 1 );
        {
            final_act _2 = _1;
            EXPECT( g_i == 1 );
        }
        EXPECT( g_i == 1 );
    }
    EXPECT( g_i == 1 );
#endif
}

CASE( "finally: Allows moving final_act to throw" "[.]")
{
#if gsl_CPP11_OR_GREATER
    struct action
    {
        int & i_;
        void operator()(){ i_ += 1; }     
        action( int & i ) : i_( i ) {}
        action( action && other ) : i_( other.i_) { throw std::runtime_error("action move-ctor"); }   
    };

    int i = 0;
    {
        {
            EXPECT_THROWS( finally( action( i ) ) );
        }
        EXPECT( i == 1 );
    }

    // ... 
#else
    EXPECT( !!"lambda is not available (no C++11)" );
#endif
}

CASE( "narrow_cast<>: Allows narrowing without value loss" )
{
    EXPECT( narrow_cast<char>( 120 ) == 120 );
}

CASE( "narrow_cast<>: Allows narrowing with value loss" )
{
    EXPECT( narrow_cast<unsigned char>( 300 ) == 44 );
}

CASE( "narrow<>(): Allows narrowing without value loss" )
{
    EXPECT( narrow<char>( 120 ) == 120 );
}

CASE( "narrow<>(): Terminates when narrowing with value loss" )
{
    EXPECT_THROWS_AS( narrow<char>( 300 ), narrowing_error );
}

CASE( "narrow<>(): Terminates when narrowing with sign loss" )
{
    EXPECT_THROWS_AS( narrow<unsigned>( -42 ), narrowing_error );
}

// end of file
