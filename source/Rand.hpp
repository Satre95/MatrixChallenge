#pragma once

#include <random>
#include <cmath>
class Rand {
public:
    Rand()
    : mBase( 214u ), mHaveNextNextGaussian( false )
    {}
    
    Rand( uint32_t seed )
    : mBase( seed ), mHaveNextNextGaussian( false )
    {}
    
    //! Re-seeds the random generator
    void seed( uint32_t seedValue );
    
    //! returns a random boolean value
    bool nextBool()
    {
        return mBase() & 1;
    }
    
    //! returns a random integer in the range [-2147483648,2147483647]
    int32_t nextInt()
    {
        return mBase();
    }
    
    //! returns a random integer in the range [0,4294967296)
    uint32_t nextUint()
    {
        return mBase();
    }
    
    //! returns a random integer in the range [0,v)
    int32_t nextInt( int32_t v )
    {
        if( v <= 0 ) return 0;
        return mBase() % v;
    }
    
    //! returns a random integer in the range [0,v)
    uint32_t nextUint( uint32_t v )
    {
        if( v == 0 ) return 0;
        return mBase() % v;
    }
    
    //! returns a random integer in the range [a,b)
    int32_t nextInt( int32_t a, int32_t b )
    {
        return nextInt( b - a ) + a;
    }
    
    //! returns a random float in the range [0.0f,1.0f)
    float nextFloat()
    {
        return mFloatGen(mBase);
    }
    
    //! returns a random float in the range [0.0f,v)
    float nextFloat( float v )
    {
        return mFloatGen(mBase) * v;
    }
    
    //! returns a random float in the range [a,b)
    float nextFloat( float a, float b )
    {
        return mFloatGen(mBase) * ( b - a ) + a;
    }
    
    //! returns a random float in the range [a,b] or the range [-b,-a)
    float posNegFloat( float a, float b )
    {
        if( nextBool() )
            return nextFloat( a, b );
        else
            return -nextFloat( a, b );
    }
    
    
    //! returns a random float via Gaussian distribution
    float nextGaussian()
    {
        if( mHaveNextNextGaussian ) {
            mHaveNextNextGaussian = false;
            return mNextNextGaussian;
        }
        else {
            float v1, v2, s;
            do {
                v1 = 2.0f * nextFloat() - 1.0f;
                v2 = 2.0f * nextFloat() - 1.0f;
                
                s = v1 * v1 + v2 * v2;
            }
            while( s >= 1.0f || s == 0.0f );
            
            float m = std::sqrt(-2.0f * std::log(s)/s);
            
            mNextNextGaussian       = v2 * m;
            mHaveNextNextGaussian   = true;
            
            return v1 * m;
        }
    }
    
    // STATICS
    //! Resets the static random generator to a random seed based on the clock
    static void randomize();
    
    //! Resets the static random generator to the specific seed \a seedValue
    static void    randSeed( uint32_t seedValue );
    
    //! returns a random boolean value
    static bool randBool()
    {
        return sBase() & 1;
    }
    
    //! returns a random integer in the range [-2147483648,2147483647]
    static int32_t randInt()
    {
        return sBase();
    }
    
    //! returns a random integer in the range [0,4294967296)
    static uint32_t randUint()
    {
        return sBase();
    }
    
    //! returns a random integer in the range [0,v)
    static int32_t randInt( int32_t v )
    {
        if( v <= 0 ) return 0;
        else return sBase() % v;
    }
    
    //! returns a random integer in the range [0,v)
    static uint32_t randUint( uint32_t v )
    {
        if( v == 0 ) return 0;
        else return sBase() % v;
    }
    
    //! returns a random integer in the range [a,b)
    static int32_t randInt( int32_t a, int32_t b )
    {
        return randInt( b - a ) + a;
    }
    
    //! returns a random float in the range [0.0f,1.0f)
    static float randFloat()
    {
        return sFloatGen(sBase);
    }
    
    //! returns a random float in the range [0.0f,v)
    static float randFloat( float v )
    {
        return sFloatGen(sBase) * v;
    }
    
    //! returns a random float in the range [a,b)
    static float randFloat( float a, float b )
    {
        return sFloatGen(sBase) * ( b - a ) + a;
    }
    
    //! returns a random float in the range [a,b) or the range [-b,-a)
    static float randPosNegFloat( float a, float b )
    {
        if( randBool() )
            return randFloat( a, b );
        else
            return -randFloat( a, b );
    }
    
    
    //! returns a random float via Gaussian distribution; refactor later
    static float randGaussian()
    {
        static bool  sHaveNextNextGaussian = false;
        static float sNextNextGaussian;
        
        if( sHaveNextNextGaussian ) {
            sHaveNextNextGaussian = false;
            return sNextNextGaussian;
        }
        else {
            float v1, v2, s;
            do {
                v1 = 2.0f * sFloatGen(sBase) - 1.0f;
                v2 = 2.0f * sFloatGen(sBase) - 1.0f;
                
                s = v1 * v1 + v2 * v2;
            }
            while( s >= 1.0f || s == 0.0f );
            
            float m = std::sqrt(-2.0f * std::log(s)/s);
            
            sNextNextGaussian       = v2 * m;
            sHaveNextNextGaussian   = true;
            
            return v1 * m;
        }
    }
    
private:
    std::mt19937 mBase;
    std::uniform_real_distribution<float>    mFloatGen;
    float    mNextNextGaussian;
    bool    mHaveNextNextGaussian;
    
    static std::mt19937 sBase;
    static std::uniform_real_distribution<float> sFloatGen;
};
